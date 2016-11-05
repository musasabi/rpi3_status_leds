#ifndef __NETWORKDATA_H__
#define __NETWORKDATA_H__

#include <string>
#include <cstdint>

#define MAX_HOSTS 2

class LEDControl;

enum host_enums { serverpi, eah };

class NetworkData
{
  public:
    NetworkData(LEDControl *led_control);
    ~NetworkData() {};

    void update();
    void display();

  private:
    LEDControl *led;

    std::string hostnames[MAX_HOSTS] = { "serverpi",
                                         "einstein.phys.uwm.edu" };

    bool hosts_reached[MAX_HOSTS];

    bool ping(std::string host);
    int32_t checksum(uint16_t *buf, int32_t len);

    void update_leds();
};

#endif //__NETWORKDATA_H__
