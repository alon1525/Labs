all: task1
task1: adresses.o
	gcc -m32 -g -Wall -o task1 adresses.o
adresses.o: adresses.c
	gcc -g -Wall -m32 -c -o adresses.o adresses.c
.PHONY: clean
clean:
	rm -f *.o task1