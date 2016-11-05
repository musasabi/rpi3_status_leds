#ifndef __LED_CONTROL_H__
#define __LED_CONTROL_H__

#include <vector>
#include <cstdint>

#define MAX_NET_PINS 2
#define MAX_CPU_PINS 4
#define MAX_BOINC_PINS 6

enum led_pins {
                net_led_1, net_led_2,
                cpu_led_1, cpu_led_2, cpu_led_3, cpu_led_4
              };

class LEDControl
{
  public:
    LEDControl();
    ~LEDControl();

    void turn_on(uint32_t pin);
    void turn_off(uint32_t pin);

    void turn_off_boinc_pins();
    void turn_on_boinc_pins(uint32_t number);

  private:
    std::vector<uint32_t> pins = {
                                   17, 18,                 //network LED's
                                   27, 22, 23, 24          //CPU LED's
                                 };

    std::vector<uint32_t> boinc_pins = {
                                         25, 4, 28, 29, 30, 31
                                       };
};

#endif //__LED_CONTROL_H__
