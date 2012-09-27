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

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	FILE *elf_file;
	Elf32_Ehdr *elf_header = NULL;
	Elf32_Phdr *program_headers = NULL;

	elf_file = fopen(argv[argc - 1], "r");

	if (elf_file == NULL) {
		printf("Cannot open file %s\n", argv[argc - 1]);
		return -1;
	}

	if (elf_read_header(elf_file, &elf_header) < 0) {
		return -1;
	}

	if (elf_read_segments_table(elf_file, elf_header, &program_headers) < 0) {
		return -1;
	}

	elf_execve(elf_file, elf_header, program_headers);

	fclose(elf_file);

	// XXX: replace it?
	if (elf_header != NULL) free(elf_header);
	if (program_headers != NULL) free(program_headers);

	return 0;
}
