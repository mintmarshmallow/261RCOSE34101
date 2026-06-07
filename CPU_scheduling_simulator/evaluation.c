#include "evaluation.h"
#include <stdio.h>
#include <string.h>

GanttEvent gantt_chart[MAX_GANTT_EVENTS];
int num_gantt_events = 0;



void add_gantt_event(int start_time, int end_time, int pid) {
    if (num_gantt_events > 0 && gantt_chart[num_gantt_events - 1].pid == pid) {
        // 이전 이벤트와 같은 프로세스면 합침
        gantt_chart[num_gantt_events - 1].end_time = end_time;
    } else {
        if (num_gantt_events < MAX_GANTT_EVENTS) {
            gantt_chart[num_gantt_events].start_time = start_time;
            gantt_chart[num_gantt_events].end_time = end_time;
            gantt_chart[num_gantt_events].pid = pid;
            num_gantt_events++;
        }
    }
}

void print_gantt_chart() {
    printf("\n==== Gantt Chart ====\n");
    for (int i = 0; i < num_gantt_events; i++) {
        printf("[%d~%d] P%d\n", gantt_chart[i].start_time, gantt_chart[i].end_time, gantt_chart[i].pid);
    }
    printf("=====================\n\n");
}

SimulationResult evaluate_and_print_results(Process processes[], int num_processes, const char* scheduler_name) {
    int total_waiting = 0;
    int total_turnaround = 0;
    SimulationResult result;

    
    printf("\n>> Simulation Results for %s <<\n", scheduler_name);
    strncpy(result.name, scheduler_name, 49);
    result.name[49] = '\0';
    printf("PID\tArrival\tPriority\tBurst\tCompletion\tTurnaround\tWaiting\n");
    
    for (int i = 0; i < num_processes; i++) {
        Process* p = &processes[i];
        
        // 대기시간 계산: (종료시간 - 도착시간 - 순수CPU사용시간 - 순수IO사용시간)
        int pure_cpu_time = 0;
        int pure_io_time = 0;
        for (int b = 0; b < p->num_cpu_bursts; b++) {
            pure_cpu_time += p->cpu_bursts[b];
            if (b < p->num_cpu_bursts - 1) {
                pure_io_time += p->io_bursts[b];
            }
        }
        
        p->total_turnaround_time = p->completion_time - p->arrival_time;
        // Waiting time = Turnaround time - (CPU Burst + I/O Burst)
        p->total_waiting_time = p->total_turnaround_time - pure_cpu_time - pure_io_time;
        if (p->total_waiting_time < 0) p->total_waiting_time = 0;
        
        total_waiting += p->total_waiting_time;
        total_turnaround += p->total_turnaround_time;
        
        printf("P%d\t%d\t%d\t\t%d\t%d\t\t%d\t\t%d\n",
               p->pid, p->arrival_time, p->priority, pure_cpu_time,
               p->completion_time, p->total_turnaround_time, p->total_waiting_time);
    }
    
    result.avg_waiting_time = (float)total_waiting / num_processes;
    result.avg_turnaround_time = (float)total_turnaround / num_processes;

    printf("\nAverage Waiting Time: %.2f\n", result.avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", result.avg_turnaround_time);

    print_gantt_chart();
    return result;
}
