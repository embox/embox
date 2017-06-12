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

    Elf32_Ehdr header;
    int elf_file;
    int err;

    elf_file = open(argv[argc - 1], O_RDONLY);

    if ((err = elf_read_header(elf_file, &header)) < 0) {
        close(elf_file);
        return err;
    }
    void (*functionPtr)();
    functionPtr = (void*) header.e_entry;
    (*functionPtr)();

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
