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
#include <mem/vmem.h>

EMBOX_CMD(exec);

static void *execve_trampoline(void *data);

static int exec(int argc, char **argv) {
	FILE *elf_file = fopen(argv[argc - 1], "r");

	if (elf_file == NULL) {
		printf("Cannot open file %s\n", argv[argc - 1]);
		return -1;
	}

	new_task(execve_trampoline, (void *) elf_file, 0);

	return 0;
}

static inline int elf_load_in_mem(FILE *fd, Elf32_Ehdr *EH, Elf32_Phdr *EPH) {
	for (int i = 0; i < EH->e_phnum; i++) {
		if (EPH->p_type == PT_LOAD) {
			vmem_create_space(task_self()->vmem_data->ctx, EPH->p_paddr, L_REV(EPH->p_memsz, EH->e_ident[EI_DATA]), VMEM_PAGE_WRITABLE | VMEM_PAGE_USERMODE);
			elf_read_segment(fd, EH, EPH, (int8_t *)EPH->p_paddr);
		}

		EPH = (Elf32_Phdr *) ((unsigned char *) EPH + EH->e_phentsize);
	}

	return 0;
}

static void *execve_trampoline(void *data) {
	int (*function_main)(int argc, char * argv[]);
	FILE *elf_file = (FILE *) data;
	Elf32_Ehdr *elf_header = NULL;
	Elf32_Phdr *program_headers = NULL;

	if (elf_read_header(elf_file, &elf_header) < 0) {
		return NULL;
	}

	if (elf_read_segments_table(elf_file, elf_header, &program_headers) < 0) {
		return NULL;
	}

	elf_load_in_mem(elf_file, elf_header, program_headers);

	fclose(elf_file);

    function_main = (int (*)(int argc, char *argv[])) elf_header->e_entry;
    function_main (0, NULL);


	// XXX: replace it?
	if (elf_header != NULL) free(elf_header);
	if (program_headers != NULL) free(program_headers);

	return NULL;
}

