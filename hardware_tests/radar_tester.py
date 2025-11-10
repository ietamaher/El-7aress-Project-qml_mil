#!/usr/bin/env python3
"""
Radar Tester - NMEA RATTM Protocol
Tests radar target tracking data reception via serial
Parses NMEA RATTM sentences (Radar Tracked Target Message)

Configuration loaded from: ../config/devices.json
"""

import serial
import time
import json
import os
import sys
import re

# --- Load Configuration ---
def load_config():
    """Load device configuration from devices.json"""
    config_path = os.path.join(os.path.dirname(__file__), '..', 'config', 'devices.json')
    try:
        with open(config_path, 'r') as f:
            config = json.load(f)
            return config['radar']
    except FileNotFoundError:
        print(f"ERROR: Configuration file not found: {config_path}")
        sys.exit(1)
    except KeyError:
        print("ERROR: 'radar' configuration not found in devices.json")
        sys.exit(1)

def parse_nmea_rattm(sentence):
    """
    Parse NMEA RATTM sentence
    Format: $RATTM,xx,x.x,x.x,a,x.x,x.x,a,x.x,x.x,a,c--c,a,a,hhmmss.ss,a*hh
    Example: $RATTM,01,3.2,045.0,T,2.5,120.0,T,0.5,0.3,N,Target1,L,,,*3C
    """
    try:
        # Remove $ and checksum
        if sentence.startswith('$'):
            sentence = sentence[1:]
        if '*' in sentence:
            sentence = sentence.split('*')[0]

        fields = sentence.split(',')

        if fields[0] != 'RATTM' or len(fields) < 12:
            return None

        return {
            'target_id': fields[1],
            'distance_nm': float(fields[2]) if fields[2] else None,
            'bearing_deg': float(fields[3]) if fields[3] else None,
            'bearing_ref': fields[4],  # T=True, R=Relative
            'speed_knots': float(fields[5]) if fields[5] else None,
            'course_deg': float(fields[6]) if fields[6] else None,
            'course_ref': fields[7],
            'cpa_nm': float(fields[8]) if fields[8] else None,  # Closest Point of Approach
            'tcpa_min': float(fields[9]) if fields[9] else None,  # Time to CPA
            'status': fields[10],
            'target_name': fields[11] if len(fields) > 11 else '',
        }
    except (ValueError, IndexError) as e:
        print(f"Parse error: {e} | Sentence: {sentence}")
        return None

def main():
    # Load configuration
    config = load_config()
    SERIAL_PORT = config['port']
    BAUD_RATE = config['baudRate']

    print("=" * 70)
    print("  RADAR TESTER - NMEA RATTM Protocol")
    print("=" * 70)
    print(f"Configuration:")
    print(f"  Port: {SERIAL_PORT}")
    print(f"  Baud Rate: {BAUD_RATE}")
    print("=" * 70)
    print(f"Attempting to connect...")

    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2) as ser:
            print("✓ Connection successful!")
            print("Waiting for NMEA RATTM sentences (Press Ctrl+C to stop)...")
            print("=" * 70)
            print("Format: ID | Distance | Bearing | Speed | Course | CPA | TCPA | Name")
            print("=" * 70)

            while True:
                # Read NMEA sentence
                line = ser.readline()

                if line:
                    try:
                        sentence = line.decode('ascii', errors='ignore').strip()

                        # Only process RATTM sentences
                        if '$RATTM' in sentence or '$GARATTM' in sentence:
                            data = parse_nmea_rattm(sentence)

                            if data:
                                print(
                                    f"Target {data['target_id']:>2} | "
                                    f"Dist: {data['distance_nm']:>6.2f} nm | "
                                    f"Brg: {data['bearing_deg']:>6.1f}° {data['bearing_ref']} | "
                                    f"Spd: {data['speed_knots']:>5.1f} kts | "
                                    f"Crs: {data['course_deg']:>6.1f}° | "
                                    f"CPA: {data['cpa_nm']:>5.2f} nm | "
                                    f"TCPA: {data['tcpa_min']:>5.1f} min | "
                                    f"{data['target_name']}"
                                )
                            else:
                                print(f"⚠ Invalid RATTM: {sentence}")
                        # Optionally show other NMEA sentences
                        elif sentence.startswith('$'):
                            print(f"Other NMEA: {sentence[:50]}...")

                    except UnicodeDecodeError:
                        print(f"Decode error: {line.hex()}")

    except serial.SerialException as e:
        print(f"✗ Error: Could not open serial port {SERIAL_PORT}")
        print(f"  {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n" + "=" * 70)
        print("Monitoring stopped by user. Exiting.")
        print("=" * 70)

if __name__ == "__main__":
    main()
