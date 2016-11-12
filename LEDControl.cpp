#include "LEDControl.h"

#include <wiringPi.h>

#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

LEDControl::LEDControl()
{
  wiringPiSetupSys();

  vector<uint32_t>::iterator pin;

  for(pin = net_pins.begin(); pin != net_pins.end(); pin++)
  {
    digitalWrite(*pin, HIGH);
    this_thread::sleep_for(chrono::milliseconds(50));
  }

  for(pin = cpu_pins.begin(); pin != cpu_pins.end(); pin++)
  {
    digitalWrite(*pin, HIGH);
    this_thread::sleep_for(chrono::milliseconds(50));
  }

  for(pin = boinc_pins.begin(); pin != boinc_pins.end(); pin++)
  {
    digitalWrite(*pin, HIGH);
    this_thread::sleep_for(chrono::milliseconds(50));
  }

  for(pin = net_pins.begin(); pin != net_pins.end(); pin++)
  {
    digitalWrite(*pin, LOW);
    this_thread::sleep_for(chrono::milliseconds(50));
  }

  for(pin = cpu_pins.begin(); pin != cpu_pins.end(); pin++)
  {
    digitalWrite(*pin, LOW);
    this_thread::sleep_for(chrono::milliseconds(50));
  }

  for(pin = boinc_pins.begin(); pin != boinc_pins.end(); pin++)
  {
    digitalWrite(*pin, LOW);
    this_thread::sleep_for(chrono::milliseconds(50));
  }
}

LEDControl::~LEDControl()
{
  vector<uint32_t>::iterator pin;

  for(pin = net_pins.begin(); pin != net_pins.end(); pin++)
  {
    digitalWrite(*pin, LOW);
  }

  for(pin = cpu_pins.begin(); pin != cpu_pins.end(); pin++)
  {
    digitalWrite(*pin, LOW);
  }

  for(pin = boinc_pins.begin(); pin != boinc_pins.end(); pin++)
  {
    digitalWrite(*pin, LOW);
  }
}

void LEDControl::turn_on(uint32_t pin, led_group group)
{
  vector<uint32_t> *pins = nullptr;

  switch(group)
  {
    case NET_LED:
        pins = &net_pins;
      break;
    case CPU_LED:
        pins = &cpu_pins;
      break;
    case BOINC_LED:
        pins = &boinc_pins;
      break;
    default:
      cerr << "Unknown LED group ("<<group<<")"<<endl;
      break;
  }

  if(digitalRead((*pins)[pin]) == LOW)
  {
    digitalWrite((*pins)[pin], HIGH);
  }
}

void LEDControl::turn_off(uint32_t pin, led_group group)
{
  vector<uint32_t> *pins = nullptr;

  switch(group)
  {
    case NET_LED:
        pins = &net_pins;
      break;
    case CPU_LED:
        pins = &cpu_pins;
      break;
    case BOINC_LED:
        pins = &boinc_pins;
      break;
    default:
      cerr << "Unknown LED group ("<<group<<")"<<endl;
      break;
  }

  if(digitalRead((*pins)[pin]) == HIGH)
  {
    digitalWrite((*pins)[pin], LOW);
  }
}

void LEDControl::turn_off_boinc_pins()
{
  for(int32_t pin = boinc_pins.size()-1; pin >= 0; pin--)
  {
    turn_off(pin, BOINC_LED);
    this_thread::sleep_for(chrono::milliseconds(100));
  }
}

void LEDControl::turn_on_boinc_pins(uint32_t number)
{
  for(uint32_t pin = 0; pin != number; pin++)
  {
    turn_on(pin, BOINC_LED);
    this_thread::sleep_for(chrono::milliseconds(100));
  }
}
