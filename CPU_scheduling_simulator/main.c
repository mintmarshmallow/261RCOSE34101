#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "process.h"
#include "scheduler.h"
#include "evaluation.h"

int main() {
    srand(time(NULL));
    Process original_processes[MAX_PROCESSES];
    Process simulation_processes[MAX_PROCESSES];
    
    // 1. 프로세스 생성 (랜덤)
    generate_processes(original_processes);
    
    printf("========================================================================\n");
    printf("                       CPU Scheduling Simulator                         \n");
    printf("========================================================================\n\n");
    
    printf(">> Generated Processes <<\n");
    printf("PID\tArrival\tPriority\tBurst Sequence (CPU, I/O...)\n");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        printf("P%d\t%d\t%d\t\t", original_processes[i].pid, original_processes[i].arrival_time, original_processes[i].priority);
        for (int b = 0; b < original_processes[i].num_cpu_bursts; b++) {
            printf("[CPU %d]", original_processes[i].cpu_bursts[b]);
            if (b < original_processes[i].num_cpu_bursts - 1) {
                printf(" -> [I/O %d] -> ", original_processes[i].io_bursts[b]);
            }
        }
        printf("\n");
    }
    printf("------------------------------------------------------------------------\n");

    // 2. 7가지 스케줄링 알고리즘 개별 시뮬레이션 실행 (공정성을 위해 매번 Deep Copy)
    
    // 1) First-Come, First-Served (FCFS)
    deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
    run_fcfs(simulation_processes, MAX_PROCESSES);
    evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "FCFS");
    
    // 2) Non-Preemptive SJF
    deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
    run_sjf_np(simulation_processes, MAX_PROCESSES);
    evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Non-Preemptive SJF");
    
    // 3) Preemptive SJF (SRTF)
    deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
    run_sjf_p(simulation_processes, MAX_PROCESSES);
    evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Preemptive SJF (SRTF)");
    
    // 4) Non-Preemptive Priority
    deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
    run_priority_np(simulation_processes, MAX_PROCESSES);
    evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Non-Preemptive Priority");
    
    // 5) Preemptive Priority
    deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
    run_priority_p(simulation_processes, MAX_PROCESSES);
    evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Preemptive Priority");
    
    // 6) Round Robin (RR, Quantum = 4)
    deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
    run_rr(simulation_processes, MAX_PROCESSES);
    evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Round Robin (RR)");
    
    // 7) Priority Scheduling with Aging
    deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
    run_priority_aging(simulation_processes, MAX_PROCESSES);
    evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Priority with Aging");
    
    return 0;
}
