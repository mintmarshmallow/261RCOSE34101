#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_queue(Queue* q) {
    q->head = NULL;
    q->tail = NULL;
}

bool is_queue_empty(Queue* q) {
    return q->head == NULL;
}

void enqueue(Queue* q, Process* p) {
    p->next = NULL;
    if (is_queue_empty(q)) {
        q->head = p;
        q->tail = p;
    } else {
        q->tail->next = p;
        q->tail = p;
    }
}

Process* dequeue(Queue* q) {
    if (is_queue_empty(q)) return NULL;
    Process* p = q->head;
    q->head = q->head->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    p->next = NULL;
    return p;
}

Process* peek_queue(Queue* q) {
    return q->head;
}

void sorted_enqueue_sjf(Queue* q, Process* p) {
    p->next = NULL;
    if (is_queue_empty(q)) {
        q->head = p;
        q->tail = p;
        return;
    }

    int val_p = p->remaining_burst_time;
    int val_head = q->head->remaining_burst_time;

    if (val_p < val_head || (val_p == val_head && p->arrival_time < q->head->arrival_time)) {
        p->next = q->head;
        q->head = p;
        return;
    }

    Process* curr = q->head;
    while (curr->next != NULL) {
        int val_next = curr->next->remaining_burst_time;
        if (val_p < val_next || (val_p == val_next && p->arrival_time < curr->next->arrival_time)) {
            break;
        }
        curr = curr->next;
    }

    p->next = curr->next; //curr와 curr->next 사이에 p를 삽입
    curr->next = p;

    if (p->next == NULL) {
        q->tail = p;
    }
}

void sorted_enqueue_priority(Queue* q, Process* p) {
    p->next = NULL;
    if (is_queue_empty(q)) {
        q->head = p;
        q->tail = p;
        return;
    }

    int val_p = p->dynamic_priority;
    int val_head = q->head->dynamic_priority;

    if (val_p < val_head || (val_p == val_head && p->arrival_time < q->head->arrival_time)) {
        p->next = q->head;
        q->head = p;
        return;
    }

    Process* curr = q->head;
    while (curr->next != NULL) {
        int val_next = curr->next->dynamic_priority;
        if (val_p < val_next || (val_p == val_next && p->arrival_time < curr->next->arrival_time)) {
            break;
        }
        curr = curr->next;
    }

    p->next = curr->next;
    curr->next = p;

    if (p->next == NULL) {
        q->tail = p;
    }
}

void generate_processes(Process processes[]) {
    // 무작위로 생성. 7개의 프로세스를 고정하여 만듦
    // 난수 생성은 rand()를 이용
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].pid = (rand() % 99) + 1; // 1 ~ 99 랜덤 PID
        processes[i].arrival_time = rand() % 20;  // 0 ~ 19 사이에 도착
        processes[i].priority = (rand() % 10) + 1; // 1 ~ 10 우선순위 (작을수록 높음)
        
        // I/O와 CPU burst 번갈아 발생
        // 최소 1쌍의 CPU-IO-CPU, 최대 5개의 CPU burst
        processes[i].num_cpu_bursts = (rand() % 5) + 1;
        
        for (int j = 0; j < processes[i].num_cpu_bursts; j++) {
            processes[i].cpu_bursts[j] = (rand() % 10) + 2; // 2 ~ 11 사이의 CPU burst 시간
            if (j < processes[i].num_cpu_bursts - 1) {
                processes[i].io_bursts[j] = (rand() % 8) + 1;   // 1 ~ 8 사이의 IO burst 시간
            }
        }
        
        processes[i].current_burst_idx = 0;
        processes[i].remaining_burst_time = processes[i].cpu_bursts[0];
        processes[i].remaining_io_time = 0;
        
        processes[i].state = NEW;
        processes[i].completion_time = 0;
        processes[i].total_waiting_time = 0;
        processes[i].total_turnaround_time = 0;
        processes[i].first_run_time = -1;
        
        processes[i].dynamic_priority = processes[i].priority;
        processes[i].rr_quantum_used = 0;
        processes[i].wait_ticks = 0;
        processes[i].next = NULL;
    }
}

void deep_copy_processes(Process dest[], const Process src[], int count) {
    for (int i = 0; i < count; i++) {
        dest[i] = src[i];
        dest[i].next = NULL;
    }
}

int get_total_burst_time(const Process* p) {
    int total = 0;
    for (int i = 0; i < p->num_cpu_bursts; i++) {
        total += p->cpu_bursts[i];
    }
    return total;
}
