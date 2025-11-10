#!/usr/bin/env python3
"""
LRF Tester - Laser Range Finder
Tests the Laser Range Finder via serial communication
Reads distance measurements (50m - 4000m range)

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

# --- LRF Protocol (Example - adapt to your actual LRF protocol) ---
COMMAND_GET_DISTANCE = b'D\r\n'  # Example command, adjust as needed
POLL_INTERVAL_S = 0.5  # 500 ms

def parse_distance(line):
    """
    Parse distance from LRF response
    Adjust this based on your actual LRF protocol
    """
    try:
        # Example: Response might be "D:1234.5\r\n" or just "1234.5\r\n"
        line = line.strip()

        # Try to extract numeric value
        if ':' in line:
            distance_str = line.split(':')[1]
        else:
            distance_str = line

        distance = float(distance_str)
        return distance
    except (ValueError, IndexError) as e:
        print(f"Parse error: {e} | Raw: {line}")
        return None

def main():
    # Load configuration
    config = load_config()
    SERIAL_PORT = config['port']
    BAUD_RATE = config['baudRate']
    MIN_RANGE = config['minRange']
    MAX_RANGE = config['maxRange']

    print("=" * 70)
    print("  LRF TESTER - Laser Range Finder")
    print("=" * 70)
    print(f"Configuration:")
    print(f"  Port: {SERIAL_PORT}")
    print(f"  Baud Rate: {BAUD_RATE}")
    print(f"  Range: {MIN_RANGE}m - {MAX_RANGE}m")
    print("=" * 70)
    print(f"Attempting to connect...")

    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            print("✓ Connection successful!")
            print("Starting continuous measurements (Press Ctrl+C to stop)...")
            print("=" * 70)
            print("NOTE: Adjust COMMAND_GET_DISTANCE and parse_distance() for your LRF model")
            print("=" * 70)

            while True:
                # Send distance request command
                ser.write(COMMAND_GET_DISTANCE)

                # Read response (up to newline)
                response = ser.readline()

                if response:
                    try:
                        line = response.decode('ascii', errors='ignore')
                        distance = parse_distance(line)

                        if distance is not None:
                            # Check if in valid range
                            if MIN_RANGE <= distance <= MAX_RANGE:
                                status = "✓"
                            else:
                                status = "⚠"

                            print(f"{status} Distance: {distance:>8.2f} m | Raw: {line.strip()}")
                        else:
                            print(f"✗ Invalid response: {line.strip()}")
                    except UnicodeDecodeError:
                        print(f"✗ Decode error: {response.hex()}")
                else:
                    print("⚠ No response (timeout)")

                time.sleep(POLL_INTERVAL_S)

    except serial.SerialException as e:
        print(f"✗ Error: Could not open serial port {SERIAL_PORT}")
        print(f"  {e}")
        print("  Please check the port name and ensure the device is connected.")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n" + "=" * 70)
        print("Measurements stopped by user. Exiting.")
        print("=" * 70)

if __name__ == "__main__":
    main()
