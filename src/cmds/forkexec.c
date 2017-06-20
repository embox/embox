/**
 * @file
 * @brief
 *
 * @date 21.11.2012
 * @author Anton Bulychev
 */

#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include <kernel/task.h>
#include "lib/libelf.h"
#include <errno.h>

#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <util/math.h>
#include <sys/mman.h>

#include <lib/libelf.h>
#include <kernel/usermode.h>
#include <mem/mmap.h>
#include <kernel/task.h>
#include <kernel/task/resource/mmap.h>

//static void *new_task_entry(void *file);

int main(int argc, char **argv) {
    /*char *filename;
	int pid;
*/
    if(argc < 1) return -1;

    // need sto read segments from file
    Elf32_Ehdr header;
    int elf_file;
    int err;

    elf_file = open(argv[argc - 1], O_RDONLY);

    // read elf file header
    if ((err = elf_read_header(elf_file, &header)) < 0) {
        close(elf_file);
        return err;
    }

    Elf32_Shdr * section_header_table = malloc(sizeof(Elf32_Shdr)*header.e_shnum);


    // read section header table
    if((err = elf_read_sh_table(elf_file, &header, section_header_table)) < 0)
    {
        close(elf_file);
        return err;
    }

    // read .shstrtab section
    Elf32_Shdr *names_section = malloc(sizeof(Elf32_Shdr));
    ///TODO change parametrs \index and name to elf_read_header....
    elf_read_section_by_index(elf_file, &header, names_section, header.e_shstrndx);

    // array of section names, string are presented as null-terminated
    char * names = malloc(names_section->sh_size);
    elf_read_section(elf_file, names_section, names);

    // text section. Used to iterate a table
    Elf32_Shdr *text_section = section_header_table;

    int i = 0;

    while(strcmp(names + text_section->sh_name, ".text") != 0)
    {
        text_section++;
        i++;
        // iterate through all table and didn't find .text section
        if(i == header.e_shnum) return -1;
    }

    void * instructions = malloc(text_section->sh_size);
    elf_read_section(elf_file, text_section, instructions);

    printf("Code address : %p \n", instructions);

    int (*functionPtr)();

    functionPtr = instructions;
    printf("Result : %d", functionPtr());

    /*
    /*filename = malloc(strlen(argv[argc-1]));
	strcpy(filename, argv[argc - 1]);
	pid = new_task(filename, new_task_entry, filename);

	if (pid > 0) {
		task_waitpid(pid);
	}

	free(filename);

	return pid > 0 ? 0 : pid;
    */

    return 0;
}
/*
extern int execve_syscall(const char *filename, char *const argv[], char *const envp[]);

static void *new_task_entry(void *filename) {
	char *argv[2] = {filename, NULL};
	char *envp[1] = {NULL};

	execve_syscall(filename, argv, envp);

	return NULL;
}
*/
