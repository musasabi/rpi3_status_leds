#include "LEDControl.h"
#include "CPUData.h"
#include "NetworkData.h"
#include "BOINCData.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <csignal>
#include <cstring>

#include <iomanip>
#include <ctime>

using namespace std;

//necessary globals for signal handling
namespace
{
  volatile sig_atomic_t done = 0;

  void caught_signal(int signal)
  {
    done = signal;
  }
}

int main()
{
  //set up signal handling
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = caught_signal;
  sigfillset(&sa.sa_mask);
  sigaction(SIGINT, &sa, nullptr);
  sigaction(SIGTERM, &sa, nullptr);

  LEDControl led_control; //must be instantiated first

  CPUData cpu_data(&led_control);
  BOINCData boinc_data(&led_control);
  NetworkData network_data(&led_control);

  uint64_t update = 0;

  while(!done)
  {
    this_thread::sleep_for(chrono::seconds(5));

    //timestamp for logs
    char timestamp[64];
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime(timestamp, sizeof(timestamp), "%Y%m%d %H:%M:%S", timeinfo);
    cout << timestamp << endl;

    update++;

    cout << "  net " << update << endl;
    network_data.update();
    network_data.display();

    cout << "  cpu " << update << endl;
    cpu_data.update();
    cpu_data.display();

    cout << "  boinc " << update << endl;
    boinc_data.update();
    boinc_data.display();
  }

  return 0;
}

