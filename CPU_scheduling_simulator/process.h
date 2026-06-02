#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>

#define MAX_PROCESSES 7
#define MAX_BURSTS 10  // 최대 CPU/IO burst 짝의 수

typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

typedef struct Process {
    int pid;                     // 랜덤 PID
    int arrival_time;            // 도착 시간
    int priority;                // 우선순위 (작을수록 높음)
    
    // CPU 및 I/O Burst 관리 (CPU -> IO -> CPU -> IO -> CPU...)
    int num_cpu_bursts;          // CPU burst의 총 개수
    int cpu_bursts[MAX_BURSTS];  // 각 CPU burst 시간
    int io_bursts[MAX_BURSTS];   // 각 I/O burst 시간 (개수는 num_cpu_bursts - 1)
    
    int current_burst_idx;       // 현재 실행/대기 중인 burst의 인덱스
    int remaining_burst_time;    // 현재 CPU burst에서 남은 시간
    int remaining_io_time;       // 현재 I/O burst에서 남은 시간
    
    ProcessState state;
    
    // 평가 지표 산출용
    int first_run_time;          // 최초 실행 시간
    int completion_time;         // 종료 시간
    int total_waiting_time;      // 총 대기 시간
    int total_turnaround_time;   // 총 반환 시간
    
    // 추가 기능: Aging 기법용
    int dynamic_priority;        // Aging 적용을 위한 동적 우선순위
    int wait_ticks;              // 큐 대기 시간
    
    // 라운드로빈용
    int rr_quantum_used;         // RR에서 사용한 quantum 시간
    
    struct Process* next;        // Linked List Queue를 위한 포인터
} Process;

// 연결 리스트 기반 큐 구조체
typedef struct {
    Process* head;
    Process* tail;
} Queue;

// Queue 연산 함수
void init_queue(Queue* q);
bool is_queue_empty(Queue* q);
void enqueue(Queue* q, Process* p);
Process* dequeue(Queue* q);
void sorted_enqueue_sjf(Queue* q, Process* p);
void sorted_enqueue_priority(Queue* q, Process* p);

// 큐의 맨 앞(front) 프로세스 확인용 (제거하지 않음)
Process* peek_queue(Queue* q);

// 프로세스 데이터 생성 및 복사
void generate_processes(Process processes[]);
void deep_copy_processes(Process dest[], const Process src[], int count);
int get_total_burst_time(const Process* p);

#endif
