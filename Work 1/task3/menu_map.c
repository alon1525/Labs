#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char my_get(char c);
char cprt(char c);
char encrypt(char c);
char decrypt(char c);
char xprt(char c);

struct fun_desc
{
    char *name;
    char (*f)(char);
};

char *map(char *array, int array_length, char (*f)(char))
{
    char *mapped_array = (char *)(malloc(array_length * sizeof(char)));
    for (int i = 0; i < array_length; i++)
    {
        mapped_array[i] = (*f)(array[i]);
    }
    return mapped_array;
}

char my_get(char c)
{
    return fgetc(stdin);
}

char cprt(char c)
{
    if (c >= 0x20 && c <= 0x7E)
    {
        printf("%c\n", c);
    }
    else
    {
        printf("%c\n", '.');
    }
    return c;
}

char encrypt(char c)
{
    if (c >= 0x20 && c <= 0x7E)
    {
        return c + 1;
    }
    return c;
}

char decrypt(char c)
{
    if (c >= 0x20 && c <= 0x7E)
    {
        return c - 1;
    }
    else
    {
        printf("%c\n", '.');
    }
    return c;
}

char xprt(char c)
{
    printf("%x\n", c);
    return c;
}

void menu()
{
    char *carray = (char *)calloc(5,sizeof(char));
    struct fun_desc menu[] = {
        {"Get string", my_get},
        {"Print string", cprt},
        {"Encrypt", encrypt},
        {"Decrypt", decrypt},
        {"Print Hex", xprt},
        {NULL, NULL}};
    int menuSize = (sizeof menu - 1) / sizeof menu[0];
    char input[1000];
    while (1)
    {
        printf("Enter function number you want to use : \n");
        for (int i = 0; i < menuSize; ++i)
        {
            printf("%d) ", i);
            printf("%s\n", menu[i].name);
        }
        printf("%s", "Option : ");
        
        if(fgets(input, 1000, stdin) == NULL)
        {
            free(carray);
            break;
        }
        int numberPicked = atoi(input);

        if (numberPicked >= 0 && numberPicked < menuSize && input[0]!= '\0' && input[0]!= '\n')
        {
            printf("Within bounds\n");
            carray = map(carray, 5, menu[numberPicked].f);
            printf("Done.\n");
        }
        else{
            printf("Not within bounds\n");
        }
        
    }
    
}

int main(int argc, char **argv)
{
    menu();
}
