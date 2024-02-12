all: Lab2
Lab2: LineParser.o
	gcc -m32 -g -Wall -o LineParser LineParser.o
LineParser.o: LineParser.c
	gcc -g -Wall -m32 -c -o LineParser.o LineParser.c
.PHONY: clean
clean:
	rm -f *.o Lab2

		