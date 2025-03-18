#include "race.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <iostream>
#include <unistd.h>
#include <ncurses.h>
#include <vector>
#include <algorithm> 
#include <fstream>


void draw_race(const std::vector<int> &positions) {
    clear();
    for (int i = 0; i < NUM_CARS; ++i) {
        move(i, 0);
        printw("Car %d: ", i);
        for (int j = 0; j < positions[i]; ++j) {
            printw("#");
        }
        for (int j = positions[i]; j < TRACK_LENGTH; ++j) {
            printw("-");
        }
    }
    refresh();
}

void draw_table(const Score scores[NUM_CARS], const int stage) {
    move(NUM_CARS + 2, 0);
    printw("-----------------------------------------------------------------");
    move(NUM_CARS + 3, 0);
    printw("| Car | Stage 1 | Stage 2 | Stage 3 | Total Points | Total Time |");
    move(NUM_CARS + 4, 0);
    printw("-----------------------------------------------------------------");

    for (int i = 0; i < NUM_CARS; ++i) {
        move(NUM_CARS + 5 + i, 0);
        printw("|  %d  |", i);
        for (int s = 1; s <= NUM_STAGES; ++s) {
            if (s <= stage)
                printw("   %d    |", scores[i].stage_scores[s - 1]);
            else
                printw("         |");
        }
        printw("   %*d       |   %*ld s |", 4, scores[i].total_points, 6, scores[i].total_time);
    }
    move(NUM_CARS + 6 + NUM_CARS, 0);
    printw("-----------------------------------------------------------------");
    refresh();
}

bool compare_times(const std::pair<int, long>& a, const std::pair<int, long>& b) {
    return a.second < b.second;
}

bool compare_final_scores(const std::pair<int, Score>& a, const std::pair<int, Score>& b) {
    if (a.second.total_points != b.second.total_points)
        return a.second.total_points > b.second.total_points;
    return a.second.total_time < b.second.total_time;
}

void referee_process(int msgid) {
    Score scores[NUM_CARS] = {};
    std::vector<int> positions(NUM_CARS, 0);

    initscr();
    noecho();
    curs_set(0);

    for (int stage = 1; stage <= NUM_STAGES; ++stage) {
        std::string continue_file = "continue" + std::to_string(stage) + ".txt";
        printw("\nStage %d started!\n", stage);
        refresh();

        int finished_cars = 0;
        std::vector<std::pair<int, long>> stage_results;
        std::vector<bool> car_finished(NUM_CARS, false);

        while (finished_cars < NUM_CARS) {
            message msg;
            while (msgrcv(msgid, &msg, sizeof(message) - sizeof(long), 0, IPC_NOWAIT) > 0) {
                if (msg.stage == stage) {
                    if (msg.type == 2) {
                        positions[msg.car_id] = msg.position;
                    } else if (msg.type == 1 && !car_finished[msg.car_id]) {
                        car_finished[msg.car_id] = true;
                        stage_results.emplace_back(msg.car_id, msg.time_ms);
                        finished_cars++;
                    }
                }
            }
            draw_race(positions);
            draw_table(scores, stage - 1);
            refresh();
            usleep(50000);
        }

        std::sort(stage_results.begin(), stage_results.end(), compare_times);
        for (size_t i = 0; i < stage_results.size(); ++i) {
            int car_id = stage_results[i].first;
            long time_ms = stage_results[i].second;
            int points = (NUM_CARS - i) * 10;

            scores[car_id].total_points += points;
            scores[car_id].total_time += time_ms;
            scores[car_id].stage_scores[stage - 1] = points;
        }

        draw_race(positions);
        draw_table(scores, stage);
        refresh();

        if (stage != 3) {
            printw("\nPress Enter to start Stage %d...", stage + 1);
            refresh();
            getch();
        }

        std::ofstream file(continue_file);
        file.close();

        usleep(1000000);
    }

    for (int stage = 1; stage <= NUM_STAGES; ++stage) {
        std::string prev_file = "continue" + std::to_string(stage) + ".txt";
        remove(prev_file.c_str());
    }

    std::vector<std::pair<int, Score>> final_scores;
    for (int i = 0; i < NUM_CARS; ++i) {
        final_scores.emplace_back(i, scores[i]);
    }
    std::sort(final_scores.begin(), final_scores.end(), compare_final_scores);

    printw("\nFinal Scores:\n");
    for (size_t i = 0; i < final_scores.size(); ++i) {
        printw("Place %d - Car %d - Total Points: %d - Total Time: %ld s\n",
               (int)(i + 1), final_scores[i].first, final_scores[i].second.total_points, final_scores[i].second.total_time);
    }
    printw("\nWinner: Car %d with %d points in %ld s!\n",
           final_scores[0].first, final_scores[0].second.total_points, final_scores[0].second.total_time);

    refresh();
    getch();
    endwin();
    exit(0);
}
