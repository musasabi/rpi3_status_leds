#ifndef __BOINC_SPECS_H__
#define __BOINC_SPECS_H__

#include <string>

#define BOINC_MAX_LINE_LENGTH 64

enum client_states { result_new, downloading, downloaded, compute_error,
                     uploading, uploaded, aborted, upload_failed,
                     max_client_states };

std::string client_state_strings[9] =
                                    { "result_new", "downloading", "downloaded",
                                      "compute_error", "uploading", "uploaded",
                                      "aborted", "upload_failed", "invalid" };

enum scheduler_states { uninit_scheduler, preempted, scheduled,
                        max_scheduler_states };

std::string scheduler_state_strings[4] = { "uninitialized", "preempted",
                                           "scheduled", "invalid" };


enum task_states { uninint_task, executing, suspended, abort_pending, 
                   quit_pending, copy_pending, max_task_states };

std::string task_state_strings[7] = { "UNINITIALIZED", "EXECUTING", "SUSPENDED",
                                      "ABORT_PENDING", "QUIT_PENDING",
                                      "COPY_PENDING", "invalid" };

struct BOINCTask
{
  std::string name;
  client_states client_state;
  scheduler_states scheduler_state;
  task_states task_state;
  float fraction_done;
  float estimated_time_remaining;
};

#endif //__BOINC_SPECS_H__
