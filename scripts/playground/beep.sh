#!/bin/bash

COUNTER=0

while [ $COUNTER -lt 15 ]; do
  echo 1 > /sys/class/gpio/gpio18/value
  ./usleep 333
  echo 0 > /sys/class/gpio/gpio18/value
  ./usleep 333
  let COUNTER+=1
done
