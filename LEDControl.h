#ifndef __LED_CONTROL_H__
#define __LED_CONTROL_H__

#include <vector>
#include <cstdint>

enum led_group { NET_LED, CPU_LED, BOINC_LED };

class LEDControl
{
  public:
    LEDControl();
    ~LEDControl();

    void turn_on(uint32_t pin, led_group group);
    void turn_off(uint32_t pin, led_group group);

    void turn_off_boinc_pins();
    void turn_on_boinc_pins(uint32_t number);

  private:
    std::vector<uint32_t> net_pins   = { 17, 18 };
    std::vector<uint32_t> cpu_pins   = { 27, 22, 23, 24 };
    std::vector<uint32_t> boinc_pins = { 21, 20, 16, 26, 19, 13 };
};

#endif //__LED_CONTROL_H__
