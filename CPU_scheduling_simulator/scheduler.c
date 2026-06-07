#include "scheduler.h"
#include "evaluation.h"
#include <stdio.h>

// 1. First-Come, First-Served (FCFS) 스케줄러
void run_fcfs(Process processes[], int num_processes) {
    Queue ready_queue;
    init_queue(&ready_queue);
    
    int tick = 0;
    int completed_processes = 0;
    Process* running_process = NULL;
    
    num_gantt_events = 0; // 간트 차트 초기화
    
    while (completed_processes < num_processes) {
        // 도착한 프로세스 Ready 큐 삽입
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].state == NEW && processes[i].arrival_time == tick) {
                processes[i].state = READY;
                enqueue(&ready_queue, &processes[i]);
            }
        }
        
        // I/O 대기(Waiting) 처리 - 전체 배열 순회
        for (int i = 0; i < num_processes; i++) {
            Process* p = &processes[i];
            if (p->state == WAITING) {
                p->remaining_io_time--;
                if (p->remaining_io_time <= 0) {
                    p->state = READY;
                    p->current_burst_idx++;
                    p->remaining_burst_time = p->cpu_bursts[p->current_burst_idx];
                    enqueue(&ready_queue, p);
                }
            }
        }
        
        // 프로세스 디스패치 (비선점)
        if (running_process == NULL && !is_queue_empty(&ready_queue)) {
            running_process = dequeue(&ready_queue);
            running_process->state = RUNNING;
        }
        
        // CPU 실행 진행
        if (running_process != NULL) {
            add_gantt_event(tick, tick + 1, running_process->pid);
            running_process->remaining_burst_time--;
            
            if (running_process->remaining_burst_time <= 0) {
                if (running_process->current_burst_idx < running_process->num_cpu_bursts - 1) {
                    running_process->state = WAITING;
                    running_process->remaining_io_time = running_process->io_bursts[running_process->current_burst_idx];
                } else {
                    running_process->state = TERMINATED;
                    running_process->completion_time = tick + 1;
                    completed_processes++;
                }
                running_process = NULL;
            }
        } else {
            // IDLE 상태를 간트 차트에 기록 (-1을 IDLE로 간주)
            add_gantt_event(tick, tick + 1, -1);
        }
        tick++;
    }
}

// 2. Non-Preemptive SJF 스케줄러
void run_sjf_np(Process processes[], int num_processes) {
    Queue ready_queue;
    init_queue(&ready_queue);
    
    int tick = 0;
    int completed_processes = 0;
    Process* running_process = NULL;
    
    num_gantt_events = 0; // 간트 차트 초기화
    
    while (completed_processes < num_processes) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].state == NEW && processes[i].arrival_time == tick) {
                processes[i].state = READY;
                sorted_enqueue_sjf(&ready_queue, &processes[i]);
            }
        }
        
        for (int i = 0; i < num_processes; i++) {
            Process* p = &processes[i];
            if (p->state == WAITING) {
                p->remaining_io_time--;
                if (p->remaining_io_time <= 0) {
                    p->state = READY;
                    p->current_burst_idx++;
                    p->remaining_burst_time = p->cpu_bursts[p->current_burst_idx];
                    sorted_enqueue_sjf(&ready_queue, p);
                }
            }
        }
        
        if (running_process == NULL && !is_queue_empty(&ready_queue)) {
            running_process = dequeue(&ready_queue);
            running_process->state = RUNNING;
        }
        
        if (running_process != NULL) {
            add_gantt_event(tick, tick + 1, running_process->pid);
            running_process->remaining_burst_time--;
            
            if (running_process->remaining_burst_time <= 0) {
                if (running_process->current_burst_idx < running_process->num_cpu_bursts - 1) {
                    running_process->state = WAITING;
                    running_process->remaining_io_time = running_process->io_bursts[running_process->current_burst_idx];
                } else {
                    running_process->state = TERMINATED;
                    running_process->completion_time = tick + 1;
                    completed_processes++;
                }
                running_process = NULL;
            }
        } else {
            // IDLE 상태를 간트 차트에 기록 (-1을 IDLE로 간주)
            add_gantt_event(tick, tick + 1, -1);
        }
        tick++;
    }
}

// 3. Preemptive SJF (SRTF) 스케줄러
void run_sjf_p(Process processes[], int num_processes) {
    Queue ready_queue;
    init_queue(&ready_queue);
    
    int tick = 0;
    int completed_processes = 0;
    Process* running_process = NULL;
    
    num_gantt_events = 0; // 간트 차트 초기화
    
    while (completed_processes < num_processes) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].state == NEW && processes[i].arrival_time == tick) {
                processes[i].state = READY;
                sorted_enqueue_sjf(&ready_queue, &processes[i]);
            }
        }
        
        for (int i = 0; i < num_processes; i++) {
            Process* p = &processes[i];
            if (p->state == WAITING) {
                p->remaining_io_time--;
                if (p->remaining_io_time <= 0) {
                    p->state = READY;
                    p->current_burst_idx++;
                    p->remaining_burst_time = p->cpu_bursts[p->current_burst_idx];
                    sorted_enqueue_sjf(&ready_queue, p);
                }
            }
        }
        
        // 선점 여부 검사 (매 tick마다 큐 맨 앞 프로세스와 비교)
        // 기능 명확화를 위해 도착한 프로세스를 검사하지 않고, 매 tick마다 ready queue를 검사
        if (running_process != NULL) {
            Process* front = peek_queue(&ready_queue);
            if (front != NULL && front->remaining_burst_time < running_process->remaining_burst_time) {
                running_process->state = READY;
                sorted_enqueue_sjf(&ready_queue, running_process);
                running_process = NULL;
            }
        }
        
        if (running_process == NULL && !is_queue_empty(&ready_queue)) {
            running_process = dequeue(&ready_queue);
            running_process->state = RUNNING;
        }
        
        if (running_process != NULL) {
            add_gantt_event(tick, tick + 1, running_process->pid);
            running_process->remaining_burst_time--;
            
            if (running_process->remaining_burst_time <= 0) {
                if (running_process->current_burst_idx < running_process->num_cpu_bursts - 1) {
                    running_process->state = WAITING;
                    running_process->remaining_io_time = running_process->io_bursts[running_process->current_burst_idx];
                } else {
                    running_process->state = TERMINATED;
                    running_process->completion_time = tick + 1;
                    completed_processes++;
                }
                running_process = NULL;
            }
        } else {
            // IDLE 상태를 간트 차트에 기록 (-1을 IDLE로 간주)
            add_gantt_event(tick, tick + 1, -1);
        }
        tick++;
    }
}

// 4. Non-Preemptive Priority 스케줄러
void run_priority_np(Process processes[], int num_processes) {
    Queue ready_queue;
    init_queue(&ready_queue);
    
    int tick = 0;
    int completed_processes = 0;
    Process* running_process = NULL;
    
    num_gantt_events = 0; // 간트 차트 초기화
    
    while (completed_processes < num_processes) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].state == NEW && processes[i].arrival_time == tick) {
                processes[i].state = READY;
                sorted_enqueue_priority(&ready_queue, &processes[i]);
            }
        }
        
        for (int i = 0; i < num_processes; i++) {
            Process* p = &processes[i];
            if (p->state == WAITING) {
                p->remaining_io_time--;
                if (p->remaining_io_time <= 0) {
                    p->state = READY;
                    p->current_burst_idx++;
                    p->remaining_burst_time = p->cpu_bursts[p->current_burst_idx];
                    sorted_enqueue_priority(&ready_queue, p);
                }
            }
        }
        
        if (running_process == NULL && !is_queue_empty(&ready_queue)) {
            running_process = dequeue(&ready_queue);
            running_process->state = RUNNING;
        }
        
        if (running_process != NULL) {
            add_gantt_event(tick, tick + 1, running_process->pid);
            running_process->remaining_burst_time--;
            
            if (running_process->remaining_burst_time <= 0) {
                if (running_process->current_burst_idx < running_process->num_cpu_bursts - 1) {
                    running_process->state = WAITING;
                    running_process->remaining_io_time = running_process->io_bursts[running_process->current_burst_idx];
                } else {
                    running_process->state = TERMINATED;
                    running_process->completion_time = tick + 1;
                    completed_processes++;
                }
                running_process = NULL;
            }
        } else {
            // IDLE 상태를 간트 차트에 기록 (-1을 IDLE로 간주)
            add_gantt_event(tick, tick + 1, -1);
        }
        tick++;
    }
}

// 5. Preemptive Priority 스케줄러
void run_priority_p(Process processes[], int num_processes) {
    Queue ready_queue;
    init_queue(&ready_queue);
    
    int tick = 0;
    int completed_processes = 0;
    Process* running_process = NULL;
    
    num_gantt_events = 0; // 간트 차트 초기화
    
    while (completed_processes < num_processes) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].state == NEW && processes[i].arrival_time == tick) {
                processes[i].state = READY;
                sorted_enqueue_priority(&ready_queue, &processes[i]);
            }
        }
        
        for (int i = 0; i < num_processes; i++) {
            Process* p = &processes[i];
            if (p->state == WAITING) {
                p->remaining_io_time--;
                if (p->remaining_io_time <= 0) {
                    p->state = READY;
                    p->current_burst_idx++;
                    p->remaining_burst_time = p->cpu_bursts[p->current_burst_idx];
                    sorted_enqueue_priority(&ready_queue, p);
                }
            }
        }
        
        // 선점 여부 검사 (매 tick마다 우선순위 비교)
        // 기능 명확화를 위해 도착한 프로세스를 검사하지 않고, 매 tick마다 ready queue를 검사
        if (running_process != NULL) {
            Process* front = peek_queue(&ready_queue);
            if (front != NULL && front->dynamic_priority < running_process->dynamic_priority) {
                running_process->state = READY;
                sorted_enqueue_priority(&ready_queue, running_process);
                running_process = NULL;
            }
        }
        
        if (running_process == NULL && !is_queue_empty(&ready_queue)) {
            running_process = dequeue(&ready_queue);
            running_process->state = RUNNING;
        }
        
        if (running_process != NULL) {
            add_gantt_event(tick, tick + 1, running_process->pid);
            running_process->remaining_burst_time--;
            
            if (running_process->remaining_burst_time <= 0) {
                if (running_process->current_burst_idx < running_process->num_cpu_bursts - 1) {
                    running_process->state = WAITING;
                    running_process->remaining_io_time = running_process->io_bursts[running_process->current_burst_idx];
                } else {
                    running_process->state = TERMINATED;
                    running_process->completion_time = tick + 1;
                    completed_processes++;
                }
                running_process = NULL;
            }
        } else {
            // IDLE 상태를 간트 차트에 기록 (-1을 IDLE로 간주)
            add_gantt_event(tick, tick + 1, -1);
        }
        tick++;
    }
}

// 6. Round Robin (RR, Quantum = 4) 스케줄러
void run_rr(Process processes[], int num_processes) {
    Queue ready_queue;
    init_queue(&ready_queue);
    
    int tick = 0;
    int completed_processes = 0;
    Process* running_process = NULL;
    
    num_gantt_events = 0; // 간트 차트 초기화
    
    while (completed_processes < num_processes) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].state == NEW && processes[i].arrival_time == tick) {
                processes[i].state = READY;
                enqueue(&ready_queue, &processes[i]);
            }
        }
        
        for (int i = 0; i < num_processes; i++) {
            Process* p = &processes[i];
            if (p->state == WAITING) {
                p->remaining_io_time--;
                if (p->remaining_io_time <= 0) {
                    p->state = READY;
                    p->current_burst_idx++;
                    p->remaining_burst_time = p->cpu_bursts[p->current_burst_idx];
                    enqueue(&ready_queue, p);
                }
            }
        }
        
        // Time Quantum 만료 시 선점
        if (running_process != NULL) {
            if (running_process->rr_quantum_used >= 4) {
                running_process->state = READY;
                running_process->rr_quantum_used = 0;
                enqueue(&ready_queue, running_process);
                running_process = NULL;
            }
        }
        
        if (running_process == NULL && !is_queue_empty(&ready_queue)) {
            running_process = dequeue(&ready_queue);
            running_process->state = RUNNING;
            running_process->rr_quantum_used = 0;
        }
        
        if (running_process != NULL) {
            add_gantt_event(tick, tick + 1, running_process->pid);
            running_process->remaining_burst_time--;
            running_process->rr_quantum_used++;
            
            if (running_process->remaining_burst_time <= 0) {
                if (running_process->current_burst_idx < running_process->num_cpu_bursts - 1) {
                    running_process->state = WAITING;
                    running_process->remaining_io_time = running_process->io_bursts[running_process->current_burst_idx];
                } else {
                    running_process->state = TERMINATED;
                    running_process->completion_time = tick + 1;
                    completed_processes++;
                }
                running_process = NULL;
            }
        } else {
            // IDLE 상태를 간트 차트에 기록 (-1을 IDLE로 간주)
            add_gantt_event(tick, tick + 1, -1);
        }
        tick++;
    }
}

// 7. Priority 스케줄러 + Aging 적용
void run_priority_aging(Process processes[], int num_processes) {
    Queue ready_queue;
    init_queue(&ready_queue);
    
    int tick = 0;
    int completed_processes = 0;
    Process* running_process = NULL;
    
    num_gantt_events = 0; // 간트 차트 초기화
    
    while (completed_processes < num_processes) {
        // Aging 처리 및 재정렬을 동시에 수행 (큐에서 뺐다가 정렬 삽입)
        // 새로 도착하거나 I/O에서 막 깨어난 프로세스가 억울하게 대기시간을 먹지 않도록 틱 사이클 제일 먼저 실행
        if (!is_queue_empty(&ready_queue)) {
            Queue temp_q;
            init_queue(&temp_q);
            while (!is_queue_empty(&ready_queue)) {
                Process* p = dequeue(&ready_queue);
                
                // 5틱 대기할 때마다 dynamic priority 증가 (숫자 감소)
                p->wait_ticks++;
                if (p->wait_ticks >= 5) {
                    if (p->dynamic_priority > 1) {
                        p->dynamic_priority--;
                    }
                    p->wait_ticks = 0;
                }
                
                // Aging이 반영된 상태로 임시 큐에 우선순위 정렬하여 삽입
                sorted_enqueue_priority(&temp_q, p);
            }
            ready_queue = temp_q;
        }

        for (int i = 0; i < num_processes; i++) {
            if (processes[i].state == NEW && processes[i].arrival_time == tick) {
                processes[i].state = READY;
                sorted_enqueue_priority(&ready_queue, &processes[i]);
            }
        }
        
        for (int i = 0; i < num_processes; i++) {
            Process* p = &processes[i];
            if (p->state == WAITING) {
                p->remaining_io_time--;
                if (p->remaining_io_time <= 0) {
                    p->state = READY;
                    p->current_burst_idx++;
                    p->remaining_burst_time = p->cpu_bursts[p->current_burst_idx];
                    sorted_enqueue_priority(&ready_queue, p);
                }
            }
        }
        // 선점 여부 검사 (매 tick마다 우선순위 비교)
        if (running_process != NULL) {
            Process* front = peek_queue(&ready_queue);
            if (front != NULL && front->dynamic_priority < running_process->dynamic_priority) {
                running_process->state = READY;
                sorted_enqueue_priority(&ready_queue, running_process);
                running_process = NULL;
            }
        }
        
        if (running_process == NULL && !is_queue_empty(&ready_queue)) {
            running_process = dequeue(&ready_queue);
            running_process->state = RUNNING;
            running_process->wait_ticks = 0; // 실행되면 대기 틱 초기화
        }
        
        if (running_process != NULL) {
            add_gantt_event(tick, tick + 1, running_process->pid);
            running_process->remaining_burst_time--;
            
            if (running_process->remaining_burst_time <= 0) {
                if (running_process->current_burst_idx < running_process->num_cpu_bursts - 1) {
                    running_process->state = WAITING;
                    running_process->remaining_io_time = running_process->io_bursts[running_process->current_burst_idx];
                } else {
                    running_process->state = TERMINATED;
                    running_process->completion_time = tick + 1;
                    completed_processes++;
                }
                running_process = NULL;
            }
        } else {
            // IDLE 상태를 간트 차트에 기록 (-1을 IDLE로 간주)
            add_gantt_event(tick, tick + 1, -1);
        }
        tick++;
    }
}


