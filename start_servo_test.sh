#!/bin/bash
# Helper script to test servo communication
# This script provides instructions for testing servos step-by-step

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

clear
echo "=========================================="
echo "Servo Communication Test Guide"
echo "=========================================="
echo ""
echo -e "${YELLOW}This script will guide you through testing servo communication.${NC}"
echo ""
echo "You will need 4 terminals open:"
echo "  1. This terminal (instructions)"
echo "  2. Azimuth servo simulator"
echo "  3. Elevation servo simulator"
echo "  4. Qt application"
echo ""
read -p "Press Enter to continue..."
clear

# Step 1: Check virtual ports
echo "=========================================="
echo -e "${BLUE}Step 1: Verify Virtual Ports${NC}"
echo "=========================================="
echo ""
echo "Checking for virtual serial ports..."
if ls /dev/ttyVIRT_SERVO_* 2>/dev/null; then
    echo -e "${GREEN}✓ Virtual servo ports found${NC}"
else
    echo -e "${RED}✗ Virtual servo ports NOT found!${NC}"
    echo ""
    echo "Please make sure your virtual serial port system is set up."
    echo "Expected ports:"
    echo "  - /dev/ttyVIRT_SERVO_AZ1"
    echo "  - /dev/ttyVIRT_SERVO_EL1"
    exit 1
fi
echo ""
read -p "Press Enter to continue..."
clear

# Step 2: Fix symlinks
echo "=========================================="
echo -e "${BLUE}Step 2: Fix Symlinks${NC}"
echo "=========================================="
echo ""
echo "The Qt application expects ports at:"
echo "  - /dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if04 (Azimuth)"
echo "  - /dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if06 (Elevation)"
echo ""
echo "These must point to:"
echo "  - /dev/ttyVIRT_SERVO_AZ1"
echo "  - /dev/ttyVIRT_SERVO_EL1"
echo ""
echo -e "${YELLOW}Running symlink fix script (requires sudo)...${NC}"
echo ""
sudo ./fix_servo_symlinks.sh
echo ""
read -p "Press Enter to continue..."
clear

# Step 3: Start simulators
echo "=========================================="
echo -e "${BLUE}Step 3: Start Simulators${NC}"
echo "=========================================="
echo ""
echo -e "${YELLOW}You need to start TWO simulators in separate terminals.${NC}"
echo ""
echo -e "${GREEN}Terminal 2 - Azimuth Simulator:${NC}"
echo "cd ~/Desktop/Projet_RCWS/Test-Devices"
echo "python servo_simulator.py --port /dev/ttyVIRT_SERVO_AZ1 --baudrate 230400 --slave-id 2 --name Azimuth"
echo ""
echo -e "${GREEN}Terminal 3 - Elevation Simulator:${NC}"
echo "cd ~/Desktop/Projet_RCWS/Test-Devices"
echo "python servo_simulator.py --port /dev/ttyVIRT_SERVO_EL1 --baudrate 230400 --slave-id 1 --name Elevation"
echo ""
echo -e "${YELLOW}Open two new terminals and run these commands.${NC}"
echo ""
read -p "Press Enter when both simulators are running..."
clear

# Step 4: Check simulators are running
echo "=========================================="
echo -e "${BLUE}Step 4: Verify Simulators${NC}"
echo "=========================================="
echo ""
echo "Checking for running simulators..."
if ps aux | grep -q "[s]ervo_simulator.py"; then
    echo -e "${GREEN}✓ Servo simulators are running${NC}"
    echo ""
    echo "Running simulators:"
    ps aux | grep "[s]ervo_simulator.py" | awk '{print "  - PID " $2 ": " $NF " " $(NF-1) " " $(NF-2)}'
else
    echo -e "${RED}✗ No servo simulators detected!${NC}"
    echo ""
    echo "Please start the simulators as instructed in Step 3."
    exit 1
fi
echo ""
read -p "Press Enter to continue..."
clear

# Step 5: Run Qt application
echo "=========================================="
echo -e "${BLUE}Step 5: Start Qt Application${NC}"
echo "=========================================="
echo ""
echo -e "${GREEN}Terminal 4 - Qt Application:${NC}"
echo "cd ~/Desktop/Projet_RCWS/QT6-qml-migration"
echo "./build/El-7arres-Project-qml_mil"
echo ""
echo -e "${YELLOW}Open a new terminal and run the Qt application.${NC}"
echo ""
echo "Watch for these log messages:"
echo "  ${GREEN}✓ SUCCESS:${NC}"
echo "    ModbusTransport: State changed to \"Connected\" for slave 2"
echo "    ModbusTransport: State changed to \"Connected\" for slave 1"
echo ""
echo "  ${RED}✗ FAILURE:${NC}"
echo "    ModbusTransport: Slave X connection failed: ..."
echo ""
read -p "Press Enter to continue..."
clear

# Step 6: Monitor communication
echo "=========================================="
echo -e "${BLUE}Step 6: Monitor Communication${NC}"
echo "=========================================="
echo ""
echo "If everything is working correctly, you should see:"
echo ""
echo "${GREEN}In the simulator terminals:${NC}"
echo "  REQUEST #1"
echo "  Protocol Address: 204 (0x00CC)"
echo "  Count: 2 registers"
echo "  Position read: ..."
echo ""
echo "${GREEN}In the Qt application:${NC}"
echo "  ModbusTransport: Connected successfully to ... with slave ID 2"
echo "  ModbusTransport: Connected successfully to ... with slave ID 1"
echo "  \"az\" initialized successfully with poll interval: 50 ms"
echo "  \"el\" initialized successfully with poll interval: 50 ms"
echo ""
echo "=========================================="
echo -e "${GREEN}Setup Complete!${NC}"
echo "=========================================="
echo ""
echo "Your system is now configured for servo testing."
echo ""
echo "To stop:"
echo "  1. Stop Qt application (Ctrl+C)"
echo "  2. Stop both simulators (Ctrl+C in each terminal)"
echo ""
