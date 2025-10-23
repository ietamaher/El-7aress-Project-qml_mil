#!/bin/bash

# ============================================================================
# RCWS Complete Test Environment Setup
# ============================================================================

set -e

echo "=========================================="
echo "RCWS Complete Test Environment"
echo "=========================================="
echo ""

# Step 1: Setup virtual ports
echo "Step 1: Setting up virtual ports..."
./scripts/setup_virtual_ports.sh
echo ""

# Step 2: Verify setup
echo "Step 2: Verifying setup..."
./scripts/verify_setup.sh
echo ""

# Step 3: Start simulators
echo "Step 3: Starting Modbus simulators..."
./scripts/start_modbus_simulators.sh
echo ""

# Wait a bit
sleep 2

# Step 4: Final verification
echo "Step 4: Final verification..."
./scripts/verify_setup.sh
echo ""

echo "=========================================="
echo "Test Environment Ready!"
echo "=========================================="
echo ""
echo "You can now run your Qt application:"
echo "  ./build/your_rcws_app"
echo ""
echo "To cleanup later:"
echo "  ./scripts/cleanup_virtual_ports.sh"
echo "  pkill -f diagslave"
echo ""
