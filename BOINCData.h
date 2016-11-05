#ifndef __BOINCDATA_H__
#define __BOINCDATA_H__

#include <map>
#include <string>
#include <memory>
#include <fstream>

#define BOINC_TMP_FILE "/tmp/boincdata"

class LEDControl;
struct BOINCTask;

class BOINCData
{
  public:
    BOINCData(LEDControl *led_control);
    ~BOINCData() {};

    void update();
    void display();

  private:
    LEDControl *led;

    std::map<std::string, std::shared_ptr<BOINCTask>> tasks;

    std::shared_ptr<BOINCTask> parse_boinc_data(std::ifstream& boinc_info);
    void boinc_skip_lines(std::ifstream& boinc_info, uint32_t num_lines);
    void boinc_skip_to_value(std::ifstream& boinc_info);

    void update_leds();
};

#endif //__BOINCDATA_H__

