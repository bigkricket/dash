#pragma once
#include <stdint.h>

// Task that discovers supported OBD2 PIDs (0x01–0x80)
void get_supported_pids_task(void *arg);

const uint8_t* get_supported_pid_list(int *length);
