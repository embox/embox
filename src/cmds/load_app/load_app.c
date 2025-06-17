/**
* @file
* @brief
*
* @date 30.06.2017
* @author Artem Sharganov
*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib/libelf.h"

#define MAX_LOADABLE_SECTIONS 0x10

static void print_usage(char **argv) {
	printf("Usage: %s [ELF_file]\n", argv[0]);
}

int main(int argc, char **argv) {
	Elf32_Addr sections_begin = 0xFFFFFFFF;
	Elf32_Addr sections_end = 0;

	void *mem = 0;

	Elf32_Ehdr *header = NULL;
	Elf32_Phdr *ph_table = NULL;

	int elf_file = 0;
	int err, ret;
	int (*entryFunction)() = NULL;

	if (argc != 2) {
		print_usage(argv);
		return 0;
	}

	header = malloc(sizeof(Elf32_Ehdr));
	if (header == NULL) {
		return -ENOMEM;
	}

	elf_file = open(argv[argc - 1], O_RDONLY);
	if ((err = elf_read_header(elf_file, header)) < 0) {
		ret = err;
		goto out;
	}

	ph_table = malloc(header->e_phnum * header->e_phentsize);

	if ((err = elf_read_ph_table(elf_file, header, ph_table)) < 0) {
		fprintf(stderr, "Wrong ELF file format");
		ret = err;
		goto out;
	}

	for (int i = 0; i < header->e_phnum; i++) {
		if (ph_table[i].p_type == PT_LOAD) {
			Elf32_Addr vaddr = ph_table[i].p_vaddr;
			Elf32_Word size = ph_table[i].p_memsz;

			if (vaddr < sections_begin) {
				sections_begin = vaddr;
			}

			if (vaddr + size > sections_end) {
				sections_end = vaddr + size;
			}
		}
	}

	mem = malloc(sections_end - sections_begin);
	if (mem == NULL) {
		fprintf(stderr, "Failed to allocate %d bytes for app\n",
				sections_end - sections_begin);
		ret = -1;
		goto out;
	}

	for (int i = 0; i < header->e_phnum; i++) {
		if (ph_table[i].p_type == PT_LOAD) {
			void *load_addr = mem + (ph_table[i].p_vaddr - sections_begin);

			if ((err = elf_read_segment(elf_file, &(ph_table[i]), load_addr)) < 0) {
				fprintf(stderr, "Failed to read section #%d", i);
				ret = err;
				goto out;
			}
		}
	}

	entryFunction = mem + (header->e_entry - sections_begin);

	if (!(((void *) entryFunction >= mem) &&
				((void *) entryFunction <= mem + (sections_end - sections_begin)))) {
		ret = -1;
		goto out;
	}

	ret = entryFunction();

out:
	if (elf_file != 0) {
		close(elf_file);
	}

	if (header != NULL) {
		free(header);
	}

	if (ph_table != NULL) {
		free(ph_table);
	}

	if (mem != NULL) {
		free(mem);
	}

	return ret;
}
