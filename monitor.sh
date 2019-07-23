#!/bin/bash

while true
do
    particle serial monitor | grep -ov "No serial port identified"
done
