EXE = completion_test
CC = gcc
FLAGS = -g -Wall

all: $(EXE)

completion_test: completion_test.o completion.o printing.o reading.o
	$(CC) $(FLAGS) $^ -o $@ -fopenmp

completion_test.o: completion_test.c odd.h
	$(CC) -c $(FLAGS) $< -o $@ -fopenmp

completion.o: completion.c odd.h
	$(CC) -c $(FLAGS) $< -o $@ -fopenmp

printing.o: printing.c odd.h
	$(CC) -c $(FLAGS) $< -o $@ -fopenmp

reading.o: reading.c odd.h
	$(CC) -c $(FLAGS) $< -o $@ -fopenmp

clean:
	rm -f *.o *.exe
	rm -f completion_test
