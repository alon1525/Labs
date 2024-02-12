all: run
run: task2b.o
	gcc -m32 -g -Wall -o run task2b.o
task2b.o: task2b.c
	gcc -g -Wall -m32 -c -o task2b.o task2b.c
.PHONY: clean
clean:
	rm -f *.o all
