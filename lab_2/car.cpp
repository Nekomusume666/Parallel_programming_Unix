#include "race.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <fstream>

// Процесс автомобиля
void car_process(int car_id, int msgid) {
    srand(time(nullptr) + car_id);
    
    for (int stage = 1; stage <= NUM_STAGES; ++stage) {
        // Ждем файл завершения предыдущего этапа (кроме первого)
        if (stage > 1) {
            std::string continue_file = "continue" + std::to_string(stage - 1) + ".txt";
            while (access(continue_file.c_str(), F_OK) == -1) {
                usleep(1000000);
            }
        }

        int position = 0;
        long start_time = time(nullptr);
        int speed = rand() % 4 + 1;

        while (position < TRACK_LENGTH) {
            usleep(100000);
            position += speed;
            if (position > TRACK_LENGTH) {
                position = TRACK_LENGTH;
            }

            // Отправка текущей позиции судье
            message msg;
            msg.type = 2;  // Тип для обновления позиции
            msg.car_id = car_id;
            msg.stage = stage;
            msg.position = position;
            msgsnd(msgid, &msg, sizeof(message) - sizeof(long), 0);
        }

        long end_time = time(nullptr);
        long time_ms = (end_time - start_time);

        // Отправка сообщения о завершении этапа
        message msg;
        msg.type = 1;  // Тип для завершения этапа
        msg.car_id = car_id;
        msg.stage = stage;
        msg.time_ms = time_ms;
        msgsnd(msgid, &msg, sizeof(message) - sizeof(long), 0);
    }

    exit(0);
}
