all: task2
task2: base.o
	gcc -m32 -g -Wall -o task2 base.o
base.o: base.c
	gcc -g -Wall -m32 -c -o base.o base.c
.PHONY: clean
clean:
	rm -f *.o task2