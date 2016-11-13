#include "BOINC_specs.h"
#include "BOINCData.h"
#include "LEDControl.h"

#include <iostream>
#include <cstdlib>
#include <limits>
#include <utility>
#include <thread>
#include <chrono>

using namespace std;

BOINCData::BOINCData(LEDControl *led_control)
{
  led = led_control;
  update();
}

void BOINCData::update()
{
  string boinccmd = "boinccmd --get_tasks > ";
  boinccmd.append(BOINC_TMP_FILE);
  system(boinccmd.c_str());

  ifstream boinc_info(BOINC_TMP_FILE);
  if(!boinc_info || boinc_info.eof())
  {
    cerr << "Unable to open " << BOINC_TMP_FILE << endl;
    return;
  }

  string input;
  shared_ptr<BOINCTask> task;
  while(getline(boinc_info, input))
  {
    //find the first task number and go from there
    uint32_t task_number = atoi(input.c_str());
    if(task_number <= 0) continue;

    task = parse_boinc_data(boinc_info);

    tasks[task->name] = task;
  }

  boinc_info.close();

  boinccmd = "rm ";
  boinccmd.append(BOINC_TMP_FILE);
  system(boinccmd.c_str());

  update_leds();
}

void BOINCData::update_leds()
{
  uint32_t number_active_tasks = 0;
  float total_progress = 0;

  for(const auto& task : tasks)
  {
    if(task.second->task_state != executing) continue;

    number_active_tasks++;
    total_progress += task.second->fraction_done;
  }

  float average_progress = total_progress/number_active_tasks;

  uint32_t number_leds_to_light = 6 * average_progress;

  led->turn_off_boinc_pins();
  this_thread::sleep_for(chrono::milliseconds(250));
  led->turn_on_boinc_pins(number_leds_to_light);
}

void BOINCData::display()
{
  for(const auto& task : tasks)
  {
    if(task.second->fraction_done <= 0) continue;

    cout << "Task Name: " << task.second->name << endl;
    cout << "State: " << client_state_strings[task.second->client_state]
         << endl;
    cout << "Scheduler State: "
         << scheduler_state_strings[task.second->scheduler_state]
         << endl;
    cout << "Task State: " << task_state_strings[task.second->task_state]
         << endl;
    cout << "Fraction: " << task.second->fraction_done << endl;
    cout << "ETA: " << task.second->estimated_time_remaining << endl;
  }
}

shared_ptr<BOINCTask> BOINCData::parse_boinc_data(ifstream& boinc_info)
{
  shared_ptr<BOINCTask> task = make_shared<BOINCTask>();
  string input;

  //retrieve task name
  boinc_skip_to_value(boinc_info);
  getline(boinc_info, task->name);

  //retrieve client state
  boinc_skip_lines(boinc_info, 6);
  boinc_skip_to_value(boinc_info);
  getline(boinc_info, input);

  task->client_state = max_client_states;
  for(uint32_t state = result_new; state < max_client_states; state++)
  {
    if(input == client_state_strings[state])
    {
      task->client_state = (client_states) state;
    }
  }

  //retrieve scheduler state
  boinc_skip_to_value(boinc_info);
  getline(boinc_info, input);

  task->scheduler_state = max_scheduler_states;
  for(uint32_t state = uninit_scheduler; state < max_scheduler_states; state++)
  {
    if(input == scheduler_state_strings[state])
    {
      task->scheduler_state = (scheduler_states) state;
    }
  }

  //retrieve task state
  boinc_skip_lines(boinc_info, 3);
  boinc_skip_to_value(boinc_info);
  getline(boinc_info, input);

  task->task_state = max_task_states;
  for(uint32_t state = uninint_task; state < max_task_states; state++)
  {
    if(input == task_state_strings[state])
    {
      task->task_state = (task_states) state;
    }
  }

  //retrieve fraction done
  boinc_skip_lines(boinc_info, 3);
  boinc_skip_to_value(boinc_info);
  getline(boinc_info, input);

  task->fraction_done = stof(input);

  //retrieve ETA
  boinc_skip_lines(boinc_info, 2);
  boinc_skip_to_value(boinc_info);
  getline(boinc_info, input);

  task->estimated_time_remaining = stof(input);

  return task;
}

void BOINCData::boinc_skip_lines(std::ifstream& boinc_info, uint32_t num_lines)
{
  for(uint32_t line = 0; line < num_lines; line++)
  {
    boinc_info.ignore(numeric_limits<streamsize>::max(), '\n');
  }
}
void BOINCData::boinc_skip_to_value(std::ifstream& boinc_info)
{
  boinc_info.ignore(numeric_limits<streamsize>::max(), ':');
  boinc_info.ignore(1);
}
