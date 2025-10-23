#!/bin/bash
DIAGSLAVE="$HOME/diagslave/x86_64-linux-gnu/diagslave"
echo "Starting PLC21 Simulator (Slave ID 31)"
$DIAGSLAVE -m rtu -a 31 -b 115200 -p even /dev/ttyVIRT_PLC21_1
