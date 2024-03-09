#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char size_variable_buffer[5];

typedef struct
{
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    char display_mode;
    /*
   .
   .
   Any additional fields you deem necessary
  */
} state;

void Toggle_debug_mode(state *s)
{
    if (s->debug_mode == '0')
    {
        s->debug_mode = '1';
        fprintf(stderr, "Debug flag now on\n");
    }
    else
    {
        fprintf(stderr, "Debug flag now off\n");
        s->debug_mode = '0';
    }
}

void Set_File_Name(state *s)
{
    fprintf(stderr, "Please enter a file name\n");
    char *the_input = fgets(s->file_name, 100, stdin);
    size_t length = strlen(s->file_name);
    if (length > 0 && s->file_name[length - 1] == '\n')
        s->file_name[length - 1] = '\0';
    if (s->debug_mode == '1')
    {
        fprintf(stderr, "Debug: file name set to %s\n", the_input);
    }
}

void Set_Unit_Size(state *s)
{
    fprintf(stderr, "Please enter a number\n");
    char *the_size = fgets(size_variable_buffer, sizeof(size_variable_buffer), stdin);
    int num_size = atoi(the_size);
    if (num_size == 1 || num_size == 2 || num_size == 4)
    {
        s->unit_size = num_size;

        if (s->debug_mode == '1')
        {
            fprintf(stderr, "Debug: set size to %d\n", num_size);
        }
    }
    else
    {
        perror("The selected size is not valid\n"); 
    }
}

void Quit(state *s)
{
    if (s->debug_mode == '1')
    {
        fprintf(stderr, "quitting\n");
    }
    exit(0);
}

void Load_Into_Memory(state *s)
{
    if (strlen(s->file_name) == 0)
    {
        perror("File name is empty.\n"); 
        return;
    }
    //Open file_name for reading
    FILE *file = fopen(s->file_name, "rb");
    if (file == NULL)
    {
        perror("Failed to open file \n"); 
        return;
    }
    // Prompt the user for location and length
    unsigned int location;
    size_t length;
    char memory_load_input[256];
    printf("Please enter <location> <length>\n");
    if (fgets(memory_load_input, sizeof(memory_load_input), stdin) == NULL)
    {
        fprintf(stderr, "Error reading memory_load_input\n"); 
        fclose(file);
        return;
    }
    if (sscanf(memory_load_input, "%x %zu", &location, &length) != 2)
    {
        fprintf(stderr, "Error: Invalid location or length.\n");
        fclose(file);
        return;
    }
    if (s->debug_mode == '1')
    {
        printf("File Name: %s\n", s->file_name);
        printf("Location: 0x%x\n", location);
        printf("Length: %zu\n", length);
    }
    if (fseek(file, location, SEEK_SET) != 0)
    {
        fprintf(stderr, "Error: Unable to move file pointer to the specified location.\n");
        fclose(file);
        return;
    }
    size_t total_bytes = length * s->unit_size;
    size_t items_read = fread(s->mem_buf, s->unit_size, length, file);
    if (items_read == length)
    {
        printf("Loaded %zu units into memory\n", total_bytes);
    }
    else
    {
        fprintf(stderr, "Error: Failed to read items from the file.\n");
        fclose(file);
        return;
    }

    fclose(file);
}

void Toggle_Display_Mode(state *s)
{
    if (s->display_mode == '0')
    {
        s->display_mode = '1';
        printf("Display flag now on, hexadecimal representation\n");
    }
    else
    {
        printf("Display flag now off, decimal representation\n");
        s->display_mode = '0';
    }
}

void Memory_Display(state *s)
{
    static char *hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
    static char *dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};
    size_t u;
    unsigned int addr;
    char display_input[256];
    fprintf(stderr, "Enter address and length\n"); 
    if (fgets(display_input, sizeof(display_input), stdin) == NULL)
    {
        fprintf(stderr, "Error reading display_input\n");
        return;
    }
    if (sscanf(display_input, "%x %zu", &addr, &u) != 2)
    {
        fprintf(stderr, "Error: Invalid address or length.\n");
        return;
    }    
    unsigned char *start_addr = s->mem_buf;
    if (addr != 0){
        start_addr = s->mem_buf + addr;
    }
    unsigned char *last = start_addr + s->unit_size * u;

    if (s->display_mode == '1')
    {
        printf("Hexadecimal\n");
        printf("===========\n");
        for (size_t i = 0; i < u; ++i)
        {
            if (start_addr + i < last)
            {
                unsigned int val = *((unsigned int *)(start_addr + i * s->unit_size));
                printf(hex_formats[s->unit_size - 1], val);
            }
            else
            {
                printf("No such unit\n");
            }
        }
    }
    else
    {
        printf("Decimal\n");
        printf("=======\n");
        for (size_t i = 0; i < u; ++i)
        {
            if (start_addr + i < last)
            {
                unsigned int val = *((unsigned int *)(start_addr + i * s->unit_size));
                printf(dec_formats[s->unit_size - 1], val);
            }
            else
            {
                printf("No such unit\n");
            }
        }
    }
}

void Save_Into_File(state *s)
{
    static char *hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
    static char *dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};
    unsigned int sourceAddress;
    unsigned int targetLocation;
    unsigned int length;
    char buffer[256];
    if (strlen(s->file_name) == 0)
    {
        printf("File Name Invalid\n");
        return;
    }
    
    printf("Please enter <source-address> <target-location> <length>\n");
    fgets(buffer, sizeof(buffer), stdin);
    if (sscanf(buffer,"%x %x %d", &sourceAddress, &targetLocation,&length) != 3)
    {
        printf("Error: Invalid sourceAddress or targetLocation or length.\n"); 
        return;
    }
    printf(hex_formats[s->unit_size - 1], sourceAddress);
    printf(hex_formats[s->unit_size - 1], targetLocation);
    printf(dec_formats[s->unit_size - 1], length);
    int size = 0;
    FILE* file = fopen(s->file_name, "r+");
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if(targetLocation > size)
    {
        printf("Error: target location is greater than the size of the file\n");
        return;
    }
    fseek(file, targetLocation, SEEK_SET); 
    if (sourceAddress == 0)
    {
        fwrite(&(s->mem_buf), s->unit_size, length,file);
    }
    else
    {
        fwrite(&(sourceAddress), s->unit_size, length,file);
    }
    fclose(file);
}

void Memory_Modify(state *s)
{
    static char *hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
    char input[256];
    int location;
    int val;

    printf("Please enter <location> <val>: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %x", &location,&val);
    if(s->debug_mode == '1'){
        printf(hex_formats[s->unit_size - 1], location);
        printf(hex_formats[s->unit_size - 1], val);
    }
    strncpy((char*)(s->mem_buf) + location, (char*)&val, s->unit_size);   
}

void (*pointer_Toggle_Debug_Mode)(state *) = &Toggle_debug_mode;
void (*pointer_Set_File_Name)(state *) = &Set_File_Name;
void (*pointer_Set_Unit_Size)(state *) = &Set_Unit_Size;
void (*pointer_Load_Into_Memory)(state *) = &Load_Into_Memory;
void (*pointer_Toggle_Display_Mode)(state *) = &Toggle_Display_Mode;
void (*pointer_Memory_Display)(state *) = &Memory_Display;
void (*pointer_Save_Into_File)(state *) = &Save_Into_File;
void (*pointer_Memory_Modify)(state *) = &Memory_Modify;
void (*pointer_Quit)(state *) = &Quit;

struct fun_desc
{
    char *name;
    void (*fun)(state *);
};

int main(int argc, char **argv)
{
    state *my_state = malloc(sizeof(state));
    my_state->debug_mode = '0';
    my_state->display_mode = '0';
    memcpy(my_state->mem_buf,"",10000);

    char input_buffer[5];


    struct fun_desc menu[] =
        {{"Toggle Debug Mode", pointer_Toggle_Debug_Mode},
         {"Set File Name", pointer_Set_File_Name},
         {"Set Unit Size", pointer_Set_Unit_Size},
         {"Load Into Memory", pointer_Load_Into_Memory},
         {"Toggle Display Mode", pointer_Toggle_Display_Mode},
         {"Memory Display", pointer_Memory_Display},
         {"Save Into File", pointer_Save_Into_File},
         {"Memory Modify", pointer_Memory_Modify},
         {"Quit", pointer_Quit},
         {NULL, NULL}};
    if (my_state->debug_mode == '1')
    {
        printf("unit size: %d\nfile name: %s\nmem count: %d\n", my_state->unit_size, my_state->file_name, my_state->mem_count);
    }

    fprintf(stdout, "Choose action:\n"); 
    int i = 0;

    while (menu[i].name != NULL)
    {
        fprintf(stdout, "%d) %s\n", i, menu[i].name);
        ++i;
    }
    char *the_input = fgets(input_buffer, sizeof(input_buffer), stdin);
    while (the_input != NULL)
    {

        int num_input = atoi(the_input);
        if (num_input >= 0 && num_input < 9)
        {
            fprintf(stdout, "Within bounds\n");
            menu[num_input].fun(my_state);
            printf("DONE.\n");
        }
        else
        {
            fprintf(stdout, "Not within bounds\n");
            free(my_state);
            exit(0);
        }
        if (my_state->debug_mode == '1')
        {
            printf("unit size: %d\nfile name: %s\nmem count: %d\n", my_state->unit_size, my_state->file_name, my_state->mem_count);
        }

        fprintf(stdout, "Choose action:\n");
        int i = 0;

        while (menu[i].name != NULL)
        {
            fprintf(stdout, "%d) %s\n", i, menu[i].name);
            ++i;
        }
        the_input = fgets(input_buffer, sizeof(input_buffer), stdin);
    }
    free(my_state);
    return 0;
}