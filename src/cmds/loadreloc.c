/**
 * @file
 * @brief Loadreloc loads relocatable ELF binaries.
 *
 * @date 26.10.2012
 * @author Anton Bulychev
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <ctype.h>
#include <stdio.h>
#include <lib/libelf.h>
#include <lib/dl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>

EMBOX_CMD(exec);

static int execve_trampoline(void *data);

static int exec(int argc, char **argv) {
	FILE *elf_file = fopen(argv[argc - 1], "r");

	if (elf_file == NULL) {
		printf("Cannot open file %s\n", argv[argc - 1]);
		return -1;
	}

	return execve_trampoline((void *) elf_file);

	return 0;
}

static inline int elf_load_in_mem(FILE *fd, Elf32_Obj *obj, uint32_t *offsets) {
	Elf32_Shdr *sh;
	void *addr;
	size_t size = 0;

	for (int i = 0; i < obj->header->e_shnum; i++) {
		offsets[i] = size;
		size += obj->sh_table[i].sh_size;
	}

	addr = malloc(size);
	for (int i = 0; i < obj->header->e_shnum; i++) {
		sh = &obj->sh_table[i];
		offsets[i] += (size_t) addr;

		fseek(fd, sh->sh_offset, 0);
		if (!sh->sh_size) {
			continue;
		}

		if (sh->sh_size != fread((void *) offsets[i], sh->sh_size, 1, fd)) {
			return -EBADF;
		}
	}

	return ENOERR;
}

static int execve_trampoline(void *data) {
	int (*f_init)(void);
	FILE *elf_file = (FILE *) data;
	uint32_t *offsets;
	Elf32_Obj elf;

	elf_initialize_object(&elf);

	if (elf_read_header(elf_file, &elf) < 0) {
		return 1;
	}

	if (elf_read_section_header_table(elf_file, &elf) < 0) {
		return 1;
	}

	if (elf_read_symbol_table(elf_file, &elf) < 0) {
		return 1;
	}

	if (elf_read_symbol_names(elf_file, &elf) < 0) {
		return 1;
	}

	offsets = malloc(sizeof(uint32_t) * elf.header->e_shnum);
	elf_load_in_mem(elf_file, &elf, offsets);
	elf_relocate(elf_file, &elf, offsets);

	fclose(elf_file);
	elf_finilize_object(&elf);

	f_init = (int (*)(void)) offsets[1];
	assert(f_init() == 40);
	//usermode_call_and_switch_if(1, function_main, NULL);

	return ENOERR;
}
