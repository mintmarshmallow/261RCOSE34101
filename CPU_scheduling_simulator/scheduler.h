#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"



void run_fcfs(Process processes[], int num_processes);
void run_sjf_np(Process processes[], int num_processes);
void run_sjf_p(Process processes[], int num_processes);
void run_priority_np(Process processes[], int num_processes);
void run_priority_p(Process processes[], int num_processes);
void run_rr(Process processes[], int num_processes);
void run_priority_aging(Process processes[], int num_processes);

#endif
