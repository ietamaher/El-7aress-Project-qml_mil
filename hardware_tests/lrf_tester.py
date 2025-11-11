#!/usr/bin/env python3
"""
LRF Tester - Laser Range Finder (SAFE DIAGNOSTIC MODE)
Tests the LRF via serial communication - DIAGNOSTICS ONLY
Reads: Status, Temperature, Product Info, Laser Shot Count

⚠️ SAFETY: NO DISTANCE MEASUREMENTS - DIAGNOSTICS ONLY
This script will NOT fire the laser or measure distances.

Protocol: Jioptics LRF Binary Protocol
- 9-byte fixed packets
- 115200 baud, 8-N-1
- Frame: 0xEE | 0x07 | CMD | PARAMS(5 bytes) | CHECKSUM

Configuration loaded from: ../config/devices.json
"""

import serial
import time
import json
import os
import sys

# --- Load Configuration ---
def load_config():
    """Load device configuration from devices.json"""
    config_path = os.path.join(os.path.dirname(__file__), '..', 'config', 'devices.json')
    try:
        with open(config_path, 'r') as f:
            config = json.load(f)
            return config['lrf']
    except FileNotFoundError:
        print(f"ERROR: Configuration file not found: {config_path}")
        sys.exit(1)
    except KeyError:
        print("ERROR: 'lrf' configuration not found in devices.json")
        sys.exit(1)

# --- LRF Protocol Constants ---
FRAME_HEADER = 0xEE
DEVICE_CODE_LRF = 0x07
PACKET_SIZE = 9

# Command Codes (Diagnostics Only - NO Laser Firing)
CMD_SELF_TEST = 0x01              # Self-test (returns status)
CMD_TEMPERATURE_READING = 0x06    # Get internal temperature
CMD_PULSE_COUNT_REPORT = 0x0A    # Get accumulated laser shot count
CMD_PRODUCT_INFO = 0x10           # Get device info (ID, firmware version)

POLL_INTERVAL_S = 2.0  # 2 seconds between checks

def calculate_checksum(body):
    """Calculate checksum: sum of 6-byte body"""
    return sum(body) & 0xFF

def build_command(command_code):
    """Build a 9-byte command packet"""
    # Body: command code + 5 parameter bytes (all zeros for diagnostics)
    body = bytes([command_code, 0x00, 0x00, 0x00, 0x00, 0x00])
    checksum = calculate_checksum(body)
    
    # Full packet: Header | Device | Body | Checksum
    packet = bytes([FRAME_HEADER, DEVICE_CODE_LRF]) + body + bytes([checksum])
    return packet

def verify_checksum(packet):
    """Verify packet checksum"""
    if len(packet) != PACKET_SIZE:
        return False
    body = packet[2:8]  # Bytes 2-7 (6 bytes)
    received_checksum = packet[8]
    calculated_checksum = calculate_checksum(body)
    return received_checksum == calculated_checksum

def send_command(ser, command_code, description):
    """Send a diagnostic command and read binary response"""
    try:
        packet = build_command(command_code)
        ser.write(packet)
        time.sleep(0.1)  # Wait for response

        # Read 9-byte response
        response = ser.read(PACKET_SIZE)
        if len(response) == PACKET_SIZE:
            if response[0] == FRAME_HEADER and response[1] == DEVICE_CODE_LRF:
                if verify_checksum(response):
                    return response
                else:
                    return f"[Checksum Error: {response.hex()}]"
            else:
                return f"[Invalid Header: {response.hex()}]"
        elif len(response) > 0:
            return f"[Incomplete: {response.hex()}]"
        else:
            return "[No response]"
    except Exception as e:
        return f"[Error: {e}]"

def parse_self_test_response(response):
    """Parse self-test response (returns status flags)"""
    if isinstance(response, bytes) and len(response) == PACKET_SIZE:
        status1 = response[3]  # Byte 4: Status1
        status0 = response[4]  # Byte 5: Status0
        
        is_fault = (status1 == 0x01)
        no_echo = (status0 & 0x08) >> 3
        laser_not_out = (status0 & 0x10) >> 4
        over_temperature = (status0 & 0x20) >> 5
        
        return {
            'fault': is_fault,
            'no_echo': no_echo,
            'laser_not_out': laser_not_out,
            'over_temp': over_temperature,
            'status0': status0,
            'status1': status1
        }
    return None

def parse_temperature_response(response):
    """Parse temperature response"""
    if isinstance(response, bytes) and len(response) == PACKET_SIZE:
        temp_byte = response[4]  # Byte 5: Temp
        
        # Sign-magnitude format: Bit7=sign, Bit6-0=value
        temp_value = temp_byte & 0x7F
        if temp_byte & 0x80:  # Negative
            temp_value = -temp_value
        
        return temp_value
    return None

def parse_product_info_response(response):
    """Parse product info response"""
    if isinstance(response, bytes) and len(response) == PACKET_SIZE:
        product_id = response[3]  # Byte 4: ID
        version_byte = response[4]  # Byte 5: Version
        
        main_version = (version_byte & 0xF0) >> 4
        sub_version = (version_byte & 0x0F)
        
        return {
            'product_id': f"0x{product_id:02X}",
            'version': f"{main_version}.{sub_version}"
        }
    return None

def parse_pulse_count_response(response):
    """Parse laser shot count response"""
    if isinstance(response, bytes) and len(response) == PACKET_SIZE:
        pnum_l = response[5]  # Byte 6: PNUM_L
        pnum_h = response[6]  # Byte 7: PNUM_H
        
        pulse_base = (pnum_h << 8) | pnum_l
        # Actual count = pulse_base * 100
        laser_count = pulse_base * 100
        
        return laser_count
    return None

def main():
    # Load configuration
    config = load_config()
    SERIAL_PORT = config['port']
    # LRF always uses 115200 baud per Jioptics spec
    BAUD_RATE = 115200

    print("=" * 70)
    print("  LRF TESTER - SAFE DIAGNOSTIC MODE")
    print("=" * 70)
    print("⚠️  SAFETY: This script performs DIAGNOSTICS ONLY")
    print("    NO distance measurements - NO laser firing")
    print("=" * 70)
    print(f"Configuration:")
    print(f"  Port: {SERIAL_PORT}")
    print(f"  Baud Rate: {BAUD_RATE} (fixed by protocol)")
    print(f"  Protocol: Jioptics Binary (9-byte packets)")
    print("=" * 70)
    print(f"Attempting to connect...")

    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1,
                          bytesize=8, parity='N', stopbits=1) as ser:
            print("✓ Connection successful!")
            print("=" * 70)
            print("Starting diagnostic monitoring (Press Ctrl+C to stop)...")
            print("=" * 70)

            cycle_count = 0

            while True:
                cycle_count += 1
                print(f"\n[Cycle {cycle_count}] {time.strftime('%H:%M:%S')}")
                print("-" * 70)

                # 1. Self-Test (Status Check)
                print("1. Self-Test Status:   ", end="")
                status_response = send_command(ser, CMD_SELF_TEST, "Self-Test")
                if isinstance(status_response, bytes):
                    status_data = parse_self_test_response(status_response)
                    if status_data:
                        if status_data['fault']:
                            print(f"✗ FAULT")
                        else:
                            print(f"✓ OK")
                        
                        # Show detailed flags
                        flags = []
                        if status_data['no_echo']:
                            flags.append("NoEcho")
                        if status_data['laser_not_out']:
                            flags.append("LaserNotOut")
                        if status_data['over_temp']:
                            flags.append("⚠OverTemp")
                        
                        if flags:
                            print(f"   Flags: {', '.join(flags)}")
                        print(f"   Status Bytes: 0x{status_data['status1']:02X}, 0x{status_data['status0']:02X}")
                    else:
                        print(f"? Parse error")
                else:
                    print(f"{status_response}")

                # 2. Temperature Reading
                print("2. Temperature:        ", end="")
                temp_response = send_command(ser, CMD_TEMPERATURE_READING, "Temperature")
                if isinstance(temp_response, bytes):
                    temp = parse_temperature_response(temp_response)
                    if temp is not None:
                        # Check temperature ranges
                        if temp < 0 or temp > 60:
                            indicator = "⚠"
                        else:
                            indicator = "✓"
                        print(f"{indicator} {temp:>3d}°C")
                    else:
                        print(f"? Parse error")
                else:
                    print(f"{temp_response}")

                # 3. Laser Shot Count
                print("3. Laser Shot Count:   ", end="")
                count_response = send_command(ser, CMD_PULSE_COUNT_REPORT, "Pulse Count")
                if isinstance(count_response, bytes):
                    laser_count = parse_pulse_count_response(count_response)
                    if laser_count is not None:
                        print(f"{laser_count:,} shots")
                    else:
                        print(f"? Parse error")
                else:
                    print(f"{count_response}")

                # 4. Product Info (only first cycle to avoid spam)
                if cycle_count == 1:
                    print("\n--- Device Information (One-time) ---")
                    info_response = send_command(ser, CMD_PRODUCT_INFO, "Product Info")
                    if isinstance(info_response, bytes):
                        prod_info = parse_product_info_response(info_response)
                        if prod_info:
                            print(f"Product ID: {prod_info['product_id']}")
                            print(f"Firmware Version: {prod_info['version']}")
                        else:
                            print(f"? Parse error")
                    else:
                        print(f"{info_response}")
                    print("-" * 70)

                # Wait before next cycle
                time.sleep(POLL_INTERVAL_S)

    except serial.SerialException as e:
        print(f"✗ Error: Could not open serial port {SERIAL_PORT}")
        print(f"  {e}")
        print("  Please check the port name and ensure the device is connected.")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n" + "=" * 70)
        print("Diagnostic monitoring stopped by user. Exiting.")
        print("=" * 70)
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
