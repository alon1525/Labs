#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void debugMode();

void enc(bool mode, int isPlus, char keys[], FILE *outputFile, FILE *inputFile);

void subString(const char *source, char *dest, int start, int length);

int findLength(char *str);
bool stop = false;

int main(int argc, char *argv[])
{
    FILE *inputFile = stdin;
    FILE *outputFile = stdout;
    for (int i = 1; i < argc; ++i)
    {
        int length = findLength(argv[i]);
        char adress[length - 2];
        subString(argv[i], adress, 2, length);

        if (strncmp(argv[i], "-I", 2) == 0)
        {
            inputFile = fopen(adress, "r");
            if (inputFile == NULL)
            {
                perror("Error opening input file");
                return 1;
            }
        }
        else if (strncmp(argv[i], "-O", 2) == 0)
        {
            outputFile = fopen(adress, "w");
            if (outputFile == NULL)
            {
                perror("Error opening output file");
                return 1;
            }
        }
    }

    bool mode = true;
    char *eWord = 0;

    for (int i = 1; i < argc && stop == false; ++i)
    {
        if (argv[i][1] == 'E')
        {
            eWord = argv[i];
            if (eWord != 0)
            {
                if (eWord[0] == '+')
                {
                    enc(mode, 1, eWord, outputFile, inputFile);
                }
                else if (eWord[0] == '-')
                {
                    enc(mode, 0, eWord, outputFile, inputFile);
                }
            }
        }
        else if (argv[i][1] == 'D')
        {
            if (argv[i][0] == '+')
            {
                mode = true;
            }
            else if (argv[i][0] == '-')
            {
                mode = false;
            }
        }
        else
        {
            {
                if (mode == true)
                {
                    printf("%s ", argv[i]);
                }
            }
        }
    }


    if (inputFile != stdin)
    {
        fclose(inputFile);
    }

    if (outputFile != stdout)
    {
        fclose(outputFile);
    }
}
void subString(const char *source, char *dest, int start, int length)
{
    int i;

    for (i = 0; i < length && source[start + i] != '\0'; ++i)
    {
        dest[i] = source[start + i];
    }

    dest[i] = '\0';
}

void debugMode(int argc, char *argv[])
{
}

int findLength(char *str)
{
    int i = 1;
    while (str[i - 1] != '\0')
    {
        i++;
    }
    return i;
}

void enc(bool mode, int isPlus, char keys[], FILE *outputFile, FILE *inputFile)
{
    while (1)
    {
        char string;
        if (inputFile == stdin)
        {
            printf("%s", "enter your input: \n");
        }
        string = fgetc(inputFile);
        int i = 2;
        if (string == 'q')
        {
            break;
        }
        
        if (isPlus == 2)
        {
            i = 0;
            for (int j = 0; i < findLength(keys); i++)
            {
                printf("%d", keys[j]);
            }
            break;
        }
        
        while (string != '\n')
        {
            if (keys[i] == '\0')
            {
                i = 2;
            }
            if (!(((string < 91) & (string > 64)) | ((string > 47) & (string < 58))))
            {
                fputc(string, outputFile);
            }
            else
            {
                if (isPlus == 1)
                    if ((string < 58) & (string > 47) & ((string + keys[i] - 48) > 57))
                    {
                        fputc((string + keys[i] - 48) - 10, outputFile);
                    }
                    else if ((string < 58) & (string > 47))
                    {
                        fputc((string + keys[i] - 48), outputFile);
                    }
                    else if (string + keys[i] - 48 > 90)
                    {
                        fputc((string + keys[i] - 48) - 26, outputFile);
                    }
                    else
                    {
                        fputc((string + keys[i] - 48), outputFile);
                    }
                else if (isPlus == 0)
                {
                    if ((string < 58) & (string > 47) & ((string - keys[i] + 48) < 48))
                    {
                        fputc((string - keys[i] + 48) + 10, outputFile);
                    }
                    else if ((string < 58) & (string > 47))
                    {
                        fputc((string - keys[i] + 48), outputFile);
                    }

                    else if (string - keys[i] + 48 < 65)
                    {
                        fputc((string - keys[i] + 48) + 26, outputFile);
                    }
                    else
                    {
                        fputc((string - keys[i] + 48), outputFile);
                    }
                }
            }
            string = fgetc(inputFile);

            i += 1;
        }
        if (mode == true)
        {
            printf("\n%s", keys);
        }
        printf("%s", "\n");
    }
}