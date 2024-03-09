#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define SYS_SEEK 19
#define SEEK_SET 0
#define BUF_SIZE 8192
#define SYS_EXIT 60
#define SYS_READ 0
#define SYS_CLOSE 3
#define O_RDONLY 00
#define O_RDWR 2
#include "dirent.h"

#ifndef UTIL_H
#define UTIL_H

int strncmp(const char *str1, const char *str2, unsigned int n);
unsigned int strlen(const char *str);

#endif

extern int system_call();
extern void infector(char *path);
extern void infection();

void printFileContent(const char *filename) {
    int file = system_call(SYS_OPEN, filename,O_RDONLY);

    if (file < 0) {
        system_call(0x55);
    }

    char buffer[BUF_SIZE] = {0};
    int bytes_read = system_call(SYS_READ, file, buffer, BUF_SIZE);
    
    if (bytes_read < 0) {
        system_call(SYS_CLOSE, file);
        system_call(0x55);
    }

    system_call(SYS_WRITE, STDOUT, buffer, BUF_SIZE);
    system_call(SYS_WRITE, STDOUT, "\n", 1);

    system_call(SYS_CLOSE, file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        system_call(0x55);
    }
    char *msg = "VIRUS ATTACHED ";
    int i = 1; 
    while (i < argc) {
        char *filename = argv[i];

        if (strncmp(argv[i], "-a", 2) != 0) {
            printFileContent(filename);
        }
        else {
            
            char *file = filename + 2;
            infection();
            infector(file);
            system_call(SYS_WRITE, STDOUT, msg, strlen(msg));
            system_call(SYS_WRITE, STDOUT, file, strlen(file));
            system_call(SYS_WRITE, STDOUT, "\n", 1);
            
        }

        i++;
    }

    return 0;
}

