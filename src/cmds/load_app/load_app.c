/**
* @file
* @brief
*
* @date 30.06.2017
* @author Artem Sharganov
*/

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib/libelf.h"

int main(int argc, char **argv) {

	if (argc < 1) {
		return -1;
	}

	Elf32_Ehdr *header;
	int elf_file;
	int err;

	elf_file = open(argv[argc - 1], O_RDONLY);
	header = malloc(sizeof(Elf32_Ehdr));

	if ((err = elf_read_header(elf_file, header)) < 0) {
		close(elf_file);
		return err;
	}

	Elf32_Phdr *ph_table = malloc(header->e_phnum * header->e_phentsize);

	if ((err = elf_read_ph_table(elf_file, header, ph_table)) < 0) {
		close(elf_file);
		fprintf(stderr, "Wrong ELF file format");
		return err;
	}

	char *instructions = NULL;
	int offset = header->e_entry;

	for (int i = 0; i < header->e_phnum; i++) {
		/* TODO: PT_LOAD may be more than one */
		if (ph_table[i].p_type == PT_LOAD) {
			/* calculate offset of entry point */
			offset -= ph_table[i].p_vaddr;
			instructions = malloc(ph_table[i].p_memsz);

			if ((err = elf_read_segment(elf_file, &(ph_table[i]), instructions)) < 0) {
				close(elf_file);
				fprintf(stderr, "Wrong ELF file format");
				return err;
			}
		}
	}

	instructions += offset;
	int (*functionPtr)();
	functionPtr = (void *) instructions;

	int ret = functionPtr();

	close(elf_file);
	free(header);
	free(ph_table);
	instructions -= offset;
	free(instructions);

	return ret;
}
