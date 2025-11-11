#!/usr/bin/env python3
"""
Baud Rate Scanner - Test different baud rates
"""

import serial
import time
import json
import os
import sys

def load_config():
    config_path = os.path.join(os.path.dirname(__file__), '..', 'config', 'devices.json')
    with open(config_path, 'r') as f:
        config = json.load(f)
        return config['video']['nightCamera']

def calculate_crc16(data, length):
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
    packet = bytearray()
    packet.append(0x6E)
    packet.append(0x00)
    packet.append(0x00)
    packet.append(function_code)
    byte_count = len(data)
    packet.append((byte_count >> 8) & 0xFF)
    packet.append(byte_count & 0xFF)
    crc1 = calculate_crc16(packet, 6)
    packet.append((crc1 >> 8) & 0xFF)
    packet.append(crc1 & 0xFF)
    packet.extend(data)
    crc2 = calculate_crc16(packet, len(packet))
    packet.append((crc2 >> 8) & 0xFF)
    packet.append(crc2 & 0xFF)
    return bytes(packet)

def test_baud_rate(port, baud_rate):
    """Test a specific baud rate"""
    try:
        with serial.Serial(port, baud_rate, timeout=0.5) as ser:
            time.sleep(0.2)
            
            # Send status command
            cmd = build_tau2_command(0x06, b'\x00\x00')
            ser.write(cmd)
            ser.flush()
            
            time.sleep(0.3)
            response = ser.read(128)
            
            if len(response) > 0:
                # Check if it looks like TAU2
                if len(response) >= 10 and response[0] == 0x6E:
                    return (True, "Valid TAU2 response!", response)
                else:
                    return (False, f"Got {len(response)} bytes: {response.hex().upper()}", response)
            else:
                return (False, "No response", b'')
    except Exception as e:
        return (False, f"Error: {e}", b'')

def main():
    config = load_config()
    SERIAL_PORT = config['controlPort']
    
    # Common FLIR baud rates
    BAUD_RATES = [9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600]
    
    print("=" * 80)
    print("  BAUD RATE SCANNER - Testing all common rates")
    print("=" * 80)
    print(f"Port: {SERIAL_PORT}\n")
    
    for baud in BAUD_RATES:
        print(f"Testing {baud:7d} baud... ", end='', flush=True)
        success, message, response = test_baud_rate(SERIAL_PORT, baud)
        
        if success:
            print(f"✓ SUCCESS - {message}")
            print(f"    Response: {response.hex().upper()}")
        else:
            print(f"✗ {message}")
    
    print("\n" + "=" * 80)
    print("Scan complete!")
    print("=" * 80)

if __name__ == "__main__":
    main()
