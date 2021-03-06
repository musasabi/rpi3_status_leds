<snippet>
# RPi3 Status LED's
Uses the Raspberry Pi's (40-pin) GPIO to drive a few LED's for real-time status
updates on the progress of BOINC work units. This project uses the wiringPi
library: http://wiringpi.com/

## Installation
`$ make wp` - This builds the wiringPi library and related utilities

`$ make`

Then copy the status-leds binary wherever you like. No config, no parameters.

## Usage
If you want to change the pinout, feel free to edit `LEDControl.h`. Because this
program runs as non-privileged user, the wiringPi library requires you to use
the Broadcom nomenclature for the pins.

You'll also need to edit the list in `export_gpio.sh` to match.

If you're looking for help on which pin corresponds with which number, this
guide helped me: http://pinout.xyz/

## TODO
2016-11-13: I fixed ping, but now it just waits if I take down serverpi's wifi

## History
**2016-11-13:** Swapped out ping for a simpler one, adding in checks

**2016-11-12:** Changed how LED's are managed and re-assigned the
  BOINC pins.

**2016-11-05:** Initial commit
</snippet>
