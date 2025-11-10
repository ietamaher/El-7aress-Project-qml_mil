#!/usr/bin/env python3
"""
Actuator Tester - Gun Actuator / Trigger Control
Tests the weapon actuator via serial communication
Reads: Position, status

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
            return config['actuator']
    except FileNotFoundError:
        print(f"ERROR: Configuration file not found: {config_path}")
        sys.exit(1)
    except KeyError:
        print("ERROR: 'actuator' configuration not found in devices.json")
        sys.exit(1)

# --- Actuator Protocol (Example - adapt to your actuator) ---
CMD_GET_STATUS = b'S\r\n'     # Example: Get status command
CMD_GET_POSITION = b'P\r\n'   # Example: Get position command

POLL_INTERVAL_S = 0.5  # 500 ms

def parse_response(line):
    """Parse actuator response - adjust for your protocol"""
    try:
        line = line.strip()
        # Example: Response might be "STATUS:OK" or "POSITION:1234"
        if ':' in line:
            key, value = line.split(':', 1)
            return {key: value}
        return {'raw': line}
    except Exception as e:
        return {'error': str(e)}

def main():
    # Load configuration
    config = load_config()
    SERIAL_PORT = config['port']
    BAUD_RATE = config['baudRate']

    print("=" * 70)
    print("  ACTUATOR TESTER - Gun Actuator / Trigger Control")
    print("=" * 70)
    print(f"Configuration:")
    print(f"  Port: {SERIAL_PORT}")
    print(f"  Baud Rate: {BAUD_RATE}")
    print("=" * 70)
    print("⚠ WARNING: DO NOT TEST WITH LIVE AMMUNITION")
    print("=" * 70)
    print(f"Attempting to connect...")

    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            print("✓ Connection successful!")
            print("Starting status polling (Press Ctrl+C to stop)...")
            print("=" * 70)
            print("NOTE: Adjust commands for your specific actuator model")
            print("=" * 70)

            while True:
                # Request status
                ser.write(CMD_GET_STATUS)
                time.sleep(0.1)

                # Read response
                response = ser.readline()
                if response:
                    try:
                        line = response.decode('ascii', errors='ignore')
                        data = parse_response(line)
                        print(f"Status: {data} | Raw: {line.strip()}")
                    except UnicodeDecodeError:
                        print(f"Decode error: {response.hex()}")
                else:
                    print("⚠ No response")

                time.sleep(POLL_INTERVAL_S)

    except serial.SerialException as e:
        print(f"✗ Error: Could not open serial port {SERIAL_PORT}")
        print(f"  {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n" + "=" * 70)
        print("Polling stopped by user. Exiting.")
        print("=" * 70)

if __name__ == "__main__":
    main()
