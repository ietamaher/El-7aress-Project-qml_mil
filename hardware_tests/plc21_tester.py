#!/usr/bin/env python3
"""
PLC21 Tester - Control Panel PLC (21 I/O)
Tests the PLC21 panel via Modbus RTU
Reads: Digital inputs (switches), Holding registers (analog inputs)

Configuration loaded from: ../config/devices.json
Requires: pymodbus (pip install pymodbus)
"""

import time
import json
import os
import sys

try:
    from pymodbus.client import ModbusSerialClient
    from pymodbus.constants import Endian
    from pymodbus.payload import BinaryPayloadDecoder
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
            return config['plc']['plc21']
    except FileNotFoundError:
        print(f"ERROR: Configuration file not found: {config_path}")
        sys.exit(1)
    except KeyError:
        print("ERROR: 'plc.plc21' configuration not found in devices.json")
        sys.exit(1)

# --- PLC21 Modbus Addresses ---
# Discrete Inputs (Read-Only) - Function Code 0x02
DISCRETE_INPUT_START = 0
DISCRETE_INPUT_COUNT = 10

# Holding Registers (Read/Write) - Function Code 0x03 / 0x06
HOLDING_REGISTER_START = 0
HOLDING_REGISTER_COUNT = 3

POLL_INTERVAL_S = 0.2  # 200 ms

def read_discrete_inputs(client, slave_id):
    """Read discrete inputs (switches)"""
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
    """Read holding registers (analog values)"""
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
    PARITY = config['parity'][0].upper()  # 'E' for even

    print("=" * 70)
    print("  PLC21 TESTER - Control Panel (21 I/O)")
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
    print("Switch Labels: [ARM_GUN, LOAD_AMMO, ENABLE_STATION, HOME_POS, STAB, AUTH, CAM_SW, UP, DOWN, VAL]")
    print("=" * 70)

    try:
        while True:
            # Read discrete inputs (switches)
            switches = read_discrete_inputs(client, SLAVE_ID)

            # Read holding registers (analog inputs)
            registers = read_holding_registers(client, SLAVE_ID)

            if switches is not None:
                # Format switch states
                switch_str = ''.join(['1' if sw else '0' for sw in switches])
                print(f"Switches: {switch_str} ", end="")

                # Highlight pressed switches
                pressed = [i for i, sw in enumerate(switches) if sw]
                if pressed:
                    print(f"| Pressed: {pressed}", end="")

                if registers is not None:
                    print(f" | Registers: Speed={registers[0]} FireMode={registers[1]} Temp={registers[2]}")
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
