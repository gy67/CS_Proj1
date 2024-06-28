#ifndef _SCHEDULING_H_
#define _SCHEDULING_H_

#include "linkedList.h"

#define PERCENTAGE_NUMBER 100

// Schedule processes and return the total time taken
int schedule_processes(list_t *input_queue, list_t *ready_queue, list_t *finished_queue, int quantum, char **strategy);
// print all running information
void print_running_info(node_t *process, int time, char **strategy, int *memory, int *pages);
// calculate memory usage,if the recording number is 1 it means the memory is used
int calculate_memory_usage(int memory[], int total);

#endif