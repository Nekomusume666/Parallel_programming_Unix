#include "race.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <iostream>
#include <unistd.h>
#include <ncurses.h>
#include <fstream>


int main() {
    key_t key = ftok("racefile", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);

    pid_t referee_pid = fork();
    if (referee_pid == 0) {
        referee_process(msgid);
        exit(0);
    }

    pid_t car_pids[NUM_CARS];
    for (int i = 0; i < NUM_CARS; i++) {
        car_pids[i] = fork();
        if (car_pids[i] == 0) {
            car_process(i, msgid);
            exit(0);
        }
    }

    for (int i = 0; i < NUM_CARS; i++) {
        waitpid(car_pids[i], NULL, 0);
    }
    waitpid(referee_pid, NULL, 0);

    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}