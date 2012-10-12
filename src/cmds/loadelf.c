/**
 * @file
 * @brief Loadelf loads ELF binaries.
 *
 * @date 27.09.2012
 * @author Anton Bulychev
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <ctype.h>
#include <stdio.h>
#include <lib/libelf.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <kernel/task.h>

EMBOX_CMD(exec);

static void *execve_trampoline(void *data);

static int exec(int argc, char **argv) {
	FILE *elf_file = fopen(argv[argc - 1], "r");

	if (elf_file == NULL) {
		printf("Cannot open file %s\n", argv[argc - 1]);
		return -1;
	}

	new_task(execve_trampoline, (void *) elf_file, TASK_FLAG_USERMODE);

	return 0;
}


static void *execve_trampoline(void *data) {
	FILE *elf_file = (FILE *) data;
	Elf32_Ehdr *elf_header = NULL;
	Elf32_Phdr *program_headers = NULL;

	if (elf_read_header(elf_file, &elf_header) < 0) {
		return NULL;
	}

	if (elf_read_segments_table(elf_file, elf_header, &program_headers) < 0) {
		return NULL;
	}

	elf_execve(elf_file, elf_header, program_headers);

	fclose(elf_file); /* TODO: Should be done before creating of new task. */

	// XXX: replace it?
	if (elf_header != NULL) free(elf_header);
	if (program_headers != NULL) free(program_headers);

	return NULL;
}

