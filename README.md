<snippet>
  <content><![CDATA[
# ${1:RPi3 Status LED's}
Uses the Raspberry Pi's (40-pin) GPIO to drive a few LED's for real-time status
updates on the progress of BOINC work units.
## Installation
make wp
make

Then copy the status-leds binary wherever you like. No config, no parameters.
## Usage
If you want to change the pinout, feel free to edit LEDControl.h. Because this
program runs as non-privileged user, the wiringPi library requires you to use
the Broadcom nomenclature for the pins.

This is the best pin guide I've seen so far: http://pinout.xyz/
## TODO
2016-11-12 - ping is hanging after a while. What's going on?
## History
2016-11-12
-----------
Initial commitChanged how LED's are managed and re-assigned the BOINC pins.

2016-11-05
-----------
Initial commit
]]></content>
  <tabTrigger>readme</tabTrigger>
</snippet>