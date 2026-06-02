#ifndef EVALUATION_H
#define EVALUATION_H

#include "process.h"

// 시뮬레이션 결과 평가 및 출력 (Waiting time, Turnaround time 계산)
void evaluate_and_print_results(Process processes[], int num_processes, const char* scheduler_name);

// 간트 차트 이벤트 관리를 위한 구조체
#define MAX_GANTT_EVENTS 1000

typedef struct {
    int start_time;
    int end_time;
    int pid;
} GanttEvent;

extern GanttEvent gantt_chart[MAX_GANTT_EVENTS];
extern int num_gantt_events;

// 간트 차트에 기록 추가
void add_gantt_event(int start_time, int end_time, int pid);

// 간트 차트 출력
void print_gantt_chart();

#endif
