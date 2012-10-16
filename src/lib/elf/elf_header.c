/**
 * @file
 * @brief
 *
 * @date 16.10.2012
 * @author Anton Bulychev
 */

#include <lib/libelf.h>
#include <string.h>
#include <stdlib.h>

static inline void elf_reverse_header(Elf32_Ehdr *header) {
	REVERSE_S(header->e_type);
	REVERSE_S(header->e_machine);
	REVERSE_L(header->e_version);
	REVERSE_L(header->e_entry);
	REVERSE_L(header->e_phoff);
	REVERSE_L(header->e_shoff);
	REVERSE_L(header->e_flags);
	REVERSE_S(header->e_ehsize);
	REVERSE_S(header->e_phentsize);
	REVERSE_S(header->e_phnum);
	REVERSE_S(header->e_shentsize);
	REVERSE_S(header->e_shnum);
	REVERSE_S(header->e_shstrndx);
}

int elf_read_header(FILE *fd, Elf32_Obj *obj) {
	obj->header = malloc(sizeof(Elf32_Ehdr));
	if (!fread(obj->header, sizeof(Elf32_Ehdr), 1, fd)) return -1;

	if ((obj->header)->e_ident[EI_MAG0] != ELFMAG0 ||
			(obj->header)->e_ident[EI_MAG1] != ELFMAG1 ||
			(obj->header)->e_ident[EI_MAG2] != ELFMAG2 ||
			(obj->header)->e_ident[EI_MAG3] != ELFMAG3) {
                return -1;
	}

	obj->need_reverse = NEED_REV(obj->header->e_ident[EI_DATA]);

	if (obj->need_reverse) {
		elf_reverse_header(obj->header);
	}

	return 1;
}
