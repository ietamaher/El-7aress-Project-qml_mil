#!/bin/bash
DIAGSLAVE="$HOME/diagslave/x86_64-linux-gnu/diagslave"
echo "Starting Servo Azimuth Simulator (Slave ID 2)"
$DIAGSLAVE -m rtu -a 2 -b 230400 -p none /dev/ttyVIRT_SERVO_AZ1
