#include <sys/mman.h>
#include <semaphore.h>
#include <stdio.h>

#define NO_SEM_SHM_NAME "/ipc_no_semaphore_demo"
#define SEM_SHM_NAME "/ipc_semaphore_demo"
#define SEM_NAME "/ipc_semaphore_lock"

int main(){
    if (shm_unlink(NO_SEM_SHM_NAME) == -1) {
        printf("shm_unlink %s error or already removed\n", NO_SEM_SHM_NAME);
    } else {
        printf("destroy shared memory : %s\n", NO_SEM_SHM_NAME);
    }

    if (shm_unlink(SEM_SHM_NAME) == -1) {
        printf("shm_unlink %s error or already removed\n", SEM_SHM_NAME);
    } else {
        printf("destroy shared memory : %s\n", SEM_SHM_NAME);
    }

    if (sem_unlink(SEM_NAME) == -1) {
        printf("sem_unlink %s error or already removed\n", SEM_NAME);
    } else {
        printf("destroy semaphore : %s\n", SEM_NAME);
    }

    return 0;
}
