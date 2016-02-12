#!/bin/bash

prev_value="1"

while true; do
  value=`cat /sys/class/gpio/gpio4/value`
  if [ "$prev_value" != "$value" ]; then
    echo "$value"
    prev_value="$value"
  fi
  sleep 0.010
done
