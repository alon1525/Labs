all: task1
task1: encoder.o
	gcc -m32 -g -Wall -o task1 encoder.o
encoder.o: encoder.c
	gcc -g -Wall -m32 -c -o encoder.o encoder.c
.PHONY: clean
clean:
	rm -f *.o task1
