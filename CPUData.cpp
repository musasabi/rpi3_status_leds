#include "CPUData.h"
#include "LEDControl.h"

#include <fstream>
#include <iostream>
#include <numeric>
#include <thread>
#include <chrono>
#include <string>

using namespace std;

CPUData::CPUData(LEDControl *led_control) :
cores(0),
cpu_temperature(0.0f)
{
  led = led_control;

  ifstream cpuinfo("/proc/cpuinfo");

  if(!cpuinfo || cpuinfo.eof())
  {
    cerr << "Failed to open /proc/cpuinfo. Unknown core count." << endl;
  }
  else
  {
    string input;
    while(getline(cpuinfo, input))
    {
      if(input.substr(0, 9) == "processor")
      {
        cores = atoi(input.substr(12, 5).c_str());
      }
    }

    cores++; //cpuinfo has zero-based core numbers
  }

  cpuinfo.close();

  update();
}

void CPUData::update()
{
  update_temperature();
  update_frequency();
  update_utilization();

  update_leds();
}

void CPUData::update_leds()
{
  if(cpu_utilization.size() < 4)
  {
    cerr << "Less than four cores. Not touching LED's" << endl;
    return;
  }

  if(cpu_utilization[0] > 25) led->turn_on(cpu_led_1);
  else led->turn_off(cpu_led_1);

  if(cpu_utilization[1] > 25) led->turn_on(cpu_led_2);
  else led->turn_off(cpu_led_2);

  if(cpu_utilization[2] > 25) led->turn_on(cpu_led_3);
  else led->turn_off(cpu_led_3);

  if(cpu_utilization[3] > 25) led->turn_on(cpu_led_4);
  else led->turn_off(cpu_led_4);
}

void CPUData::display()
{
  cout << cpu_temperature << "'C" << endl;

  string core_clocks;
  string utilization;

  for(uint32_t core = 0; core < cores; core++)
  {
    core_clocks.append(to_string(cpu_frequency[core]));
    utilization.append(to_string(cpu_utilization[core]));

    if(core != cores-1)
    {
      core_clocks.append("/");
      utilization.append("/");
    }
  }
  core_clocks.append("mhz");
  utilization.append("%");

  cout << core_clocks << endl;
  cout << utilization << endl;
}

void CPUData::update_temperature()
{
  string path = "/sys/class/thermal/thermal_zone0/temp";
  ifstream temperature(path);
  if(!temperature || temperature.eof())
  {
    cerr << "Failed to open "<<path<<". Unknown temperature." << endl;
    return;
  }

  temperature >> cpu_temperature;
  temperature.close();

  cpu_temperature/=1000.0f;
}

void CPUData::update_frequency()
{
  string leading_path = "/sys/devices/system/cpu/cpu";
  string trailing_path = "/cpufreq/cpuinfo_cur_freq";

  uint32_t clock = 0;

  for(uint32_t core = 0; core < cores; core++)
  {
    string path = leading_path;
    path.append(to_string(core));
    path.append(trailing_path);
    ifstream current_clock(path);

    if(!current_clock || current_clock.eof())
    {
      cerr << "Unable to probe clock for core " << core << endl;
    }

    current_clock >> clock;
    clock/=1000;

    if(cpu_frequency.size() < core+1) cpu_frequency.push_back(clock);
    else cpu_frequency[core] = clock;

    current_clock.close();
  }
}

void CPUData::update_utilization()
{
  uint32_t first_idle_time;
  uint32_t first_total_time;
  uint32_t idle_time;
  uint32_t total_time;

  float idle_delta;
  float total_delta;

  uint32_t utilization;

  for(uint32_t core = 0; core < cores; core++)
  {
    if(!get_cpu_time(core, first_idle_time, first_total_time)) return;
    this_thread::sleep_for(chrono::milliseconds(250));
    if(!get_cpu_time(core, idle_time, total_time)) return;

    idle_delta = idle_time - first_idle_time;
    total_delta = total_time - first_total_time;

    utilization = (100 * (1.0f - idle_delta / total_delta));

    if(cpu_utilization.size() < core+1) cpu_utilization.push_back(utilization);
    else cpu_utilization[core] = utilization;

  }
}

bool CPUData::get_cpu_time(uint32_t core, 
                           uint32_t& idle_time, uint32_t& total_time)
{
  vector<uint32_t> times;
  ifstream proc_stat("/proc/stat");

  if(!proc_stat || proc_stat.eof())
  {
    cerr << "Failed to open /proc/stat. Not updating." << endl;
    return false;
  }

  //proc stat lists the overall cpu times on its first line, so we always
  //want to ignore at least one line
  for(uint32_t lines_to_ignore = 0; lines_to_ignore < core+1; lines_to_ignore++)
  {
    proc_stat.ignore(numeric_limits<streamsize>::max(), '\n');
  }

  proc_stat.ignore(5, ' ');
  for(uint32_t time = 0; proc_stat >> time; times.push_back(time));

  proc_stat.close();

  if(times.size() < 4)
  {
    cerr << "Less than four CPU time samples from /proc/stat. Not updating."
         << endl;
    return false;
  }

  idle_time = times[3];
  total_time = accumulate(times.begin(), times.end(), 0);

  return true;
}
