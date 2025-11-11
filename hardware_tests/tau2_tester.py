#!/usr/bin/env python3
"""
TAU2 Camera Tester - Proper TAU2 RS232 Protocol Implementation
Tests temperature reading, FFC, zoom, and other commands
Uses correct CRC16 and packet structure per TAU2 specification
"""

import serial
import struct
import time
import sys

# TAU2 Protocol Constants
START_BYTE = 0x6E
PROCESS_CODE = 0x00
RESERVED_BYTE = 0x00

# TAU2 Function Codes
FN_NO_OP = 0x00
FN_SET_DEFAULTS = 0x01
FN_CAMERA_RESET = 0x02
FN_GET_REVISION = 0x05
FN_STATUS_REQUEST = 0x06
FN_GAIN_MODE = 0x0A
FN_FFC_MODE_SELECT = 0x0B
FN_DO_FFC = 0x0C
FN_FFC_PERIOD = 0x0D
FN_VIDEO_MODE = 0x0F
FN_VIDEO_LUT = 0x10
FN_AGC_TYPE = 0x13
FN_CONTRAST = 0x14
FN_BRIGHTNESS = 0x15
FN_READ_TEMP_SENSOR = 0x20
FN_PAN_AND_TILT = 0x70

def calculate_crc16(data):
    """Calculate CRC16-CCITT for TAU2 protocol"""
    crc = 0x0000
    for byte in data:
        crc ^= (byte << 8)
        for _ in range(8):
            if crc & 0x8000:
                crc = (crc << 1) ^ 0x1021
            else:
                crc <<= 1
            crc &= 0xFFFF
    return crc

def build_tau2_command(function_code, data=b''):
    """Build a TAU2 command packet with proper CRC"""
    # Header: START + RESERVED + RESERVED + FUNCTION + BYTE_COUNT_MSB + BYTE_COUNT_LSB
    byte_count = len(data)
    packet = bytearray([
        START_BYTE,
        RESERVED_BYTE,
        RESERVED_BYTE,
        function_code,
        (byte_count >> 8) & 0xFF,
        byte_count & 0xFF
    ])

    # Calculate first CRC (header only)
    crc1 = calculate_crc16(packet)
    packet.extend([(crc1 >> 8) & 0xFF, crc1 & 0xFF])

    # Add data payload
    packet.extend(data)

    # Calculate second CRC (entire packet except final CRC)
    crc2 = calculate_crc16(packet)
    packet.extend([(crc2 >> 8) & 0xFF, crc2 & 0xFF])

    return bytes(packet)

def parse_tau2_response(response):
    """Parse TAU2 response packet"""
    if len(response) < 10:
        return None

    if response[0] != START_BYTE:
        print(f"  ⚠ Invalid start byte: 0x{response[0]:02X} (expected 0x6E)")
        return None

    status_byte = response[1]
    function_code = response[3]
    byte_count = (response[4] << 8) | response[5]

    if len(response) < 10 + byte_count:
        print(f"  ⚠ Incomplete packet (expected {10 + byte_count}, got {len(response)})")
        return None

    # Verify CRCs
    crc1_received = (response[6] << 8) | response[7]
    crc1_calculated = calculate_crc16(response[:6])

    payload_end = 8 + byte_count
    crc2_received = (response[payload_end] << 8) | response[payload_end + 1]
    crc2_calculated = calculate_crc16(response[:payload_end])

    if crc1_received != crc1_calculated:
        print(f"  ⚠ CRC1 mismatch: got 0x{crc1_received:04X}, expected 0x{crc1_calculated:04X}")
        return None

    if crc2_received != crc2_calculated:
        print(f"  ⚠ CRC2 mismatch: got 0x{crc2_received:04X}, expected 0x{crc2_calculated:04X}")
        return None

    payload = response[8:payload_end]

    return {
        'status': status_byte,
        'function': function_code,
        'payload': payload
    }

def format_hex(data):
    """Format bytes as hex string"""
    return ' '.join(f'{b:02X}' for b in data)

def test_status_request(ser):
    """Test 0x06 STATUS_REQUEST"""
    print("\n📋 Test 1: STATUS_REQUEST (0x06)")
    cmd = build_tau2_command(FN_STATUS_REQUEST, b'\x00\x00')
    print(f"  TX: {format_hex(cmd)}")
    ser.write(cmd)
    time.sleep(0.2)

    response = ser.read(100)
    if response:
        print(f"  RX: {format_hex(response)}")
        parsed = parse_tau2_response(response)
        if parsed:
            print(f"  ✓ Status: 0x{parsed['status']:02X}, Camera Status: {format_hex(parsed['payload'])}")
            return parsed
        else:
            print("  ✗ Failed to parse response")
    else:
        print("  ✗ No response received")
    return None

def test_read_temperature(ser, mode=None):
    """Test 0x20 READ_TEMP_SENSOR with different modes"""

    # Try different modes
    test_modes = [
        (b'', "GET (no argument)"),
        (b'\x00\x00', "Mode 0x0000 (disabled/get)"),
        (b'\x00\x01', "Mode 0x0001 (Fahrenheit)"),
        (b'\x00\x02', "Mode 0x0002 (Celsius)")
    ]

    if mode is not None:
        test_modes = [test_modes[mode]]

    for arg, desc in test_modes:
        print(f"\n🌡️  Test: READ_TEMP_SENSOR (0x20) - {desc}")
        cmd = build_tau2_command(FN_READ_TEMP_SENSOR, arg)
        print(f"  TX: {format_hex(cmd)}")
        ser.write(cmd)
        time.sleep(0.2)

        response = ser.read(100)
        if response:
            print(f"  RX: {format_hex(response)}")
            parsed = parse_tau2_response(response)
            if parsed:
                if parsed['status'] != 0x00:
                    error_msgs = {
                        0x01: "Camera Busy",
                        0x02: "Not Ready",
                        0x03: "Data Out of Range",
                        0x04: "Checksum Error",
                        0x05: "Undefined Process",
                        0x06: "Undefined Function",
                        0x07: "Timeout",
                        0x09: "Byte Count Mismatch",
                        0x0A: "Feature Not Enabled"
                    }
                    error_msg = error_msgs.get(parsed['status'], f"Unknown Error 0x{parsed['status']:02X}")
                    print(f"  ✗ TAU2 Error: {error_msg} (0x{parsed['status']:02X})")
                    continue

                if len(parsed['payload']) >= 2:
                    # Temperature is signed 16-bit value
                    temp_raw = struct.unpack('>h', parsed['payload'][:2])[0]
                    temp_celsius = temp_raw / 10.0
                    print(f"  ✓ SUCCESS! FPA Temperature: {temp_celsius}°C (raw: {temp_raw})")
                    return temp_celsius
                else:
                    print(f"  ⚠ Response OK but no payload (len={len(parsed['payload'])})")
            else:
                print("  ✗ Failed to parse response")
        else:
            print("  ✗ No response received")

    return None

def test_do_ffc(ser):
    """Test 0x0C DO_FFC"""
    print("\n🔄 Test 3: DO_FFC (0x0C)")
    cmd = build_tau2_command(FN_DO_FFC)
    print(f"  TX: {format_hex(cmd)}")
    ser.write(cmd)
    time.sleep(0.2)

    response = ser.read(100)
    if response:
        print(f"  RX: {format_hex(response)}")
        parsed = parse_tau2_response(response)
        if parsed:
            print("  ✓ FFC command acknowledged")
            print("  ⏳ FFC in progress (wait ~1-2 seconds)...")
            time.sleep(2)
            return True
    else:
        print("  ✗ No response received")
    return False

def test_pan_tilt(ser, tilt=0, pan=0):
    """Test 0x70 PAN_AND_TILT"""
    print(f"\n🎯 Test 4: PAN_AND_TILT (0x70) - Tilt={tilt}, Pan={pan}")
    # Bytes 0-1: Tilt (signed), Bytes 2-3: Pan (signed)
    data = struct.pack('>hh', tilt, pan)
    cmd = build_tau2_command(FN_PAN_AND_TILT, data)
    print(f"  TX: {format_hex(cmd)}")
    ser.write(cmd)
    time.sleep(0.2)

    response = ser.read(100)
    if response:
        print(f"  RX: {format_hex(response)}")
        parsed = parse_tau2_response(response)
        if parsed and len(parsed['payload']) >= 4:
            tilt_resp, pan_resp = struct.unpack('>hh', parsed['payload'][:4])
            print(f"  ✓ Pan/Tilt set: Tilt={tilt_resp}, Pan={pan_resp}")
            return True
    else:
        print("  ✗ No response received")
    return False

def test_video_mode(ser, mode=0x0004):
    """Test 0x0F VIDEO_MODE (0x0000=normal, 0x0004=zoom)"""
    mode_name = "ZOOM (2X)" if mode == 0x0004 else "NORMAL (1X)"
    print(f"\n🔍 Test 5: VIDEO_MODE (0x0F) - {mode_name}")
    data = struct.pack('>H', mode)
    cmd = build_tau2_command(FN_VIDEO_MODE, data)
    print(f"  TX: {format_hex(cmd)}")
    ser.write(cmd)
    time.sleep(0.2)

    response = ser.read(100)
    if response:
        print(f"  RX: {format_hex(response)}")
        parsed = parse_tau2_response(response)
        if parsed:
            print(f"  ✓ Video mode set to {mode_name}")
            return True
    else:
        print("  ✗ No response received")
    return False

def main():
    SERIAL_PORT = "/dev/ttyUSB1"  # Adjust to your TAU2 serial port
    BAUD_RATE = 921600  # TAU2 baud rate (as specified in your config)

    print("=" * 70)
    print("  TAU2 CAMERA TESTER - Proper RS232 Protocol")
    print("=" * 70)
    print(f"Serial Port: {SERIAL_PORT}")
    print(f"Baud Rate: {BAUD_RATE}")
    print("=" * 70)

    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            print("✓ Serial port opened successfully")
            time.sleep(0.5)

            # Run tests
            test_status_request(ser)
            test_read_temperature(ser)
            test_do_ffc(ser)
            test_video_mode(ser, 0x0004)  # Enable 2X zoom
            test_pan_tilt(ser, 0, 0)  # Center position
            test_read_temperature(ser)  # Read temp again after FFC

            print("\n" + "=" * 70)
            print("✓ All tests completed!")
            print("=" * 70)

    except serial.SerialException as e:
        print(f"✗ Error: Could not open serial port {SERIAL_PORT}")
        print(f"  {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n" + "=" * 70)
        print("Test interrupted by user.")
        print("=" * 70)

if __name__ == "__main__":
    main()
