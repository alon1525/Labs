#include <stdio.h>

int digit_cnt(char* arg);

int main(int argc, char* argv[]) {
    if(argc > 1)
        printf("%d\n", digit_cnt(argv[1]));
}

int digit_cnt(char* arg){
    int counter = 0;

    for (int i = 0; arg[i] != 0; ++i) {
        if(arg[i] >= '0' && arg[i] <= '9')
            counter++;
    }

    return counter;
}