#!/usr/bin/env python3
"""
Night Camera Tester - FLIR Boson 640 Thermal Camera (ENHANCED DIAGNOSTICS)
TAU2 Protocol with improved timing and buffer management
"""

import serial
import struct
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
            return config['video']['nightCamera']
    except FileNotFoundError:
        print(f"ERROR: Configuration file not found: {config_path}")
        sys.exit(1)
    except KeyError:
        print("ERROR: 'video.nightCamera' configuration not found in devices.json")
        sys.exit(1)

# --- TAU2 Protocol Implementation ---

def calculate_crc16(data, length):
    """Calculate CRC-16-CCITT checksum"""
    crc = 0x0000
    for i in range(length):
        crc ^= data[i] << 8
        for _ in range(8):
            if crc & 0x8000:
                crc = (crc << 1) ^ 0x1021
            else:
                crc <<= 1
            crc &= 0xFFFF
    return crc

def build_tau2_command(function_code, data=b''):
    """Build TAU2 protocol command packet"""
    packet = bytearray()
    
    # Header (6 bytes)
    packet.append(0x6E)  # Process Code
    packet.append(0x00)  # Status
    packet.append(0x00)  # Reserved
    packet.append(function_code)  # Function Code
    
    # Byte Count (2 bytes, big-endian)
    byte_count = len(data)
    packet.append((byte_count >> 8) & 0xFF)  # MSB
    packet.append(byte_count & 0xFF)         # LSB
    
    # CRC1 (Header CRC - first 6 bytes)
    crc1 = calculate_crc16(packet, 6)
    packet.append((crc1 >> 8) & 0xFF)  # MSB
    packet.append(crc1 & 0xFF)         # LSB
    
    # Data bytes
    packet.extend(data)
    
    # CRC2 (Full Packet CRC)
    crc2 = calculate_crc16(packet, len(packet))
    packet.append((crc2 >> 8) & 0xFF)  # MSB
    packet.append(crc2 & 0xFF)         # LSB
    
    return bytes(packet)

def parse_tau2_response(response):
    """Parse TAU2 protocol response packet"""
    if len(response) < 10:
        return (False, 0, 0, b'')
    
    # Check Process Code
    if response[0] != 0x6E:
        print(f"   ✗ Invalid Process Code: 0x{response[0]:02X} (expected 0x6E)")
        return (False, 0, 0, b'')
    
    # Extract fields
    status_byte = response[1]
    function_code = response[3]
    byte_count = (response[4] << 8) | response[5]
    
    # Verify packet size
    expected_size = 6 + byte_count + 2 + 2
    if len(response) < expected_size:
        print(f"   ✗ Incomplete packet: got {len(response)}, expected {expected_size}")
        return (False, function_code, status_byte, b'')
    
    # Verify CRC1
    received_crc1 = (response[6] << 8) | response[7]
    calculated_crc1 = calculate_crc16(response[:6], 6)
    if received_crc1 != calculated_crc1:
        print(f"   ✗ CRC1 mismatch: calc=0x{calculated_crc1:04X}, recv=0x{received_crc1:04X}")
        return (False, function_code, status_byte, b'')
    
    # Verify CRC2
    received_crc2 = (response[-2] << 8) | response[-1]
    calculated_crc2 = calculate_crc16(response[:-2], len(response) - 2)
    if received_crc2 != calculated_crc2:
        print(f"   ✗ CRC2 mismatch: calc=0x{calculated_crc2:04X}, recv=0x{received_crc2:04X}")
        return (False, function_code, status_byte, b'')
    
    # Extract data
    data = response[8:8 + byte_count] if byte_count > 0 else b''
    
    # Check status byte
    if status_byte != 0x00:
        error_messages = {
            0x01: "Camera is busy",
            0x02: "Camera not ready",
            0x03: "Data out of range",
            0x04: "Checksum error",
            0x05: "Undefined process code",
            0x06: "Undefined function code",
            0x07: "Command timeout",
            0x09: "Byte count mismatch",
            0x0A: "Feature not enabled"
        }
        error_msg = error_messages.get(status_byte, f"Unknown: 0x{status_byte:02X}")
        print(f"   ✗ Camera Error (0x{status_byte:02X}): {error_msg}")
        return (False, function_code, status_byte, data)
    
    return (True, function_code, status_byte, data)

# Function Codes
FN_DO_FFC = 0x0B
FN_GET_CAMERA_STATUS = 0x06
FN_SET_ZOOM = 0x0F
FN_SET_VIDEO_LUT = 0x10

def send_tau2_command_improved(ser, command, max_wait=2.0, verbose=True):
    """
    Send TAU2 command with improved response handling
    Waits for complete response packet
    """
    # Clear buffers
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    
    # Small delay before sending
    time.sleep(0.05)
    
    # Send command
    bytes_sent = ser.write(command)
    ser.flush()  # Ensure data is transmitted
    
    if verbose:
        print(f"   → Sent {bytes_sent} bytes: {command.hex().upper()}")
    
    # Wait and accumulate response
    response = bytearray()
    start_time = time.time()
    last_read_time = start_time
    read_timeout = 0.1  # 100ms between reads
    
    while (time.time() - start_time) < max_wait:
        # Check if data is available
        if ser.in_waiting > 0:
            chunk = ser.read(ser.in_waiting)
            response.extend(chunk)
            last_read_time = time.time()
            
            if verbose and len(chunk) > 0:
                print(f"   ← Chunk ({len(chunk)} bytes): {chunk.hex().upper()}")
        
        # If we have data and no new data for read_timeout, assume packet complete
        if len(response) > 0 and (time.time() - last_read_time) > read_timeout:
            break
        
        time.sleep(0.01)  # Small sleep to avoid CPU spinning
    
    if len(response) > 0:
        print(f"   ← Total received: {len(response)} bytes: {response.hex().upper()}")
    else:
        print("   ← No response received")
    
    return bytes(response)

def main():
    config = load_config()
    SERIAL_PORT = config['controlPort']
    BAUD_RATE = 921600

    print("=" * 80)
    print("  NIGHT CAMERA TESTER - FLIR Boson 640 (TAU2 Protocol) - ENHANCED")
    print("=" * 80)
    print(f"Configuration:")
    print(f"  Control Port: {SERIAL_PORT}")
    print(f"  Video Device: {config['devicePath']}")
    print(f"  Baud Rate: {BAUD_RATE}")
    print("=" * 80)

    try:
        print(f"\nOpening serial port...")
        with serial.Serial(
            SERIAL_PORT, 
            BAUD_RATE,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=0.1,  # Non-blocking reads
            write_timeout=1.0
        ) as ser:
            print("✓ Connection successful!")
            
            # Wait for camera to stabilize
            print("\nWaiting for camera to stabilize (2 seconds)...")
            time.sleep(2.0)
            
            # Check for any initial data in buffer
            if ser.in_waiting > 0:
                initial_data = ser.read(ser.in_waiting)
                print(f"⚠ Initial buffer contained {len(initial_data)} bytes: {initial_data.hex().upper()}")
                print("   (This is normal - could be startup messages)")
            
            print("=" * 80)

            # Test 1: Get Camera Status
            print("\n[TEST 1] Requesting camera status (Function 0x06)...")
            cmd = build_tau2_command(FN_GET_CAMERA_STATUS, b'\x00\x00')
            response = send_tau2_command_improved(ser, cmd, max_wait=1.0)
            success, fn, status, data = parse_tau2_response(response)
            if success:
                if len(data) > 0:
                    camera_status = data[0]
                    print(f"   ✓ SUCCESS - Camera Status: 0x{camera_status:02X}")
                else:
                    print("   ✓ SUCCESS - Acknowledged (no data returned)")
            elif len(response) == 0:
                print("   ✗ FAIL - No response (timeout)")
            else:
                print("   ✗ FAIL - Invalid response packet")
            
            time.sleep(0.5)

            # Test 2: Perform FFC
            print("\n[TEST 2] Triggering FFC - Flat Field Correction (Function 0x0B)...")
            cmd = build_tau2_command(FN_DO_FFC, b'\x00\x01')
            response = send_tau2_command_improved(ser, cmd, max_wait=3.0)
            success, fn, status, data = parse_tau2_response(response)
            if success:
                print("   ✓ SUCCESS - FFC triggered")
            elif len(response) == 0:
                print("   ✗ FAIL - No response (timeout)")
            else:
                print("   ✗ FAIL - Invalid response packet")
            
            # Wait for FFC to complete
            print("   ⏳ Waiting for FFC to complete (3 seconds)...")
            time.sleep(3.0)

            # Test 3: Set Digital Zoom to 4x
            print("\n[TEST 3] Setting digital zoom to 4x (Function 0x0F)...")
            cmd = build_tau2_command(FN_SET_ZOOM, b'\x00\x04')
            response = send_tau2_command_improved(ser, cmd, max_wait=1.0)
            success, fn, status, data = parse_tau2_response(response)
            if success:
                print("   ✓ SUCCESS - Zoom set to 4x")
            elif len(response) == 0:
                print("   ✗ FAIL - No response (timeout)")
            else:
                print("   ✗ FAIL - Invalid response packet")
            
            time.sleep(0.5)

            # Test 4: Disable Digital Zoom
            print("\n[TEST 4] Disabling digital zoom (Function 0x0F)...")
            cmd = build_tau2_command(FN_SET_ZOOM, b'\x00\x00')
            response = send_tau2_command_improved(ser, cmd, max_wait=1.0)
            success, fn, status, data = parse_tau2_response(response)
            if success:
                print("   ✓ SUCCESS - Zoom disabled (1x)")
            elif len(response) == 0:
                print("   ✗ FAIL - No response (timeout)")
            else:
                print("   ✗ FAIL - Invalid response packet")
            
            time.sleep(0.5)

            # Test 5: Set Video LUT Mode
            print("\n[TEST 5] Setting video LUT mode to 3 - WhiteHot (Function 0x10)...")
            cmd = build_tau2_command(FN_SET_VIDEO_LUT, b'\x00\x03')
            response = send_tau2_command_improved(ser, cmd, max_wait=1.0)
            success, fn, status, data = parse_tau2_response(response)
            if success:
                print("   ✓ SUCCESS - Video LUT mode set to 3")
            elif len(response) == 0:
                print("   ✗ FAIL - No response (timeout)")
            else:
                print("   ✗ FAIL - Invalid response packet")

            print("\n" + "=" * 80)
            print("Camera test completed!")
            print("=" * 80)

    except serial.SerialException as e:
        print(f"\n✗ ERROR: Could not open serial port {SERIAL_PORT}")
        print(f"  {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n" + "=" * 80)
        print("Test interrupted by user.")
        print("=" * 80)

if __name__ == "__main__":
    main()
