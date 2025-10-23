#!/bin/bash
DIAGSLAVE="$HOME/diagslave/x86_64-linux-gnu/diagslave"
echo "Starting Servo Elevation Simulator (Slave ID 1)"
$DIAGSLAVE -m rtu -a 1 -b 230400 -p none /dev/ttyVIRT_SERVO_EL1
