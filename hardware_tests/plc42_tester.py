#!/usr/bin/env python3
"""
PLC42 Tester - System PLC (42 I/O)
Tests the PLC42 system via Modbus RTU
Reads: Discrete inputs (sensors, interlocks), Holding registers (system control)

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
            return config['plc']['plc42']
    except FileNotFoundError:
        print(f"ERROR: Configuration file not found: {config_path}")
        sys.exit(1)
    except KeyError:
        print("ERROR: 'plc.plc42' configuration not found in devices.json")
        sys.exit(1)

# --- PLC42 Modbus Addresses ---
DISCRETE_INPUT_START = 0
DISCRETE_INPUT_COUNT = 8

HOLDING_REGISTER_START = 0
HOLDING_REGISTER_COUNT = 8

POLL_INTERVAL_S = 0.2  # 200 ms

def read_discrete_inputs(client, slave_id):
    """Read discrete inputs (sensors, safety interlocks)"""
    result = client.read_discrete_inputs(
        address=DISCRETE_INPUT_START,
        count=DISCRETE_INPUT_COUNT,
        slave=slave_id
    )

    if result.isError():
        print(f"✗ Error reading discrete inputs: {result}")
        return None

    return result.bits[:DISCRETE_INPUT_COUNT]

def read_holding_registers(client, slave_id):
    """Read holding registers (system control)"""
    result = client.read_holding_registers(
        address=HOLDING_REGISTER_START,
        count=HOLDING_REGISTER_COUNT,
        slave=slave_id
    )

    if result.isError():
        print(f"✗ Error reading holding registers: {result}")
        return None

    return result.registers

def main():
    # Load configuration
    config = load_config()
    SERIAL_PORT = config['port']
    BAUD_RATE = config['baudRate']
    SLAVE_ID = config['slaveId']
    PARITY = config['parity'][0].upper()

    print("=" * 70)
    print("  PLC42 TESTER - System PLC (42 I/O)")
    print("=" * 70)
    print(f"Configuration:")
    print(f"  Port: {SERIAL_PORT}")
    print(f"  Baud Rate: {BAUD_RATE}")
    print(f"  Slave ID: {SLAVE_ID}")
    print(f"  Parity: {PARITY}ven")
    print("=" * 70)
    print(f"Attempting to connect...")

    # Create Modbus client
    client = ModbusSerialClient(
        port=SERIAL_PORT,
        baudrate=BAUD_RATE,
        parity=PARITY,
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
    print("Input Labels: [UPPER_SENSOR, LOWER_SENSOR, EMERG_STOP, AMMO, IN1, IN2, IN3, SOLENOID]")
    print("=" * 70)

    try:
        while True:
            # Read discrete inputs
            inputs = read_discrete_inputs(client, SLAVE_ID)

            # Read holding registers
            registers = read_holding_registers(client, SLAVE_ID)

            if inputs is not None:
                input_str = ''.join(['1' if inp else '0' for inp in inputs])
                print(f"Inputs: {input_str} ", end="")

                # Check emergency stop
                if inputs[2]:  # Emergency stop active
                    print("| ⚠ EMERGENCY STOP ACTIVE ⚠ ", end="")

                if registers is not None:
                    print(f"| Regs: SOL_MODE={registers[0]} GIMBAL_MODE={registers[1]} AZ_SPD={registers[2]} EL_SPD={registers[3]}")
                else:
                    print()

            time.sleep(POLL_INTERVAL_S)

    except KeyboardInterrupt:
        print("\n" + "=" * 70)
        print("Polling stopped by user. Exiting.")
        print("=" * 70)
    finally:
        client.close()

if __name__ == "__main__":
    main()
