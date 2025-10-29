#!/bin/bash
# Fix servo port symlinks

set -e  # Exit on error

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "=========================================="
echo "Fixing Servo Port Symlinks"
echo "=========================================="
echo ""

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo -e "${RED}ERROR: This script must be run as root${NC}"
    echo "Please run: sudo $0"
    exit 1
fi

# Define paths
AZ_LINK="/dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if04"
EL_LINK="/dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if06"
AZ_TARGET="/dev/ttyVIRT_SERVO_AZ1"
EL_TARGET="/dev/ttyVIRT_SERVO_EL1"

# Create by-id directory if it doesn't exist
mkdir -p /dev/serial/by-id

echo "1. Removing old symlinks (if they exist)..."
if [ -e "$AZ_LINK" ]; then
    rm -f "$AZ_LINK"
    echo -e "${GREEN}✓${NC} Removed old $AZ_LINK"
else
    echo "  (No old symlink to remove)"
fi

if [ -e "$EL_LINK" ]; then
    rm -f "$EL_LINK"
    echo -e "${GREEN}✓${NC} Removed old $EL_LINK"
else
    echo "  (No old symlink to remove)"
fi
echo ""

echo "2. Checking if target virtual ports exist..."
if [ ! -e "$AZ_TARGET" ]; then
    echo -e "${RED}✗ ERROR: $AZ_TARGET does not exist!${NC}"
    echo "  Make sure your virtual serial port setup is running"
    exit 1
fi
echo -e "${GREEN}✓${NC} $AZ_TARGET exists"

if [ ! -e "$EL_TARGET" ]; then
    echo -e "${RED}✗ ERROR: $EL_TARGET does not exist!${NC}"
    echo "  Make sure your virtual serial port setup is running"
    exit 1
fi
echo -e "${GREEN}✓${NC} $EL_TARGET exists"
echo ""

echo "3. Creating new symlinks..."
ln -sf "$AZ_TARGET" "$AZ_LINK"
echo -e "${GREEN}✓${NC} Created: $AZ_LINK -> $AZ_TARGET"

ln -sf "$EL_TARGET" "$EL_LINK"
echo -e "${GREEN}✓${NC} Created: $EL_LINK -> $EL_TARGET"
echo ""

echo "4. Verifying symlinks..."
echo "Azimuth:"
ls -la "$AZ_LINK"
echo ""
echo "Elevation:"
ls -la "$EL_LINK"
echo ""

echo "=========================================="
echo -e "${GREEN}✓ Symlinks fixed successfully!${NC}"
echo "=========================================="
echo ""
echo "Next steps:"
echo "1. Start Azimuth simulator:"
echo "   python servo_simulator.py --port $AZ_TARGET --baudrate 230400 --slave-id 2 --name Azimuth"
echo ""
echo "2. Start Elevation simulator:"
echo "   python servo_simulator.py --port $EL_TARGET --baudrate 230400 --slave-id 1 --name Elevation"
echo ""
echo "3. Start your Qt application"
echo ""
