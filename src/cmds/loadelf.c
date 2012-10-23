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
#include <module/embox/arch/usermode.h>

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

static inline int elf_load_in_mem(FILE *fd, Elf32_Obj *obj) {
	Elf32_Phdr *ph;
	for (int i = 0; i < obj->header->e_phnum; i++) {
		ph = obj->ph_table + i;
		if (ph->p_type == PT_LOAD) {
			vmem_create_space(task_self()->vmem_data->ctx, ph->p_paddr, ph->p_memsz, VMEM_PAGE_WRITABLE | VMEM_PAGE_USERMODE);
			elf_read_segment(fd, ph, (int8_t *)ph->p_paddr);
		}
	}

	return 0;
}

static void *execve_trampoline(void *data) {
	extern void elf_start(void *entry);
	void *elf_entry;
	FILE *elf_file = (FILE *) data;
	Elf32_Obj elf;

	elf_initialize_object(&elf);

	if (elf_read_header(elf_file, &elf) < 0) {
		return NULL;
	}

	if (elf_read_program_header_table(elf_file, &elf) < 0) {
		return NULL;
	}

	elf_load_in_mem(elf_file, &elf);
    elf_entry = (void *) elf.header->e_entry;

	fclose(elf_file);
	elf_finilize_object(&elf);

    //usermode_call_and_switch_if(1, function_main, NULL);
	elf_start(elf_entry);

	return NULL;
}
