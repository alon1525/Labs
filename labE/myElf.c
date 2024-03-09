#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>

struct fun_desc
{
    char *name;
    void (*fun)();
};
int debug_mode = 0;
int files = 0;
void *map_start1;
void *map_start2;
int file_size1;
int file_size2;
char buffer1[100];
char buffer2[100];

void ToggleDebugMode()
{
    if (debug_mode == 0)
    {
        debug_mode = 1;
        fprintf(stdout, "Debug flag now on\n");
    }
    else
    {
        debug_mode = 0;
        fprintf(stdout, "Debug flag now off\n");
    }
}

void ExamineELFFile()
{
    if (files > 2)
    {
        fprintf(stderr, "Too many files\n");
        return;
    }
    char buffer[100];
    printf("Enter the file name: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    int fd = open(buffer, O_RDONLY);
    if (fd == -1)
    {
        printf("Error opening file\n");
        return;
    }

    struct stat file_stats;
    if (fstat(fd, &file_stats) == -1)
    {
        printf("Error getting file status\n");
        close(fd);
        return;
    }

    char *addr = mmap(NULL, file_stats.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED)
    {
        printf("Error mapping file\n");
        close(fd);
        return;
    }
    Elf32_Ehdr *header = (Elf32_Ehdr *)addr;
    if (memcmp(header->e_ident, ELFMAG, 3) == 0)
    {
        printf("magic number: %c%c%c\n", (char)header->e_ident[1], (char)header->e_ident[2], (char)header->e_ident[3]);

        if (header->e_ident[EI_DATA] == ELFDATA2MSB)
        {
            printf("Big endian\n");
        }
        else
        {
            printf("Little endian\n");
        }
        printf("The entry point is 0x%x\n", header->e_entry);
        printf("Section header:\noffset: 0x%x\n", header->e_shoff);
        printf("count: %d\n", header->e_shnum);
        printf("size: %d\n\n", header->e_shentsize);

        printf("Program header:\noffset: 0x%x\n", header->e_phoff);
        printf("count: %d\n", header->e_phnum);
        printf("size: %d\n", header->e_phentsize);

        if (files == 0)
        {
            map_start1 = addr;
            file_size1 = file_stats.st_size;
            strcpy(buffer1, buffer);
        }
        else
        {
            map_start2 = addr;
            file_size2 = file_stats.st_size;
            strcpy(buffer2, buffer);
        }
        files++;
    }
    else
    {
        printf("The File %s is not an ELF file\n", buffer);
    }
}
const char *GetSectionString(unsigned int sh_type)
{
    switch (sh_type)
    {
    case SHT_NULL:
        return "NULL";
    case SHT_PROGBITS:
        return "PROGBITS";
    case SHT_SYMTAB:
        return "SYMTAB";
    case SHT_STRTAB:
        return "STRTAB";
    case SHT_RELA:
        return "RELA";
    case SHT_HASH:
        return "HASH";
    case SHT_DYNAMIC:
        return "DYNAMIC";
    case SHT_NOTE:
        return "NOTE";
    case SHT_NOBITS:
        return "NOBITS";
    case SHT_REL:
        return "REL";
    case SHT_SHLIB:
        return "SHLIB";
    case SHT_DYNSYM:
        return "DYNSYM";
    case SHT_INIT_ARRAY:
        return "INIT_ARRAY";
    case SHT_FINI_ARRAY:
        return "FINI_ARRAY";
    case SHT_PREINIT_ARRAY:
        return "PREINIT_ARRAY";
    case SHT_GROUP:
        return "GROUP";
    case SHT_SYMTAB_SHNDX:
        return "SYMTAB_SHNDX";
    case SHT_GNU_ATTRIBUTES:
        return "GNU_ATTRIBUTES";
    case SHT_GNU_HASH:
        return "GNU_HASH";
    case SHT_GNU_LIBLIST:
        return "GNU_LIBLIST";
    case SHT_GNU_verdef:
        return "GNU_VERDEF";
    case SHT_GNU_verneed:
        return "GNU_VERNEED";
    case SHT_GNU_versym:
        return "GNU_VERSYM";
    default:
        return "UNKNOWN";
    }
}

void PrintSectionInfo(Elf32_Shdr *section_header_table, int num_sections, char *section_names)
{
    printf("Sections:\n");
    for (int i = 0; i < num_sections; i++)
    {
        printf("[%02d] %-20s %06x %06x %06x %s\n",
               i,
               &section_names[section_header_table[i].sh_name],
               section_header_table[i].sh_addr,
               section_header_table[i].sh_offset,
               section_header_table[i].sh_size,
               GetSectionString(section_header_table[i].sh_type));
        if (debug_mode)
        {
            printf("[Debug] sh_name offset: %08x\n", section_header_table[i].sh_name);
        }
    }
}

void PrintName(void *map_start)
{
    Elf32_Ehdr *header = (Elf32_Ehdr *)map_start;
    Elf32_Shdr *section_header_table = (Elf32_Shdr *)(map_start + header->e_shoff);
    int num_sections = header->e_shnum;
    Elf32_Shdr *section_names_header = &section_header_table[header->e_shstrndx];
    char *section_names = (char *)(map_start + section_names_header->sh_offset);
    
    PrintSectionInfo(section_header_table, num_sections, section_names);
    printf("\n[Debug] shstrndx: %hu\n", header->e_shstrndx);
}
void PrintSectionsNames()
{
    if (files == 2)
    {
        printf("File: %s\n", buffer1);
        PrintName(map_start1);
        printf("\n");
        printf("File: %s\n", buffer2);
        PrintName(map_start2);
        printf("\n");
    }
    else if (files == 1)
    {
        printf("File: %s\n", buffer1);
        PrintName(map_start1);
        printf("\n");
    }
}

void PrintSymbols(void *map_start)
{
    Elf32_Ehdr *header = (Elf32_Ehdr *)map_start;
    Elf32_Shdr *section_headers = (Elf32_Shdr *)(map_start + header->e_shoff);
    Elf32_Shdr *section_names_header = &section_headers[header->e_shstrndx];
    char *section_names = (char *)(map_start + section_names_header->sh_offset);

    for (int i = 0; i < header->e_shnum; i++)
    {
        Elf32_Shdr *current_section = &section_headers[i];
        if (current_section->sh_type == SHT_SYMTAB || current_section->sh_type == SHT_DYNSYM)
        {
            Elf32_Sym *symbols = (Elf32_Sym *)(map_start + current_section->sh_offset);
            int num_symbols = current_section->sh_size / current_section->sh_entsize;
            char *strtab = (char *)(map_start + section_headers[current_section->sh_link].sh_offset);
            printf("Symbol table '%s' contains %d entries:\n", &section_names[current_section->sh_name], num_symbols);
            for (int j = 0; j < num_symbols; j++)
            {
                char section_name[20] = "";
                if (symbols[j].st_shndx != SHN_ABS && symbols[j].st_shndx != SHN_UNDEF)
                {
                    strcpy(section_name, &section_names[section_headers[symbols[j].st_shndx].sh_name]);
                }
                const char *shndx_str;
                if (symbols[j].st_shndx == SHN_ABS)
                {
                    shndx_str = "ABS";
                }
                else if (symbols[j].st_shndx == SHN_UNDEF)
                {
                    shndx_str = "UND";
                }
                else
                {
                    printf("[%02d] %08x %03d %-10s %-20s\n", j, symbols[j].st_value, symbols[j].st_shndx, section_name, &strtab[symbols[j].st_name]);
                    continue;
                    ;
                }

                printf("[%02d] %08x %s %-10s %-20s\n", j, symbols[j].st_value, shndx_str, section_name, &strtab[symbols[j].st_name]);
            }
        }
    }
}
void PrintSymbol()
{
    if (files == 2)
    {
        printf("File: %s\n", buffer1);
        PrintSymbols(map_start1);
        printf("\n");
        printf("File: %s\n", buffer2);
        PrintSymbols(map_start2);
        printf("\n");
    }
    else if (files == 1)
    {
        printf("File: %s\n", buffer1);
        PrintSymbols(map_start1);
        printf("\n");
    }
}

Elf32_Sym *getSymbol(Elf32_Sym *symbols, int size, char *strtab, char *name)
{
    for (int i = 0; i < size; i++)
    {
        if (strcmp(name, &strtab[symbols[i].st_name]) == 0)
        {
            return &symbols[i];
        }
    }
    return NULL;
}

void CheckFilesforMerge()
{
    if (files != 2)
    {
        printf("Please ensure both files are loaded before merging!\n");
        return;
    }

    Elf32_Ehdr *header1 = (Elf32_Ehdr *)map_start1;
    Elf32_Shdr *section_headers1 = (Elf32_Shdr *)(map_start1 + header1->e_shoff);
    Elf32_Sym *symbols1 = NULL;
    int symbols_size1;
    char *strtab1;

    for (int i = 0; i < header1->e_shnum; i++)
    {
        if (section_headers1[i].sh_type == SHT_SYMTAB || section_headers1[i].sh_type == SHT_DYNSYM)
        {
            if (symbols1 != NULL)
            {
                printf("Merging multiple symbol tables is not supported!\n");
                return;
            }
            symbols1 = (Elf32_Sym *)(map_start1 + section_headers1[i].sh_offset);
            symbols_size1 = section_headers1[i].sh_size / section_headers1[i].sh_entsize;
            strtab1 = (char *)(map_start1 + section_headers1[section_headers1[i].sh_link].sh_offset);
        }
    }

    Elf32_Ehdr *header2 = (Elf32_Ehdr *)map_start2;
    Elf32_Shdr *section_headers2 = (Elf32_Shdr *)(map_start2 + header2->e_shoff);
    Elf32_Sym *symbols2 = NULL;
    int symbols_size2;
    char *strtab2;

    for (int i = 0; i < header2->e_shnum; i++)
    {
        if (section_headers2[i].sh_type == SHT_SYMTAB || section_headers2[i].sh_type == SHT_DYNSYM)
        {
            if (symbols2 != NULL)
            {
                printf("Merging multiple symbol tables is not supported!\n");
                return;
            }
            symbols2 = (Elf32_Sym *)(map_start2 + section_headers2[i].sh_offset);
            symbols_size2 = section_headers2[i].sh_size / section_headers2[i].sh_entsize;
            strtab2 = (char *)(map_start2 + section_headers2[section_headers2[i].sh_link].sh_offset);
        }
    }

    for (int i = 1; i < symbols_size1; i++)
    {
        Elf32_Sym *symbol = getSymbol(symbols2, symbols_size2, strtab2, &strtab1[symbols1[i].st_name]);
        if (symbols1[i].st_shndx == SHN_UNDEF)
        {
            if (symbol == NULL || symbol->st_shndx == SHN_UNDEF)
            {
                printf("Symbol %s is undefined!\n", &strtab1[symbols1[i].st_name]);
            }
        }
        else if (symbol != NULL && symbol->st_shndx != SHN_UNDEF)
        {
            printf("Symbol %s is multiply defined!\n", &strtab1[symbols1[i].st_name]);
        }
    }
}

void MergeELFFiles()
{
    printf("not implemented");
}
void Quit()
{
    if (files == 2)
    {
        munmap(map_start1, file_size1);
        munmap(map_start2, file_size2);
    }
    if (files == 1)
        munmap(map_start1, file_size2);
    printf("Quit\n");
    exit(0);
}

void ToggleDebugMode();
void ExamineELFFile();
void PrintSectionsNames();
void PrintSymbol();
void Check_Files_for_Merge();
void MergeELFFiles();
void Quit();

int main()
{
    struct fun_desc menu[] = {{"Toggle Debug Mode", ToggleDebugMode},
                              {"Examine ELF File", ExamineELFFile},
                              {"Print Section Names", PrintSectionsNames},
                              {"Print Symbols", PrintSymbol},
                              {"Check Files For Merge", CheckFilesforMerge},
                              {"Merge ELF Files", MergeELFFiles},
                              {"Quit", Quit},
                              {NULL, NULL}};
    int size = (sizeof menu - 1) / sizeof menu[0];

    while (1)
    {

        printf("%s", "Choose action: \n");
        for (int i = 0; i < size; ++i)
        {
            printf("%d) ", i);
            printf("%s\n", menu[i].name);
        }

        printf("%s", "Option: ");
        char input[1000];
        fgets(input, 1000, stdin);
        int num = atoi(input);
        if (num >= size || num < 0)
        {
            printf("%s\n", "Not within bounds");
            exit(0);
        }
        printf("%s\n", "Within bounds");

        menu[num].fun();
        printf("%s\n\n", "DONE.");
    }
}