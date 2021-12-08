#!/bin/bash

PWM=$1
DEVICE=/dev/ttyACM0

# Configure serial line (read_timeout=1sec, baudrate 115200)
/usr/bin/stty -F $DEVICE -ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke min 0 time 1 115200

echo "pwm $PWM" > $DEVICE
cat < $DEVICE

