all: run
run: AntiVirus.o
	gcc -m32 -g -Wall -o run AntiVirus.o
AntiVirus.o: AntiVirus.c
	gcc -g -Wall -m32 -c -o AntiVirus.o AntiVirus.c
.PHONY: clean
clean:
	rm -f *.o all
