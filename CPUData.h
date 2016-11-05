#ifndef __CPUDATA_H__
#define __CPUDATA_H__

#include <vector>
#include <cstdint>

class LEDControl;

class CPUData
{
  public:
    CPUData(LEDControl *led_control);
    ~CPUData() {};

    void update();
    void update_temperature();
    void update_frequency();
    void update_utilization();

    void display();

  private:
    LEDControl *led;

    uint32_t cores;

    float cpu_temperature;
    std::vector<uint32_t> cpu_frequency; //in mhz
    std::vector<uint32_t> cpu_utilization;

    bool get_cpu_time(uint32_t core, uint32_t& idle_time, uint32_t& total_time);

    void update_leds();
};

#endif //__CPUDATA_H__
