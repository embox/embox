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

static inline int elf_load_in_mem(FILE *fd, Elf32_Obj *obj) {
	Elf32_Shdr *sh;
	Elf32_Addr addr;
	size_t size = 0;


	for (int i = 0; i < obj->header->e_shnum; i++) {
		size += obj->sh_table[i].sh_size;
	}

	if (!(addr = (Elf32_Addr) malloc(size))) {
		return -ENOMEM;
	}

	// TODO: Replace it somewhere
	for (int i = 0; i < obj->header->e_shnum; i++) {
		sh = &obj->sh_table[i];

		sh->sh_addr = addr;

		fseek(fd, sh->sh_offset, 0);
		if (!sh->sh_size) {
			continue;
		}

		if (sh->sh_size != fread((void *) sh->sh_addr, sh->sh_size, 1, fd)) {
			return -EBADF;
		}

		addr += sh->sh_size;
	}

	return ENOERR;
}

static int execve_trampoline(void *data) {
	int (*f_init)(void);
	FILE *elf_file = (FILE *) data;
	Elf32_Obj elf;
	int err;

	elf_initialize_object(&elf);

	// FIXME: free mem
	if ((err = elf_read_header(elf_file, &elf)) < 0) {
		return err;
	}

	if ((err = elf_read_section_header_table(elf_file, &elf)) < 0) {
		return err;
	}

	if ((err = elf_read_symbol_table(elf_file, &elf)) < 0) {
		return err;
	}

	if ((err = elf_read_symbol_names(elf_file, &elf)) < 0) {
		return err;
	}

	if ((err = elf_load_in_mem(elf_file, &elf)) < 0) {
		return err;
	}

	if ((err = elf_relocate(elf_file, &elf)) < 0) {
		return err;
	}

	f_init = (int (*)(void)) elf.sh_table[1].sh_addr;

	fclose(elf_file);
	elf_finilize_object(&elf);

	f_init();
	//usermode_call_and_switch_if(1, function_main, NULL);

	return ENOERR;
}
