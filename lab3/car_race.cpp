#include "car_race.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <unistd.h>
#include <ncurses.h>
#include <vector>


void draw_race(const std::vector<int> &positions) {
   clear();
   for (int i = 1; i <= CarRace::NUM_CARS; ++i) {
       move(i, 0);
       printw("Car %d: ", i);
       for (int j = 0; j < positions[i]; ++j) {
           printw("#");
       }
       for (int j = positions[i]; j < CarRace::TRACK_LENGTH; ++j) {
           printw("-");
       }
   }
   refresh();
}


void draw_table(const std::vector<RaceResult> &results, const std::vector<int> &totalPoints, int stage) {
   move(CarRace::NUM_CARS + 2, 0);
   printw("-----------------------------------------------------------------");
   move(CarRace::NUM_CARS + 3, 0);
   printw("| Car | Stage 1 | Stage 2 | Stage 3 | Total Points | Total Time |");
   move(CarRace::NUM_CARS + 4, 0);
   printw("-----------------------------------------------------------------");


   for (int i = 1; i <= CarRace::NUM_CARS; ++i) {
       move(CarRace::NUM_CARS + 5 + i, 0);
       printw("|  %d  |", results[i].carId);
       for (int s = 0; s < CarRace::NUM_STAGES; ++s) {
           printw("   %.3f |", results[i].stageTimes[s]);
       }
       printw("   %*d       | %*f s |", 4, totalPoints[results[i].carId], 6, results[i].totalTime);
   }
   move(CarRace::NUM_CARS + 6 + CarRace::NUM_CARS, 0);
   printw("-----------------------------------------------------------------");
   refresh();
}


void runCarProcess(int rank) {
    RaceResult result;
    result.carId = rank;
    result.totalTime = 0;

    for (int stage = 0; stage < CarRace::NUM_STAGES; stage++) {
        int startSignal;
        MPI_Bcast(&startSignal, 1, MPI_INT, CarRace::REFEREE_RANK, MPI_COMM_WORLD);

        double startTime = MPI_Wtime();
        int raceDelay = CarRace::generateRaceDelay(1000, 5000);
        int stepDelay = raceDelay / CarRace::TRACK_LENGTH;

        for (int progress = 0; progress <= CarRace::TRACK_LENGTH; progress++) {
            MPI_Send(&progress, 1, MPI_INT, CarRace::REFEREE_RANK, CarRace::PROGRESS_TAG, MPI_COMM_WORLD);
            usleep(stepDelay * 1000);
        }

        double endTime = MPI_Wtime();
        result.stageTimes[stage] = endTime - startTime;
        result.totalTime += result.stageTimes[stage];

        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Gather(&result, sizeof(RaceResult), MPI_BYTE, nullptr, 0, MPI_BYTE,
                   CarRace::REFEREE_RANK, MPI_COMM_WORLD);
    }
}

void runRefereeProcess() {
    std::vector<RaceResult> results(CarRace::NUM_CARS);  
    std::vector<int> totalPoints(CarRace::NUM_CARS + 1, 0);
    std::vector<int> positions(CarRace::NUM_CARS + 1, 0);

    initscr();
    noecho();
    curs_set(0);

    for (int stage = 0; stage < CarRace::NUM_STAGES; stage++) {
        printw("\n=== Stage %d ===\n\n", stage + 1);
        refresh();

        std::fill(positions.begin(), positions.end(), 0);

        int startSignal = 1;
        MPI_Bcast(&startSignal, 1, MPI_INT, CarRace::REFEREE_RANK, MPI_COMM_WORLD);

        bool raceComplete = false;
        while (!raceComplete) {
            raceComplete = true;
            for (int car = 1; car <= CarRace::NUM_CARS; car++) {
                int flag;
                MPI_Status status;
                MPI_Iprobe(car, CarRace::PROGRESS_TAG, MPI_COMM_WORLD, &flag, &status);

                if (flag) {
                    int progress;
                    MPI_Recv(&progress, 1, MPI_INT, car, CarRace::PROGRESS_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    positions[car] = progress;
                }

                if (positions[car] < CarRace::TRACK_LENGTH) {
                    raceComplete = false;
                }
            }
            draw_race(positions);
            usleep(50000);
        }

        // ✅ Ждем, пока все машины финишируют
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Gather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL,  
                   results.data(), sizeof(RaceResult), MPI_BYTE,
                   CarRace::REFEREE_RANK, MPI_COMM_WORLD);

        std::sort(results.begin(), results.end() + 1,
                  [](const RaceResult &a, const RaceResult &b) {
                      return a.totalTime < b.totalTime;
                  });
        for (int i = 1; i <= CarRace::NUM_CARS; i++) {

            results[i].position = i;
            results[i].points = CarRace::POINTS[i];
            totalPoints[results[i].carId] += results[i].points;
        }

        draw_table(results, totalPoints, stage + 1);
        printw("\nPress Enter to continue...");
        refresh();
        getch();
    }

    clear();
    printw("\nFinal Standings:\n");
    printw("Car ID | Total Points\n");
    printw("--------|-------------\n");

    std::vector<std::pair<int, int>> standings;
    for (int i = 1; i <= CarRace::NUM_CARS; i++) {
        standings.push_back({i, totalPoints[i]});
    }

    std::sort(standings.begin(), standings.end(),
              [](const auto &a, const auto &b) {
                  return a.second > b.second;
              });

    for (const auto &[carId, points] : standings) {
        printw("%7d | %12d", carId, points);
        printw("\n");
    }

    refresh();
    getch();
    endwin();
}
