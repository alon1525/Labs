all:hexeditplus task4

hexeditplus: hexeditplus.c
	gcc -g -m32 -o hexeditplus hexeditplus.c 
task4: task4.c
	gcc -g -m32 -fno-pie -fno-stack-protector -o task4 task4.c 
.PHONY: clean
clean:
	rm -f hexeditplus task4
