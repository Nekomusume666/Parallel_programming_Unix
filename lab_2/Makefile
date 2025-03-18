CC = g++
CFLAGS = -Wall -g
OBJ = main.o car.o referee.o
LIBS = -lncurses  # Линковка с ncurses

# Цель по умолчанию
all: race

# Линковка объекта в исполнимый файл
race: $(OBJ)
	$(CC) $(CFLAGS) -o race $(OBJ) $(LIBS)

# Правила для компиляции исходных файлов в объектные
main.o: main.cpp race.h
	$(CC) $(CFLAGS) -c main.cpp

car.o: car.cpp race.h
	$(CC) $(CFLAGS) -c car.cpp

referee.o: referee.cpp race.h
	$(CC) $(CFLAGS) -c referee.cpp

# Очистка
clean:
	rm -f *.o race
