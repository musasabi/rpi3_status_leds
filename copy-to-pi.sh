#!/bin/bash

make clean

cd ..

rsync -avr -e "ssh" --exclude '.*' status-leds/ serverpi:status-leds/
ssh serverpi "rsync -avr -e 'ssh' status-leds/ pi3:status-leds/"
