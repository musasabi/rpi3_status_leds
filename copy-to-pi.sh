#!/bin/bash

make clean

cd ..

scp -r status-leds serverpi:
ssh serverpi "scp -r status-leds pi3:"
