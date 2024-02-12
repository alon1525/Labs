#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct virus
{
    unsigned short SigSize;
    char virusName[16];
    unsigned char *sig;
} virus;

typedef struct link
{
    struct link *nextVirus;
    virus *vir;
} link;

struct fun_desc
{
    char *name;
    link *(*fun)(link *, char *arg);
};

virus *readVirus(FILE *file, bool isVIRL);
void neutralize_virus(char *fileName, int signatureOffset);
link *printSignatures(link *list, char *arg);
link *detect_virus(link *, char *arg);
link *fixFile();
link *quit(link *list, char *arg);
link *load_signature();
void realDetectVirus(char *buffer, unsigned int size, link *virus_list);
void printViruses(virus *virus, FILE *output);
void list_print(link *virus_list, FILE *file);
void list_free(link *virus_list);
link *list_append(link *virus_list, virus *data);
void fakeDetectVirus(char *buffer, unsigned int size, link *virus_list, char *arg);


int main(int argc, char **argv)
{
    link *list = malloc(sizeof(link));
    list->vir = NULL;
    list->nextVirus = NULL;
    struct fun_desc menu[] = {{"Load signatures", load_signature},
                              {"Print signatures", printSignatures},
                              {"Detect viruses", detect_virus},
                              {"Fix file", fixFile},
                              {"Quit", quit},
                              {NULL, NULL}};
    int size = (sizeof menu - 1) / sizeof menu[0];
    int menuSize = (sizeof menu - 1) / sizeof menu[0];
    while (1)
    {
        printf("%s", "Choose a function: \n");
        for (int i = 0; i < size; ++i)
        {
            printf("%d) ", i + 1);
            printf("%s\n", menu[i].name);
        }
        printf("%s", "Option : ");
        char input[1000];
        if (fgets(input, 1000, stdin) == NULL)
        {
            list_free(list);
            break;
        }
        int numberPicked = atoi(input) - 1;
        if (numberPicked >= 0 && numberPicked < menuSize && input[0] != '\0' && input[0] != '\n')
        {
            printf("Within bounds\n");
            if (numberPicked!=4)
            {
                list = menu[numberPicked].fun(list, argv[1]);
            }
            else
            {
                if (list->vir != NULL)
                {        
                    list_free(list);
                }
                else
                {
                    free(list);
                }
                break;
            }
            
            
            printf("%s\n\n", "DONE.");
        }
        else
        {
            printf("Not within bounds\n");
        }
    }
}
link *load_signature(link *list, char *arg)
{
    FILE *file;
    unsigned char buffer[4];
    printf("Enter file name: \n");
    char fileString[100];
    fgets(fileString, 100, stdin);
    char *p = fileString;
    while (*p != '\n')
    {
        p++;
    }
    *p = '\0';
    file = fopen(fileString, "r");
    if (file == NULL)
    {
        fprintf(stderr, "could not open file");
        exit(0);
    }
    fread(buffer, sizeof(*buffer), 4, file);
    bool isVIRL = false;
    if (memcmp(buffer, "VIRL", 4) == 0)
    {
        isVIRL = true;
    }
    struct virus *v = readVirus(file, isVIRL);
    while (v != NULL)
    {
        list = list_append(list, v);
        v = readVirus(file, isVIRL);
    }
    fclose(file);
    return list;
}

link *printSignatures(link *list, char *arg)
{
    list_print(list, stdout);
    return list;
}

link *quit(link *list, char *arg)
{
   return list;
}

link *detect_virus(link *list, char *arg)
{
    FILE *file = fopen(arg, "r");
    if (file == NULL)
    {
        fprintf(stderr, "could not open file");
        exit(0);
    }
    fseek(file, 0, SEEK_END);
    unsigned int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(10000);
    fread(buffer, 10000, 1, file);
    unsigned int size;
    if ((fileSize) >= 10000)
    {
        size = 10000;
    }
    else
    {
        size = fileSize;
    }
    realDetectVirus(buffer, size, list);
    free(buffer);
    fclose(file);
    return list;
}

void realDetectVirus(char *buffer, unsigned int size, link *virus_list)
{
    while (virus_list != NULL)
    {
        if (virus_list->vir == NULL)
        {
            break;
        }
        int maxIndex = size - virus_list->vir->SigSize;
        for (int i = 0; i <= maxIndex; ++i)
        {
            if (size - i >= virus_list->vir->SigSize)
            {
                if (memcmp(buffer + i, virus_list->vir->sig, virus_list->vir->SigSize) == 0)
                {
                    printf("\nStarting byte location: %d \n", i);
                    printf("Virus name: %s \n", virus_list->vir->virusName);
                    printf("Signature size: %d \n\n", virus_list->vir->SigSize);
                    break;
                }
            }
        }
        virus_list = virus_list->nextVirus;
    }
}

void fakeDetectVirus(char *buffer, unsigned int size, link *virus_list, char *arg)
{
    while (virus_list != NULL)
    {
        if (virus_list->vir == NULL)
        {
            break;
        }
        int maxIndex = size - virus_list->vir->SigSize;
        for (int i = 0; i <= maxIndex; ++i)
        {
            if (size - i >= virus_list->vir->SigSize)
            {
                if (memcmp(buffer + i, virus_list->vir->sig, virus_list->vir->SigSize) == 0)
                {
                    neutralize_virus(arg, i);
                    break;
                }
            }
        }
        virus_list = virus_list->nextVirus;
    }
}

link *fixFile(link *list, char *arg)
{
    FILE *file = fopen(arg, "r");
    if (file == NULL)
    {
        fprintf(stderr, "could not open file");
        exit(0);
    }
    fseek(file, 0, SEEK_END);
    unsigned int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(10000);
    fread(buffer, 10000, 1, file);
    unsigned int size;
    if ((fileSize) >= 10000)
    {
        size = 10000;
    }
    else
    {
        size = fileSize;
    }
    fakeDetectVirus(buffer, size, list, arg);
    free(buffer);
    fclose(file);
    return list;
}

void neutralize_virus(char *fileName, int signatureOffset)
{
    char ret = 0xC3;
    FILE *infect = fopen(fileName, "r+");

    if (infect == 0)
    {
        fprintf(stderr, "FAILED TO OPEN FILE\n");
        exit(0);
    }
    if (fseek(infect, signatureOffset, SEEK_SET) == 0)
        fwrite(&ret, 1, 1, infect);
    fclose(infect);
}

virus *readVirus(FILE *file, bool isVIRL)
{
    if (file == NULL)
    {
        fprintf(stderr, "%s", "could not open file");
        exit(0);
    }
    unsigned short sigSize;
    unsigned char virusName[16];
    unsigned char buffer[16];

    unsigned int r1 = fread(buffer, sizeof(*buffer), 2, file);
    if (r1 == 0)
    {
        return NULL;
    }
    if (isVIRL)
    {
        sigSize = buffer[0] + 256 * buffer[1];
    }
    else
    {
        sigSize = buffer[1] + 256 * buffer[0];
    }
    fread(buffer, sizeof(*buffer), 16, file);
    for (int i = 0; i < 16; ++i)
    {
        virusName[i] = buffer[i];
    }
    unsigned char *sig = (unsigned char *)malloc(sigSize);
    for (int i = 0; i < sigSize; ++i)
    {
        fread(buffer, sizeof(*buffer), 1, file);
        sig[i] = buffer[0];
    }
    struct virus *v = (virus *)malloc(sizeof(virus));
    for (int i = 0; i < 16; ++i)
    {
        v->virusName[i] = virusName[i];
    }
    v->SigSize = sigSize;
    v->sig = sig;
    return v;
}

void printVirus(virus *virus, FILE *output)
{
    fprintf(output, "%s", "Virus name is: ");
    int i = 0;
    while (i < 16 && virus->virusName[i] != '\0')
    {
        fprintf(output, "%c", virus->virusName[i]);
        i++;
    }
    fprintf(output, "\n%s", "Virus signature length: ");
    fprintf(output, "%d\n", virus->SigSize);
    fprintf(output, "%s", "Virus signature: ");
    for (int i = 0; i < virus->SigSize; ++i)
    {
        fprintf(output, "%02X ", virus->sig[i]);
    }
    fprintf(output, "\n\n");
}

void list_print(link *virus_list, FILE *file)
{
    fprintf(file, "\n");
    while (virus_list != NULL)
    {
        printVirus(virus_list->vir, file);
        virus_list = virus_list->nextVirus;
    }
}

link *list_append(link *virus_list, virus *data)
{

    if (virus_list->vir == NULL)
    {
        virus_list->vir = data;
    }
    else if (virus_list->nextVirus == NULL)
    {
        struct link *vl = (link *)malloc(sizeof(link));
        vl->vir = data;
        vl->nextVirus = NULL;
        virus_list->nextVirus = vl;
    }
    else
    {
        list_append(virus_list->nextVirus, data);
    }
    return virus_list;
}

void list_free(link *virus_list)
{
    while(virus_list != NULL){
        free(virus_list->vir->sig);
        free(virus_list->vir);
        link* temp = virus_list;
        virus_list = virus_list->nextVirus;
        free(temp);
    }
}
