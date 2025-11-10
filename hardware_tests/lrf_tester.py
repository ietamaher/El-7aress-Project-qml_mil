#!/usr/bin/env python3
"""
LRF Tester - Laser Range Finder (SAFE DIAGNOSTIC MODE)
Tests the LRF via serial communication - DIAGNOSTICS ONLY
Reads: Status, Temperature, Error Codes, Device Info

⚠️ SAFETY: NO DISTANCE MEASUREMENTS - DIAGNOSTICS ONLY
This script will NOT fire the laser or measure distances.

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

# --- LRF SAFE Commands (Diagnostics Only - NO Laser Firing) ---
# Adjust these commands based on your specific LRF model manual

CMD_GET_STATUS = b'S\r\n'           # Get system status
CMD_GET_TEMPERATURE = b'T\r\n'      # Get internal temperature
CMD_GET_ERROR_CODE = b'E\r\n'       # Get error/fault codes
CMD_GET_DEVICE_INFO = b'I\r\n'      # Get device info (SN, firmware, etc.)
CMD_GET_VOLTAGE = b'V\r\n'          # Get power supply voltage
CMD_SELF_TEST = b'X\r\n'            # Self-test (no laser)

POLL_INTERVAL_S = 2.0  # 2 seconds between checks

def send_command(ser, command, description):
    """Send a diagnostic command and read response"""
    try:
        ser.write(command)
        time.sleep(0.2)  # Wait for response

        response = ser.readline()
        if response:
            try:
                line = response.decode('ascii', errors='ignore').strip()
                return line
            except UnicodeDecodeError:
                return f"[Binary: {response.hex()}]"
        else:
            return "[No response]"
    except Exception as e:
        return f"[Error: {e}]"

def parse_temperature(response):
    """Parse temperature response - adjust for your LRF"""
    try:
        # Example formats: "T:25.5" or "TEMP:25.5C" or just "25.5"
        response = response.replace('T:', '').replace('TEMP:', '').replace('C', '').replace('°', '').strip()
        temp = float(response)
        return temp
    except (ValueError, AttributeError):
        return None

def parse_status(response):
    """Parse status response - adjust for your LRF"""
    # Example: "STATUS:OK" or "READY" or hex code
    response = response.upper()
    if 'OK' in response or 'READY' in response:
        return '✓ OK'
    elif 'ERROR' in response or 'FAULT' in response:
        return '✗ ERROR'
    elif 'WARN' in response:
        return '⚠ WARNING'
    else:
        return f'? {response}'

def main():
    # Load configuration
    config = load_config()
    SERIAL_PORT = config['port']
    BAUD_RATE = config['baudRate']

    print("=" * 70)
    print("  LRF TESTER - SAFE DIAGNOSTIC MODE")
    print("=" * 70)
    print("⚠️  SAFETY: This script performs DIAGNOSTICS ONLY")
    print("    NO distance measurements - NO laser firing")
    print("=" * 70)
    print(f"Configuration:")
    print(f"  Port: {SERIAL_PORT}")
    print(f"  Baud Rate: {BAUD_RATE}")
    print("=" * 70)
    print("NOTE: Adjust commands (CMD_GET_*) based on your LRF model manual")
    print("=" * 70)
    print(f"Attempting to connect...")

    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            print("✓ Connection successful!")
            print("=" * 70)
            print("Starting diagnostic monitoring (Press Ctrl+C to stop)...")
            print("=" * 70)

            cycle_count = 0

            while True:
                cycle_count += 1
                print(f"\n[Cycle {cycle_count}] {time.strftime('%H:%M:%S')}")
                print("-" * 70)

                # 1. Get Device Status
                print("1. Device Status:      ", end="")
                status_response = send_command(ser, CMD_GET_STATUS, "Status")
                status = parse_status(status_response)
                print(f"{status} | Raw: {status_response}")

                # 2. Get Temperature
                print("2. Temperature:        ", end="")
                temp_response = send_command(ser, CMD_GET_TEMPERATURE, "Temperature")
                temp = parse_temperature(temp_response)
                if temp is not None:
                    # Check temperature ranges
                    if temp < 0 or temp > 60:
                        indicator = "⚠"
                    else:
                        indicator = "✓"
                    print(f"{indicator} {temp:>5.1f}°C | Raw: {temp_response}")
                else:
                    print(f"? {temp_response}")

                # 3. Get Error Codes
                print("3. Error Codes:        ", end="")
                error_response = send_command(ser, CMD_GET_ERROR_CODE, "Error")
                if 'NO ERROR' in error_response.upper() or error_response == '0':
                    print(f"✓ No errors | Raw: {error_response}")
                else:
                    print(f"⚠ {error_response}")

                # 4. Get Supply Voltage
                print("4. Supply Voltage:     ", end="")
                voltage_response = send_command(ser, CMD_GET_VOLTAGE, "Voltage")
                print(f"{voltage_response}")

                # 5. Device Info (only first cycle to avoid spam)
                if cycle_count == 1:
                    print("\n--- Device Information (One-time) ---")
                    info_response = send_command(ser, CMD_GET_DEVICE_INFO, "Info")
                    print(f"Device Info: {info_response}")

                    # Self-test (no laser)
                    print("Running self-test (no laser)...", end="")
                    test_response = send_command(ser, CMD_SELF_TEST, "Self-Test")
                    print(f" {test_response}")
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
        sys.exit(1)

if __name__ == "__main__":
    main()
