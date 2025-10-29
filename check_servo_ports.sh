#!/bin/bash
# Diagnostic script for servo port connectivity

echo "=========================================="
echo "Servo Port Diagnostic Tool"
echo "=========================================="
echo ""

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if running as root (needed for some operations)
if [ "$EUID" -ne 0 ]; then
    echo -e "${YELLOW}Note: Not running as root. Some checks may be limited.${NC}"
    echo ""
fi

echo "1. Checking virtual serial ports..."
echo "----------------------------------------"
if ls /dev/ttyVIRT_SERVO_* 2>/dev/null; then
    echo -e "${GREEN}✓ Virtual servo ports found${NC}"
else
    echo -e "${RED}✗ No virtual servo ports found!${NC}"
    echo "  Expected: /dev/ttyVIRT_SERVO_AZ1, /dev/ttyVIRT_SERVO_EL1"
fi
echo ""

echo "2. Checking expected symlinks..."
echo "----------------------------------------"
AZ_LINK="/dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if04"
EL_LINK="/dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if06"

check_symlink() {
    local link="$1"
    local expected_target="$2"

    if [ -L "$link" ]; then
        target=$(readlink -f "$link")
        echo -e "${GREEN}✓${NC} $link exists"
        echo "  → Points to: $target"
        if [ "$target" == "$expected_target" ]; then
            echo -e "  ${GREEN}✓ Correct target!${NC}"
        else
            echo -e "  ${YELLOW}⚠ Expected: $expected_target${NC}"
        fi
    elif [ -e "$link" ]; then
        echo -e "${YELLOW}⚠${NC} $link exists but is not a symlink"
    else
        echo -e "${RED}✗${NC} $link does not exist"
        echo "  Create with: sudo ln -sf $expected_target $link"
    fi
}

check_symlink "$AZ_LINK" "/dev/ttyVIRT_SERVO_AZ1"
echo ""
check_symlink "$EL_LINK" "/dev/ttyVIRT_SERVO_EL1"
echo ""

echo "3. Checking port permissions..."
echo "----------------------------------------"
check_permissions() {
    local port="$1"
    if [ -e "$port" ]; then
        ls -l "$port"
        if [ -r "$port" ] && [ -w "$port" ]; then
            echo -e "${GREEN}✓ Port is readable and writable${NC}"
        else
            echo -e "${YELLOW}⚠ Port may not have correct permissions${NC}"
            echo "  Try: sudo chmod 666 $port"
        fi
    else
        echo -e "${RED}✗ Port does not exist${NC}"
    fi
}

check_permissions "/dev/ttyVIRT_SERVO_AZ1"
echo ""
check_permissions "/dev/ttyVIRT_SERVO_EL1"
echo ""

echo "4. Checking if ports are in use..."
echo "----------------------------------------"
if command -v lsof &> /dev/null; then
    for port in /dev/ttyVIRT_SERVO_AZ1 /dev/ttyVIRT_SERVO_EL1; do
        if [ -e "$port" ]; then
            echo "Checking $port:"
            if lsof "$port" 2>/dev/null; then
                echo -e "${YELLOW}⚠ Port is currently in use${NC}"
            else
                echo -e "${GREEN}✓ Port is available${NC}"
            fi
        fi
    done
else
    echo -e "${YELLOW}⚠ lsof not installed, cannot check if ports are in use${NC}"
fi
echo ""

echo "5. Checking running simulators..."
echo "----------------------------------------"
ps aux | grep -i "[s]ervo_simulator.py" || echo "No servo simulators running"
echo ""

echo "=========================================="
echo "Summary & Recommendations"
echo "=========================================="
echo ""
echo "To fix symlinks (if needed):"
echo "  sudo ln -sf /dev/ttyVIRT_SERVO_AZ1 $AZ_LINK"
echo "  sudo ln -sf /dev/ttyVIRT_SERVO_EL1 $EL_LINK"
echo ""
echo "To start simulators:"
echo "  Terminal 1: python servo_simulator.py --port /dev/ttyVIRT_SERVO_AZ1 --baudrate 230400 --slave-id 2 --name Azimuth"
echo "  Terminal 2: python servo_simulator.py --port /dev/ttyVIRT_SERVO_EL1 --baudrate 230400 --slave-id 1 --name Elevation"
echo ""
