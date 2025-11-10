#!/usr/bin/env python3
"""
IMU Tester - MicroStrain 3DM-GX3-25 AHRS
Tests the Inertial Measurement Unit via serial communication
Reads: Roll, Pitch, Yaw, Angular Rates

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
            return config['imu']
    except FileNotFoundError:
        print(f"ERROR: Configuration file not found: {config_path}")
        sys.exit(1)
    except KeyError:
        print("ERROR: 'imu' configuration not found in devices.json")
        sys.exit(1)

# --- MicroStrain 3DM-GX3-25 Protocol Details ---
COMMAND_EULER_RATES = b'\xCF'
EXPECTED_RESPONSE_LENGTH = 31
POLL_INTERVAL_S = 0.1  # 100 ms

def calculate_checksum(data_bytes):
    """Calculates the 16-bit checksum by summing all bytes."""
    return sum(data_bytes)

def parse_response(response_bytes):
    """
    Parses the 31-byte response packet for the 0xCF command.
    Returns a dictionary of the data or None if parsing fails.
    """
    # 1. Check length
    if len(response_bytes) != EXPECTED_RESPONSE_LENGTH:
        print(f"Error: Expected {EXPECTED_RESPONSE_LENGTH} bytes, got {len(response_bytes)}")
        return None

    # 2. Check command echo
    if response_bytes[0] != COMMAND_EULER_RATES[0]:
        print(f"Error: Invalid command echo. Expected {COMMAND_EULER_RATES[0]:#02x}, got {response_bytes[0]:#02x}")
        return None

    # 3. Verify checksum
    data_for_checksum = response_bytes[:-2]
    received_checksum_bytes = response_bytes[-2:]

    calculated_checksum = calculate_checksum(data_for_checksum)
    received_checksum = struct.unpack('>H', received_checksum_bytes)[0]

    if calculated_checksum != received_checksum:
        print(f"Error: Checksum mismatch! Calculated: {calculated_checksum}, Received: {received_checksum}")
        return None

    # 4. Unpack the data (7 floats, big-endian)
    payload = response_bytes[1:-2]
    try:
        unpacked_data = struct.unpack('>7f', payload)
        return {
            "roll_deg": unpacked_data[0],
            "pitch_deg": unpacked_data[1],
            "yaw_deg": unpacked_data[2],
            "ang_rate_x_dps": unpacked_data[3],
            "ang_rate_y_dps": unpacked_data[4],
            "ang_rate_z_dps": unpacked_data[5],
            "timer": unpacked_data[6]
        }
    except struct.error as e:
        print(f"Error: Failed to unpack data. {e}")
        return None

def main():
    # Load configuration
    config = load_config()
    SERIAL_PORT = config['port']
    BAUD_RATE = config['baudRate']

    print("=" * 70)
    print("  IMU TESTER - MicroStrain 3DM-GX3-25 AHRS")
    print("=" * 70)
    print(f"Configuration:")
    print(f"  Port: {SERIAL_PORT}")
    print(f"  Baud Rate: {BAUD_RATE}")
    print(f"  Sampling Rate: {config['samplingRateHz']} Hz")
    print("=" * 70)
    print(f"Attempting to connect...")

    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            print("✓ Connection successful!")
            print("Starting continuous poll (Press Ctrl+C to stop)...")
            print("=" * 70)

            while True:
                # Send the command
                ser.write(COMMAND_EULER_RATES)

                # Read the response
                response = ser.read(EXPECTED_RESPONSE_LENGTH)

                # Parse and print
                if response:
                    data = parse_response(response)
                    if data:
                        print(
                            f"Roll: {data['roll_deg']:>8.2f}° | "
                            f"Pitch: {data['pitch_deg']:>8.2f}° | "
                            f"Yaw: {data['yaw_deg']:>8.2f}° | "
                            f"RateX: {data['ang_rate_x_dps']:>8.2f}°/s | "
                            f"RateY: {data['ang_rate_y_dps']:>8.2f}°/s | "
                            f"RateZ: {data['ang_rate_z_dps']:>8.2f}°/s"
                        )
                else:
                    print("⚠ Warning: No response from device (timeout).")

                # Wait for the next poll cycle
                time.sleep(POLL_INTERVAL_S)

    except serial.SerialException as e:
        print(f"✗ Error: Could not open serial port {SERIAL_PORT}")
        print(f"  {e}")
        print("  Please check the port name and ensure the device is connected.")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n" + "=" * 70)
        print("Polling stopped by user. Exiting.")
        print("=" * 70)

if __name__ == "__main__":
    main()
