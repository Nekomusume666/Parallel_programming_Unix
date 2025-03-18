#ifndef RACE_H
#define RACE_H

#include <sys/ipc.h>
#include <sys/msg.h>

#define NUM_CARS 5
#define TRACK_LENGTH 50
#define NUM_STAGES 3

struct message {
    long type; // 1 - финиш, 2 - обновление позиции
    int car_id;
    int stage;
    int position; // Используется только для типа 2
    long time_ms; // Используется только для типа 1
};

struct Score {
    int total_points;
    long total_time;
    int stage_scores[NUM_STAGES];
};

void car_process(int i, int msgid);
void referee_process(int msgid);

#endif  // RACE_H