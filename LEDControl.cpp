#include "LEDControl.h"

#include <wiringPi.h>

#include <thread>
#include <chrono>

#include <iostream>

using namespace std;

LEDControl::LEDControl()
{
  wiringPiSetupSys();

  vector<uint32_t>::iterator pin;

  for(pin = pins.begin(); pin != pins.end(); pin++)
  {
    digitalWrite(*pin, HIGH);
    this_thread::sleep_for(chrono::milliseconds(50));
  }

  for(pin = pins.begin(); pin != pins.end(); pin++)
  {
    digitalWrite(*pin, LOW);
    this_thread::sleep_for(chrono::milliseconds(50));
  }
}

LEDControl::~LEDControl()
{
  vector<uint32_t>::iterator pin;

  for(pin = pins.begin(); pin != pins.end(); pin++)
  {
    digitalWrite(*pin, LOW);
  }
}

void LEDControl::turn_on(uint32_t pin)
{
  if(digitalRead(pins[pin]) == LOW)
  {
    digitalWrite(pins[pin], HIGH);
  }
}

void LEDControl::turn_off(uint32_t pin)
{
  if(digitalRead(pins[pin]) == HIGH)
  {
    digitalWrite(pins[pin], LOW);
  }
}

void LEDControl::turn_off_boinc_pins()
{
  for(reverse_iterator<vector<uint32_t>::iterator> pin = pins.rbegin();
      pin != pins.rend();
      pin++)
  {
    turn_off(*pin);
    this_thread::sleep_for(chrono::milliseconds(50));
  }
}

void LEDControl::turn_on_boinc_pins(uint32_t number)
{
  for(uint32_t pin = 0; pin != number; pin++)
  {
    turn_on(boinc_pins[pin]);
    this_thread::sleep_for(chrono::milliseconds(50));
  }
}
