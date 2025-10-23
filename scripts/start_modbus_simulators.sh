#!/bin/bash

# ============================================================================
# RCWS Modbus Device Simulators
# Starts all Modbus RTU simulators for testing
# ============================================================================

# Check if diagslave exists
DIAGSLAVE="$HOME/diagslave/x86_64-linux-gnu/diagslave"
if [ ! -f "$DIAGSLAVE" ]; then
    echo "ERROR: diagslave not found at $DIAGSLAVE"
    echo "Download from: https://www.modbusdriver.com/diagslave.html"
    exit 1
fi

echo "=========================================="
echo "Starting Modbus RTU Simulators"
echo "=========================================="
echo ""

# Kill any existing diagslave processes
pkill -f diagslave || true
sleep 1

# ============================================================================
# PLC21 Simulator (Slave ID: 31, Baud: 115200, Parity: Even)
# ============================================================================
echo "Starting PLC21 simulator (Slave ID 31)..."
gnome-terminal --title="PLC21 Simulator" -- bash -c "
    echo '========================================';
    echo 'PLC21 Modbus RTU Simulator';
    echo 'Slave ID: 31';
    echo 'Port: /dev/ttyVIRT_PLC21_1';
    echo 'Baud: 115200, Parity: Even';
    echo '========================================';
    echo '';
    $DIAGSLAVE -m rtu -a 31 -b 115200 -p even /dev/ttyVIRT_PLC21_1;
    read -p 'Press Enter to close...'
" &

sleep 1

# ============================================================================
# PLC42 Simulator (Slave ID: 31, Baud: 115200, Parity: Even)
# ============================================================================
echo "Starting PLC42 simulator (Slave ID 31)..."
gnome-terminal --title="PLC42 Simulator" -- bash -c "
    echo '========================================';
    echo 'PLC42 Modbus RTU Simulator';
    echo 'Slave ID: 31';
    echo 'Port: /dev/ttyVIRT_PLC42_1';
    echo 'Baud: 115200, Parity: Even';
    echo '========================================';
    echo '';
    $DIAGSLAVE -m rtu -a 31 -b 115200 -p even /dev/ttyVIRT_PLC42_1;
    read -p 'Press Enter to close...'
" &

sleep 1

# ============================================================================
# Servo Azimuth Simulator (Slave ID: 2, Baud: 230400, Parity: None)
# ============================================================================
echo "Starting Servo Azimuth simulator (Slave ID 2)..."
gnome-terminal --title="Servo AZ Simulator" -- bash -c "
    echo '========================================';
    echo 'Servo Azimuth Modbus RTU Simulator';
    echo 'Slave ID: 2';
    echo 'Port: /dev/ttyVIRT_SERVO_AZ1';
    echo 'Baud: 230400, Parity: None';
    echo '========================================';
    echo '';
    $DIAGSLAVE -m rtu -a 2 -b 230400 -p none /dev/ttyVIRT_SERVO_AZ1;
    read -p 'Press Enter to close...'
" &

sleep 1

# ============================================================================
# Servo Elevation Simulator (Slave ID: 1, Baud: 230400, Parity: None)
# ============================================================================
echo "Starting Servo Elevation simulator (Slave ID 1)..."
gnome-terminal --title="Servo EL Simulator" -- bash -c "
    echo '========================================';
    echo 'Servo Elevation Modbus RTU Simulator';
    echo 'Slave ID: 1';
    echo 'Port: /dev/ttyVIRT_SERVO_EL1';
    echo 'Baud: 230400, Parity: None';
    echo '========================================';
    echo '';
    $DIAGSLAVE -m rtu -a 1 -b 230400 -p none /dev/ttyVIRT_SERVO_EL1;
    read -p 'Press Enter to close...'
" &

sleep 1

echo ""
echo "=========================================="
echo "All simulators started!"
echo "=========================================="
echo ""
echo "Active Simulators:"
echo "  • PLC21:      Slave ID 31, /dev/ttyVIRT_PLC21_1"
echo "  • PLC42:      Slave ID 31, /dev/ttyVIRT_PLC42_1"
echo "  • Servo AZ:   Slave ID 2,  /dev/ttyVIRT_SERVO_AZ1"
echo "  • Servo EL:   Slave ID 1,  /dev/ttyVIRT_SERVO_EL1"
echo ""
echo "To stop all simulators: pkill -f diagslave"
echo ""
