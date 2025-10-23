#!/bin/bash

# ============================================================================
# RCWS Virtual Serial Port Cleanup Script
# ============================================================================

echo "Cleaning up virtual serial ports..."

# Kill all socat processes
sudo pkill -f socat || true
echo "  ✓ Stopped socat processes"

# Remove symlinks
sudo rm -f /dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BCD9DCABCD-if00 2>/dev/null || true
sudo rm -f /dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if00 2>/dev/null || true
sudo rm -f /dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if02 2>/dev/null || true
sudo rm -f /dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if04 2>/dev/null || true
sudo rm -f /dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if06 2>/dev/null || true
sudo rm -f /dev/serial/by-id/usb-1a86_USB_Single_Serial_56D1123075-if00 2>/dev/null || true
sudo rm -f /dev/ttyUSB0 2>/dev/null || true
sudo rm -f /dev/ttyUSB1 2>/dev/null || true
sudo rm -f /dev/ttyUSB2 2>/dev/null || true
echo "  ✓ Removed symlinks"

# Remove virtual ports (they auto-remove when socat stops, but just in case)
sudo rm -f /dev/ttyVIRT_* 2>/dev/null || true

echo "Cleanup complete!"
