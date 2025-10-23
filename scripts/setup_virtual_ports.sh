#!/bin/bash

# ============================================================================
# RCWS Virtual Serial Port Setup Script
# Creates virtual serial ports matching production hardware configuration
# ============================================================================

set -e  # Exit on error

echo "=========================================="
echo "RCWS Virtual Serial Port Setup"
echo "=========================================="

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# ============================================================================
# STEP 1: Kill existing socat processes
# ============================================================================
echo -e "${YELLOW}[1/5] Cleaning up existing virtual ports...${NC}"
sudo pkill -f socat || true
sleep 1

# ============================================================================
# STEP 2: Create virtual serial port pairs
# ============================================================================
echo -e "${YELLOW}[2/5] Creating virtual serial port pairs...${NC}"

# Day Camera Control - if00
sudo socat -d -d pty,raw,echo=0,link=/dev/ttyVIRT_DAY_CAM0 \
     pty,raw,echo=0,link=/dev/ttyVIRT_DAY_CAM1 &
echo "  ✓ Day Camera ports created"

# Night Camera Control
sudo socat -d -d pty,raw,echo=0,link=/dev/ttyVIRT_NIGHT_CAM0 \
     pty,raw,echo=0,link=/dev/ttyVIRT_NIGHT_CAM1 &
echo "  ✓ Night Camera ports created"

# IMU/Gyro
sudo socat -d -d pty,raw,echo=0,link=/dev/ttyVIRT_IMU0 \
     pty,raw,echo=0,link=/dev/ttyVIRT_IMU1 &
echo "  ✓ IMU ports created"

# LRF (Laser Range Finder)
sudo socat -d -d pty,raw,echo=0,link=/dev/ttyVIRT_LRF0 \
     pty,raw,echo=0,link=/dev/ttyVIRT_LRF1 &
echo "  ✓ LRF ports created"

# PLC21 (Modbus RTU) - if00
sudo socat -d -d pty,raw,echo=0,link=/dev/ttyVIRT_PLC21_0 \
     pty,raw,echo=0,link=/dev/ttyVIRT_PLC21_1 &
echo "  ✓ PLC21 ports created"

# PLC42 (Modbus RTU) - if02
sudo socat -d -d pty,raw,echo=0,link=/dev/ttyVIRT_PLC42_0 \
     pty,raw,echo=0,link=/dev/ttyVIRT_PLC42_1 &
echo "  ✓ PLC42 ports created"

# Servo Azimuth (Modbus RTU) - if04
sudo socat -d -d pty,raw,echo=0,link=/dev/ttyVIRT_SERVO_AZ0 \
     pty,raw,echo=0,link=/dev/ttyVIRT_SERVO_AZ1 &
echo "  ✓ Servo Azimuth ports created"

# Servo Elevation (Modbus RTU) - if06
sudo socat -d -d pty,raw,echo=0,link=/dev/ttyVIRT_SERVO_EL0 \
     pty,raw,echo=0,link=/dev/ttyVIRT_SERVO_EL1 &
echo "  ✓ Servo Elevation ports created"

# Servo Actuator
sudo socat -d -d pty,raw,echo=0,link=/dev/ttyVIRT_ACTUATOR0 \
     pty,raw,echo=0,link=/dev/ttyVIRT_ACTUATOR1 &
echo "  ✓ Servo Actuator ports created"

# Wait for all ports to be created
sleep 2

# ============================================================================
# STEP 3: Create production-like symlinks in /dev/serial/by-id/
# ============================================================================
echo -e "${YELLOW}[3/5] Creating production-like symlinks...${NC}"

# Create directory if it doesn't exist
sudo mkdir -p /dev/serial/by-id/

# Day Camera - WCH USB Quad Serial port 0 (if00)
sudo ln -sf /dev/ttyVIRT_DAY_CAM0 \
     /dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BCD9DCABCD-if00
echo "  ✓ Day Camera symlink created"

# Night Camera - USB Single Serial
sudo ln -sf /dev/ttyVIRT_NIGHT_CAM0 \
     /dev/serial/by-id/usb-1a86_USB_Single_Serial_56D1123075-if00
echo "  ✓ Night Camera symlink created"

# PLC21 - WCH USB Quad Serial port 0 (if00) - different serial number
sudo ln -sf /dev/ttyVIRT_PLC21_0 \
     /dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if00
echo "  ✓ PLC21 symlink created"

# PLC42 - WCH USB Quad Serial port 1 (if02)
sudo ln -sf /dev/ttyVIRT_PLC42_0 \
     /dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if02
echo "  ✓ PLC42 symlink created"

# Servo Azimuth - WCH USB Quad Serial port 2 (if04)
sudo ln -sf /dev/ttyVIRT_SERVO_AZ0 \
     /dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if04
echo "  ✓ Servo Azimuth symlink created"

# Servo Elevation - WCH USB Quad Serial port 3 (if06)
sudo ln -sf /dev/ttyVIRT_SERVO_EL0 \
     /dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if06
echo "  ✓ Servo Elevation symlink created"

# Direct USB links (ttyUSB*)
sudo ln -sf /dev/ttyVIRT_IMU0 /dev/ttyUSB2
sudo ln -sf /dev/ttyVIRT_LRF0 /dev/ttyUSB1
sudo ln -sf /dev/ttyVIRT_ACTUATOR0 /dev/ttyUSB0
echo "  ✓ ttyUSB symlinks created"

# ============================================================================
# STEP 4: Fix permissions
# ============================================================================
echo -e "${YELLOW}[4/5] Setting permissions...${NC}"

sudo chmod 666 /dev/ttyVIRT_* 2>/dev/null || true
sudo chmod 666 /dev/pts/* 2>/dev/null || true
sudo chmod 666 /dev/ttyUSB* 2>/dev/null || true

echo "  ✓ Permissions set"

# ============================================================================
# STEP 5: Verify setup
# ============================================================================
echo -e "${YELLOW}[5/5] Verifying setup...${NC}"

echo ""
echo "Virtual Ports Created:"
ls -la /dev/ttyVIRT_* 2>/dev/null && echo -e "${GREEN}  ✓ All virtual ports exist${NC}" || echo -e "${RED}  ✗ Some ports missing${NC}"

echo ""
echo "Production Symlinks:"
ls -la /dev/serial/by-id/ 2>/dev/null && echo -e "${GREEN}  ✓ Symlinks created${NC}" || echo -e "${RED}  ✗ Symlinks missing${NC}"

echo ""
echo "ttyUSB Links:"
ls -la /dev/ttyUSB* 2>/dev/null && echo -e "${GREEN}  ✓ ttyUSB links created${NC}" || echo -e "${RED}  ✗ ttyUSB links missing${NC}"

# ============================================================================
# Port Mapping Reference
# ============================================================================
echo ""
echo "=========================================="
echo "Port Mapping Reference"
echo "=========================================="
echo ""
echo "Qt Application Side          ←→  Simulator Side"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "Day Camera Control           ←→  /dev/ttyVIRT_DAY_CAM1"
echo "Night Camera Control         ←→  /dev/ttyVIRT_NIGHT_CAM1"
echo "IMU (/dev/ttyUSB2)          ←→  /dev/ttyVIRT_IMU1"
echo "LRF (/dev/ttyUSB1)          ←→  /dev/ttyVIRT_LRF1"
echo "PLC21 (Modbus, ID 31)       ←→  /dev/ttyVIRT_PLC21_1"
echo "PLC42 (Modbus, ID 31)       ←→  /dev/ttyVIRT_PLC42_1"
echo "Servo Az (Modbus, ID 2)     ←→  /dev/ttyVIRT_SERVO_AZ1"
echo "Servo El (Modbus, ID 1)     ←→  /dev/ttyVIRT_SERVO_EL1"
echo "Actuator (/dev/ttyUSB0)     ←→  /dev/ttyVIRT_ACTUATOR1"
echo ""
echo -e "${GREEN}=========================================="
echo "Setup Complete!"
echo "==========================================${NC}"
echo ""
echo "Next steps:"
echo "  1. Run: ./start_modbus_simulators.sh"
echo "  2. Start your Qt application"
echo ""
