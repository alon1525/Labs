
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/types.h>
#include "LineParser.h"

#define MAX_INPUT_SIZE 2048

void displayPrompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s$ ", cwd);
    } else {
        perror("getcwd");
        exit(0);
    }
}

void execute(cmdLine *pCmdLine) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(0);
    } else if (pid == 0) {
        if (execv(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
            perror("execv");
            exit(0);
        }
    } else {
        waitpid(pid, NULL, 0);
    }
}

int main() {
    while (1) {
        displayPrompt();

        char userInput[MAX_INPUT_SIZE];
        if (fgets(userInput, sizeof(userInput), stdin) == NULL) {
            perror("fgets");
            exit(0);
        }

        if (strcmp(userInput, "quit\n") == 0) {
            printf("Exiting shell.\n");
            break;
        }

        cmdLine *commandLine = parseCmdLines(userInput);
        execute(commandLine);

        // Example of using replaceCmdArg to change the second argument to "newArgument"
        int argToReplace = 1; // Assuming you want to replace the second argument (index 1)
        const char *newArgument = "newArgument";
        if (!replaceCmdArg(commandLine, argToReplace, newArgument)) {
            fprintf(stderr, "Invalid argument index to replace.\n");
        }

        freeCmdLines(commandLine);
    }

    return 0;
}
