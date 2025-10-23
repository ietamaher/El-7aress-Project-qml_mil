#!/bin/bash

# ============================================================================
# RCWS Virtual Port Verification Script
# ============================================================================

echo "=========================================="
echo "RCWS Virtual Port Verification"
echo "=========================================="
echo ""

# Check virtual ports
echo "1. Virtual Ports:"
if ls /dev/ttyVIRT_* 1> /dev/null 2>&1; then
    ls -la /dev/ttyVIRT_* | awk '{print "  ✓ " $9}'
    echo ""
else
    echo "  ✗ No virtual ports found!"
    echo ""
fi

# Check production symlinks
echo "2. Production Symlinks:"
if ls /dev/serial/by-id/ 1> /dev/null 2>&1; then
    ls -la /dev/serial/by-id/ | grep "usb-" | awk '{print "  ✓ " $9 " -> " $11}'
    echo ""
else
    echo "  ✗ No symlinks found!"
    echo ""
fi

# Check ttyUSB links
echo "3. ttyUSB Links:"
if ls /dev/ttyUSB* 1> /dev/null 2>&1; then
    ls -la /dev/ttyUSB* | awk '{print "  ✓ " $9 " -> " $11}'
    echo ""
else
    echo "  ✗ No ttyUSB links found!"
    echo ""
fi

# Check socat processes
echo "4. Socat Processes:"
SOCAT_COUNT=$(ps aux | grep -c "[s]ocat")
if [ $SOCAT_COUNT -gt 0 ]; then
    echo "  ✓ $SOCAT_COUNT socat processes running"
else
    echo "  ✗ No socat processes found!"
fi
echo ""

# Check diagslave processes
echo "5. Modbus Simulators:"
DIAGSLAVE_COUNT=$(ps aux | grep -c "[d]iagslave")
if [ $DIAGSLAVE_COUNT -gt 0 ]; then
    echo "  ✓ $DIAGSLAVE_COUNT diagslave processes running"
    ps aux | grep "[d]iagslave" | awk '{print "    - " $0}' | cut -c 1-100
else
    echo "  ℹ No diagslave processes running (use start_modbus_simulators.sh)"
fi
echo ""

echo "=========================================="
