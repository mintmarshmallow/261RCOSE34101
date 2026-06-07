#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "process.h"
#include "scheduler.h"
#include "evaluation.h"

void print_simulation_ranking(SimulationResult results[]) {
    SimulationResult temp[7];
    int valid_count = 0;
    
    // 유효한(실행된) 결과만 temp 배열에 모음
    for(int i=0; i<7; i++) {
        if(results[i].avg_waiting_time != INFINITY) {
            temp[valid_count] = results[i];
            valid_count++;
        }
    }
    
    if (valid_count == 0) {
        printf("No simulation results yet. Please run a simulation first.\n\n");
        return;
    }
    
    // Waiting Time 기준 정렬 (버블 정렬)
    for(int i=0; i<valid_count-1; i++) {
        for(int j=0; j<valid_count-1-i; j++) {
            if(temp[j].avg_waiting_time > temp[j+1].avg_waiting_time) {
                SimulationResult t = temp[j];
                temp[j] = temp[j+1];
                temp[j+1] = t;
            }
        }
    }
    printf("\n[Top 3 by Average Waiting Time]\n");
    int top_count = (valid_count < 3) ? valid_count : 3;
    for(int i=0; i<top_count; i++) {
        printf("  %d. %s (%.2f)\n", i+1, temp[i].name, temp[i].avg_waiting_time);
    }
    if (valid_count > 1) {
        printf("  Worst: %s (%.2f)\n", temp[valid_count-1].name, temp[valid_count-1].avg_waiting_time);
    }
    
    // Turnaround Time 기준 정렬을 위해 다시 유효한 결과만 복사
    valid_count = 0;
    for(int i=0; i<7; i++) {
        if(results[i].avg_turnaround_time != INFINITY) {
            temp[valid_count] = results[i];
            valid_count++;
        }
    }
    
    for(int i=0; i<valid_count-1; i++) {
        for(int j=0; j<valid_count-1-i; j++) {
            if(temp[j].avg_turnaround_time > temp[j+1].avg_turnaround_time) {
                SimulationResult t = temp[j];
                temp[j] = temp[j+1];
                temp[j+1] = t;
            }
        }
    }
    printf("\n[Top 3 by Average Turnaround Time]\n");
    for(int i=0; i<top_count; i++) {
        printf("  %d. %s (%.2f)\n", i+1, temp[i].name, temp[i].avg_turnaround_time);
    }
    if (valid_count > 1) {
        printf("  Worst: %s (%.2f)\n\n", temp[valid_count-1].name, temp[valid_count-1].avg_turnaround_time);
    } else {
        printf("\n");
    }
}

int main() {
    int toggle = 0;
    SimulationResult results[7];
    for (int i = 0; i < 7; i++) {
        results[i].avg_waiting_time = INFINITY;
        results[i].avg_turnaround_time = INFINITY;
    }
    Process original_processes[MAX_PROCESSES];
    Process simulation_processes[MAX_PROCESSES];

    printf("========================================================================\n");
    printf("                       CPU Scheduling Simulator                         \n");
    printf("========================================================================\n\n");
    
    while(1){
        
        if(toggle){            
            printf("0. Exit\n");
            printf("1. Generate New Processes\n");
            printf("2. Run All Simulations\n");
            printf("3. FCFS \n");
            printf("4. SJF (Non-Preemptive) \n");
            printf("5. SJF (Preemptive) \n");
            printf("6. Priority (Non-Preemptive) \n");
            printf("7. Priority (Preemptive) \n");
            printf("8. RR (Quantum = 4)\n");
            printf("9. Priority with Aging \n");
            printf("10. Show Algorithm Rankings \n");
            printf("11. Show Processes Info \n");
            printf("Select: ");
        } else {
            printf("0. Exit\n");
            printf("1. Generate Random Processes\n");
            printf("Select: ");
            
        }
        
        
        int choice;
        if (scanf("%d", &choice) != 1) {
            // scanf가 숫자를 읽지 못한 경우 (문자 등 입력) 입력 버퍼를 비워 무한루프 방지
            while (getchar() != '\n');
            printf("\nInvalid input. Please enter a valid number.\n\n");
            continue;
        }
        if(choice == 1){
            srand(time(NULL)); 
                // 1. 프로세스 생성 (랜덤)
                generate_processes(original_processes);
        
        
                printf(">> Generated Processes <<\n");
                printf("PID\tArrival\tPriority\tBurst Sequence (CPU, I/O...)\n");
                for (int i = 0; i < MAX_PROCESSES; i++) {
                    results[i].avg_waiting_time = INFINITY;
                    results[i].avg_turnaround_time = INFINITY;
                    printf("P%d\t%d\t%d\t\t", original_processes[i].pid, original_processes[i].arrival_time, original_processes[i].priority);
                    for (int b = 0; b < original_processes[i].num_cpu_bursts; b++) {
                        printf("[CPU %d]", original_processes[i].cpu_bursts[b]);
                        if (b < original_processes[i].num_cpu_bursts - 1) {
                            printf(" -> [I/O %d] -> ", original_processes[i].io_bursts[b]);
                        }
                    }
                    printf("\n");
                }
                toggle = 1;
        } else if(choice == 2 && toggle){
            // 2. 7가지 스케줄링 알고리즘 개별 시뮬레이션 실행 (공정성을 위해 매번 Deep Copy)
    
            // 1) First-Come, First-Served (FCFS)
            deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
            run_fcfs(simulation_processes, MAX_PROCESSES);
            results[0] = evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "FCFS");
    
            // 2) Non-Preemptive SJF
            deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
            run_sjf_np(simulation_processes, MAX_PROCESSES);
            results[1] = evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Non-Preemptive SJF");
    
            // 3) Preemptive SJF (SRTF)
            deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
            run_sjf_p(simulation_processes, MAX_PROCESSES);
            results[2] = evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Preemptive SJF (SRTF)");
    
            // 4) Non-Preemptive Priority
            deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
            run_priority_np(simulation_processes, MAX_PROCESSES);
            results[3] = evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Non-Preemptive Priority");
    
            // 5) Preemptive Priority
            deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
            run_priority_p(simulation_processes, MAX_PROCESSES);
            results[4] = evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Preemptive Priority");
    
            // 6) Round Robin (RR, Quantum = 4)
            deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
            run_rr(simulation_processes, MAX_PROCESSES);
            results[5] = evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Round Robin (RR)");
    
            // 7) Priority Scheduling with Aging
            deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
            run_priority_aging(simulation_processes, MAX_PROCESSES);
            results[6] = evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Priority with Aging");
            
        } else if(choice == 3 && toggle){
            // 1) First-Come, First-Served (FCFS)
            deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
            run_fcfs(simulation_processes, MAX_PROCESSES);
            results[0] = evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "FCFS");
        } else if(choice == 4 && toggle){
            // 2) Non-Preemptive SJF
            deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
            run_sjf_np(simulation_processes, MAX_PROCESSES);
            results[1] = evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Non-Preemptive SJF");
        } else if(choice == 5 && toggle){
            // 3) Preemptive SJF (SRTF)
            deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
            run_sjf_p(simulation_processes, MAX_PROCESSES);
            results[2] = evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Preemptive SJF (SRTF)");
        } else if(choice == 6 && toggle){
            // 4) Non-Preemptive Priority
            deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
            run_priority_np(simulation_processes, MAX_PROCESSES);
            results[3] = evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Non-Preemptive Priority");
        } else if(choice == 7 && toggle){
            // 5) Preemptive Priority
            deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
            run_priority_p(simulation_processes, MAX_PROCESSES);
            results[4] = evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Preemptive Priority");
        } else if(choice == 8 && toggle){
            // 6) Round Robin (RR, Quantum = 4)
            deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
            run_rr(simulation_processes, MAX_PROCESSES);
            results[5] = evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Round Robin (RR)");
        } else if(choice == 9 && toggle){
            // 7) Priority Scheduling with Aging
            deep_copy_processes(simulation_processes, original_processes, MAX_PROCESSES);
            run_priority_aging(simulation_processes, MAX_PROCESSES);
            results[6] = evaluate_and_print_results(simulation_processes, MAX_PROCESSES, "Priority with Aging");
        } else if(choice == 10 && toggle){
            printf("========================================================================\n");
            printf("                      [Current Simulation Rankings]                     \n");
            printf("========================================================================\n\n");

            print_simulation_ranking(results);
        } else if(choice == 11 && toggle){
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
        } else if(choice == 0){
            printf("Simulator terminated.\n");
            return 0;
        } else {
            printf("Invalid choice. Please try again.\n");
        }
        
    }
    
    
    
    

    
    
    return 0;
}
