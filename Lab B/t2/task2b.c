#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

typedef struct link {
    struct link *nextVirus;
    virus *vir;
} link;

struct fun_desc{
    char *name;
    link* (*fun)(link*, char* arg);
};

link* loadSignatures(link* list, char* arg);
link* printSignatures(link* list, char* arg);
link* detectViruses(link* list, char* arg);
link* fixFile(link* list, char* arg);
link* quit(link *list, char* arg);


virus* readVirus(FILE* file);
void printVirus(virus* virus, FILE* output);
void list_print(link *virus_list, FILE* file);
link* list_append(link* virus_list, virus* data);
void list_free(link *virus_list);
void detect_virus(char *buffer, unsigned int size, link *virus_list);
void kill_virus(char* fileName, int signatureOffset, int signatureSize);


int main(int argc, char **argv){
    struct link* list = malloc(sizeof (link));
    list->vir = NULL;
    list->nextVirus = NULL;
    struct fun_desc menu[] = {{"Load signatures",  loadSignatures},
                              {"Print signatures",    printSignatures},
                              {"Detect viruses",    detectViruses},
                              {"Fix file",    fixFile},
                              {"Quit",    quit},
                              {NULL, NULL}};
    int size = (sizeof menu - 1) / sizeof menu[0];
    while (1) {
        printf("%s", "Please choose a function: \n");
        for (int i = 0; i < size; ++i) {
            printf("%d) ", i + 1);
            printf("%s\n", menu[i].name);
        }
        printf("%s", "Option : ");
        char input[1000];
        fgets(input, 1000, stdin);
        int num = atoi(input);
        if (num > size || num < 1) {
            printf("%s\n", "Not within bounds");
            exit(0);
        }
        printf("%s\n", "Within bounds");
        list = menu[num - 1].fun(list, argv[1]);
        printf("%s\n\n", "DONE.");
    }
}

link* loadSignatures(link* list, char* arg){
    FILE* file;
    unsigned char buffer[4];
    printf("Enter file name: \n");
    char fileString[100];
    fgets(fileString, 100, stdin);
    char* p = fileString;
    while(*p != '\n'){
        p++;
    }
    *p = '\0';
    file = fopen(fileString, "r");
    if(file == NULL){
        fprintf(stderr, "could not open file");
        exit(0);
    }
    fread(buffer, sizeof(*buffer), 4, file);
    struct virus* v = readVirus(file);
    while(v != NULL) {
        list = list_append(list, v);
        v = readVirus(file);
    }
    fclose(file);
    return list;
}

link* printSignatures(link* list, char* arg){
    list_print(list, stdout);
    return list;
}

link* detectViruses(link* list, char* arg){
    FILE* file = fopen(arg, "r");
    if(file == NULL){
        fprintf(stderr, "could not open file");
        exit(0);
    }
    char* buffer = malloc(10000);
    unsigned int size = fread(buffer, sizeof(*buffer), 10000, file);
    detect_virus(buffer, size, list);
    free(buffer);
    fclose(file);
    return list;
}

link* fixFile(link* list, char* arg){
    printf("Enter starting byte location: \n");
    char index[100];
    fgets(index, 100, stdin);
    int signatureOffset = atoi(index);
    printf("Enter signature size: \n");
    char size[100];
    fgets(size, 100, stdin);
    int signatureSize = atoi(size);
    kill_virus(arg, signatureOffset, signatureSize);
    return list;
}

link* quit(link *virus_list, char* arg){
    list_free(virus_list);
    exit(0);
}

virus* readVirus(FILE* file){
    if(file == NULL){
        fprintf(stderr, "%s", "could not open file");
        exit(0);
    }
    unsigned short sigSize;
    unsigned char virusName[16];

    unsigned char buffer[16];
    unsigned int r = fread(buffer, sizeof(*buffer), 2, file);
    if(r == 0){
        return NULL;
    }
    if(buffer[1] == '\0'){
        sigSize = buffer[0];
    }
    else{
        sigSize = buffer[0] + 256 * buffer[1];
    }
    fread(buffer, sizeof(*buffer), 16, file);
    for (int i = 0; i < 16; ++i) {
        virusName[i] = buffer[i];
    }
    unsigned char* sig = (unsigned char*) malloc(sigSize);
    for (int i = 0; i < sigSize; ++i) {
        fread(buffer, sizeof(*buffer), 1, file);
        sig[i] = buffer[0];
    }
    struct virus* v = (virus*) malloc(sizeof(virus));
    for (int i = 0; i < 16; ++i) {
        v->virusName[i] = virusName[i];
    }
    v->SigSize = sigSize;
    v->sig = sig;
    return v;
}


void printVirus(virus* virus, FILE* output){
    fprintf(output, "%s", "Virus name: ");
    for (int i = 0; i < 16; ++i) {
        if(virus->virusName[i] == '\0')
            break;
        fprintf(output, "%c", virus->virusName[i]);
    }
    fprintf(output, "\n");
    fprintf(output, "%s", "Virus signature length: ");
    fprintf(output, "%d\n", virus->SigSize);
    fprintf(output, "%s", "Virus signature: ");
    for (int i = 0; i < virus->SigSize; ++i) {
        fprintf(output, "%x ", virus->sig[i]);
    }
    fprintf(output, "\n");
    fprintf(output, "\n");
}

void list_print(link *virus_list, FILE* file){
    fprintf(file, "\n");
    while(virus_list != NULL){
        printVirus(virus_list->vir, file);
        virus_list = virus_list->nextVirus;
    }
}

link* list_append(link* virus_list, virus* data){
    if(virus_list->vir == NULL){
        virus_list->vir = data;
    }
    else if(virus_list->nextVirus == NULL){
        struct link *vl = (link*) malloc(sizeof (link));
        vl->vir = data;
        virus_list->nextVirus = vl;
        vl->nextVirus = NULL;
    }
    else {
        list_append(virus_list->nextVirus, data);
    }
    return virus_list;
}

void list_free(link *virus_list){
    while(virus_list != NULL){
        free(virus_list->vir->sig);
        free(virus_list->vir);
        link* temp = virus_list;
        virus_list = virus_list->nextVirus;
        free(temp);
    }
}

void detect_virus(char *buffer, unsigned int size, link *virus_list){
    while(virus_list != NULL){
        if(virus_list->vir == NULL){
            break;
        }
        int len = virus_list->vir->SigSize;
        int maxIndex = size - len;
        for (int i = 0; i < maxIndex; ++i) {
            if(memcmp(&buffer[i], virus_list->vir->sig, len) == 0){
                printf("\nStarting byte location: %d \n", i);
                printf("Virus name: %s \n", virus_list->vir->virusName);
                printf("Signature size: %d \n\n", len);
                break;
            }
        }
        virus_list = virus_list->nextVirus;
    }
}

void kill_virus(char* fileName, int signatureOffset, int signatureSize){
    FILE* file = fopen(fileName, "r+");
    if(file == NULL){
        fprintf(stderr, "could not open file");
        exit(0);
    }
    fseek(file, signatureOffset, SEEK_SET);
    char buffer[signatureSize];
    for (int i = 0; i < signatureSize; ++i) {
        buffer[i] = 0x90;
    }
    fwrite(buffer, sizeof(*buffer), signatureSize, file);
    fclose(file);
}
