#!/usr/bin/env python3
"""
Night Camera Tester - FLIR Boson 640 Thermal Camera
Tests the thermal camera control via serial (TAU2 protocol)
Commands: FFC, Digital Zoom, Video Mode

Configuration loaded from: ../config/devices.json
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

# --- TAU2/Boson Protocol Commands (simplified examples) ---
# NOTE: Actual Boson protocol is more complex - refer to FLIR documentation

def create_boson_command(function_id, args=bytes()):
    """Create a Boson camera command packet"""
    # Simplified packet structure: 0x8E + length + function_id + args + checksum
    packet = b'\x8E'  # Start byte
    length = len(args) + 2  # function_id + data
    packet += struct.pack('B', length)
    packet += struct.pack('B', function_id)
    packet += args

    # Simple checksum (actual Boson uses CRC16)
    checksum = sum(packet[1:]) & 0xFFFF
    packet += struct.pack('>H', checksum)

    return packet

# Example function IDs (refer to Boson SDK for actual values)
FN_DO_FFC = 0x0C          # Perform Flat Field Correction
FN_GET_CAMERA_SN = 0x05   # Get camera serial number
FN_SET_ZOOM = 0x17        # Set digital zoom

def send_boson_command(ser, command):
    """Send command and read response"""
    ser.write(command)
    time.sleep(0.2)

    # Read response (simplified)
    response = ser.read(64)
    return response

def main():
    # Load configuration
    config = load_config()
    SERIAL_PORT = config['controlPort']
    BAUD_RATE = 921600  # Boson typical baud rate

    print("=" * 70)
    print("  NIGHT CAMERA TESTER - FLIR Boson 640 Thermal Camera")
    print("=" * 70)
    print(f"Configuration:")
    print(f"  Control Port: {SERIAL_PORT}")
    print(f"  Video Device: {config['devicePath']}")
    print(f"  Baud Rate: {BAUD_RATE}")
    print("=" * 70)
    print("NOTE: This is a simplified tester. Refer to FLIR Boson SDK for full protocol.")
    print("=" * 70)
    print(f"Attempting to connect...")

    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            print("✓ Connection successful!")
            print("=" * 70)

            # Test 1: Get Camera Serial Number
            print("\n1. Requesting camera serial number...")
            cmd = create_boson_command(FN_GET_CAMERA_SN)
            response = send_boson_command(ser, cmd)
            if len(response) > 0:
                print(f"   ✓ Response: {response.hex()}")
            else:
                print("   ✗ No response")

            # Test 2: Perform FFC (Flat Field Correction)
            print("\n2. Triggering FFC (Flat Field Correction)...")
            cmd = create_boson_command(FN_DO_FFC)
            response = send_boson_command(ser, cmd)
            print("   ✓ FFC command sent")
            time.sleep(2)

            # Test 3: Set Digital Zoom
            print("\n3. Setting digital zoom to 2x...")
            zoom_level = struct.pack('B', 1)  # Example: 1 = 2x zoom
            cmd = create_boson_command(FN_SET_ZOOM, zoom_level)
            response = send_boson_command(ser, cmd)
            print("   ✓ Zoom command sent")

            print("\n" + "=" * 70)
            print("Camera test completed!")
            print("For full protocol support, use FLIR Boson SDK")
            print("=" * 70)

    except serial.SerialException as e:
        print(f"✗ Error: Could not open serial port {SERIAL_PORT}")
        print(f"  {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n" + "=" * 70)
        print("Test interrupted by user. Exiting.")
        print("=" * 70)

if __name__ == "__main__":
    main()
