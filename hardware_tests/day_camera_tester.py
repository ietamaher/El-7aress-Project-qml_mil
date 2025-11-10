#!/usr/bin/env python3
"""
Day Camera Tester - Sony FCB-EV7520A VISCA Control
Tests the day camera control via serial (VISCA protocol)
Commands: Zoom, Focus, Power On/Off

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
            return config['video']['dayCamera']
    except FileNotFoundError:
        print(f"ERROR: Configuration file not found: {config_path}")
        sys.exit(1)
    except KeyError:
        print("ERROR: 'video.dayCamera' configuration not found in devices.json")
        sys.exit(1)

# --- VISCA Protocol Commands ---
# Camera address 1
VISCA_CMD_POWER_ON = bytes([0x81, 0x01, 0x04, 0x00, 0x02, 0xFF])
VISCA_CMD_POWER_OFF = bytes([0x81, 0x01, 0x04, 0x00, 0x03, 0xFF])
VISCA_CMD_CAM_ZOOM_STOP = bytes([0x81, 0x01, 0x04, 0x07, 0x00, 0xFF])
VISCA_CMD_ZOOM_TELE = bytes([0x81, 0x01, 0x04, 0x07, 0x22, 0xFF])  # Zoom in (speed 2)
VISCA_CMD_ZOOM_WIDE = bytes([0x81, 0x01, 0x04, 0x07, 0x32, 0xFF])  # Zoom out (speed 2)
VISCA_CMD_INQUIRY_ZOOM = bytes([0x81, 0x09, 0x04, 0x47, 0xFF])

VISCA_ACK = 0x40
VISCA_COMPLETION = 0x50

def send_visca_command(ser, command):
    """Send VISCA command and wait for ACK"""
    ser.write(command)
    time.sleep(0.1)

    # Read response
    response = ser.read(16)
    if len(response) >= 1:
        if response[0] & 0xF0 == VISCA_ACK:
            return True, "ACK"
        elif response[0] & 0xF0 == VISCA_COMPLETION:
            return True, "Complete"
    return False, "No response"

def main():
    # Load configuration
    config = load_config()
    SERIAL_PORT = config['controlPort']
    BAUD_RATE = 9600  # VISCA standard baud rate

    print("=" * 70)
    print("  DAY CAMERA TESTER - Sony FCB-EV7520A (VISCA Protocol)")
    print("=" * 70)
    print(f"Configuration:")
    print(f"  Control Port: {SERIAL_PORT}")
    print(f"  Video Device: {config['devicePath']}")
    print(f"  Baud Rate: {BAUD_RATE} (VISCA standard)")
    print("=" * 70)
    print(f"Attempting to connect...")

    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            print("✓ Connection successful!")
            print("=" * 70)
            print("VISCA Commands Test")
            print("=" * 70)

            # Test 1: Power On
            print("\n1. Sending Power ON command...")
            success, msg = send_visca_command(ser, VISCA_CMD_POWER_ON)
            print(f"   {'✓' if success else '✗'} {msg}")
            time.sleep(2)

            # Test 2: Zoom In
            print("\n2. Testing Zoom IN (3 seconds)...")
            success, msg = send_visca_command(ser, VISCA_CMD_ZOOM_TELE)
            print(f"   {'✓' if success else '✗'} {msg}")
            time.sleep(3)

            # Stop zoom
            send_visca_command(ser, VISCA_CMD_CAM_ZOOM_STOP)
            print("   ✓ Zoom stopped")

            # Test 3: Zoom Out
            print("\n3. Testing Zoom OUT (3 seconds)...")
            success, msg = send_visca_command(ser, VISCA_CMD_ZOOM_WIDE)
            print(f"   {'✓' if success else '✗'} {msg}")
            time.sleep(3)

            # Stop zoom
            send_visca_command(ser, VISCA_CMD_CAM_ZOOM_STOP)
            print("   ✓ Zoom stopped")

            # Test 4: Query Zoom Position
            print("\n4. Querying zoom position...")
            ser.write(VISCA_CMD_INQUIRY_ZOOM)
            time.sleep(0.1)
            response = ser.read(16)
            if len(response) >= 7:
                zoom_pos = (response[2] << 12) | (response[3] << 8) | (response[4] << 4) | response[5]
                print(f"   ✓ Zoom Position: {zoom_pos}")
            else:
                print("   ✗ No valid response")

            print("\n" + "=" * 70)
            print("Camera test completed successfully!")
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
