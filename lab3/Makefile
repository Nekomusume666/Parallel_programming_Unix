CXX = mpic++
CXXFLAGS = -Wall -std=c++17
OBJECTS = matrix_mult.o car_race.o main.o
LDFLAGS = -lncurses


all: matrix_mult 1

matrix_mult: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o matrix_mult

1: 1.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) 1.cpp -o 1

matrix_mult.o: matrix_mult.cpp matrix_mult.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c matrix_mult.cpp

car_race.o: car_race.cpp car_race.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c car_race.cpp

main.o: main.cpp matrix_mult.h car_race.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c main.cpp

clean:
	rm -f *.o matrix_mult 1

run4: matrix_mult
	mpiexec -np 4 ./matrix_mult matrix4

run20: matrix_mult
	mpiexec -np 20 ./matrix_mult matrix20

run_race: matrix_mult
	mpiexec -np 6 ./matrix_mult car_race

run1: 1
	mpiexec -np 2 ./1

.PHONY: all clean run4 run20 run_race run1