#!/usr/bin/env python3
"""
Servo Azimuth Tester - Azimuth Servo Driver
Tests the azimuth servo via Modbus RTU
Reads: Position, velocity, status, alarms, temperatures

Configuration loaded from: ../config/devices.json
Requires: pymodbus (pip install pymodbus)
"""

import time
import json
import os
import sys

try:
    from pymodbus.client import ModbusSerialClient
except ImportError:
    print("ERROR: pymodbus not installed")
    print("Install with: pip install pymodbus")
    sys.exit(1)

# --- Load Configuration ---
def load_config():
    """Load device configuration from devices.json"""
    config_path = os.path.join(os.path.dirname(__file__), '..', 'config', 'devices.json')
    try:
        with open(config_path, 'r') as f:
            config = json.load(f)
            return config['servo']['azimuth']
    except FileNotFoundError:
        print(f"ERROR: Configuration file not found: {config_path}")
        sys.exit(1)
    except KeyError:
        print("ERROR: 'servo.azimuth' configuration not found in devices.json")
        sys.exit(1)

# --- Servo Modbus Addresses (adjust based on your servo driver manual) ---
ADDR_POSITION = 0x0064      # Example: Position feedback register
ADDR_VELOCITY = 0x0065      # Example: Velocity feedback register
ADDR_STATUS = 0x0010        # Example: Status register
ADDR_ALARM = 0x0011         # Example: Alarm register
ADDR_MOTOR_TEMP = 0x0020    # Example: Motor temperature
ADDR_DRIVER_TEMP = 0x0021   # Example: Driver temperature

POLL_INTERVAL_S = 0.1  # 100 ms

def read_servo_data(client, slave_id):
    """Read servo driver data via Modbus"""
    try:
        # Read multiple holding registers
        result = client.read_holding_registers(
            address=ADDR_POSITION,
            count=6,  # Read position, velocity, and other registers
            slave=slave_id
        )

        if result.isError():
            return None

        return {
            'position': result.registers[0],
            'velocity': result.registers[1],
            'status': result.registers[2],
            'alarm': result.registers[3],
            'motor_temp': result.registers[4],
            'driver_temp': result.registers[5]
        }
    except Exception as e:
        print(f"Read error: {e}")
        return None

def main():
    # Load configuration
    config = load_config()
    SERIAL_PORT = config['port']
    BAUD_RATE = config['baudRate']
    SLAVE_ID = config['slaveId']

    print("=" * 70)
    print("  SERVO AZIMUTH TESTER - Azimuth Axis Driver")
    print("=" * 70)
    print(f"Configuration:")
    print(f"  Port: {SERIAL_PORT}")
    print(f"  Baud Rate: {BAUD_RATE}")
    print(f"  Slave ID: {SLAVE_ID}")
    print("=" * 70)
    print(f"Attempting to connect...")

    # Create Modbus client (no parity for servo)
    client = ModbusSerialClient(
        port=SERIAL_PORT,
        baudrate=BAUD_RATE,
        parity='N',
        stopbits=1,
        bytesize=8,
        timeout=1
    )

    if not client.connect():
        print(f"✗ Failed to connect to {SERIAL_PORT}")
        sys.exit(1)

    print("✓ Connection successful!")
    print("Starting continuous polling (Press Ctrl+C to stop)...")
    print("=" * 70)
    print("NOTE: Adjust register addresses based on your servo driver manual")
    print("=" * 70)

    try:
        while True:
            data = read_servo_data(client, SLAVE_ID)

            if data:
                # Check for alarms
                alarm_indicator = "⚠ ALARM" if data['alarm'] != 0 else "✓"

                print(
                    f"{alarm_indicator} | "
                    f"Pos: {data['position']:>6} | "
                    f"Vel: {data['velocity']:>6} | "
                    f"Status: 0x{data['status']:04X} | "
                    f"Alarm: 0x{data['alarm']:04X} | "
                    f"Temp: M={data['motor_temp']}°C D={data['driver_temp']}°C"
                )
            else:
                print("✗ Communication error")

            time.sleep(POLL_INTERVAL_S)

    except KeyboardInterrupt:
        print("\n" + "=" * 70)
        print("Polling stopped by user. Exiting.")
        print("=" * 70)
    finally:
        client.close()

if __name__ == "__main__":
    main()
