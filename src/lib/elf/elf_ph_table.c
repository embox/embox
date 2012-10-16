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

void elf_reverse_ph(Elf32_Phdr *ph_table) {
	REVERSE_L(ph_table->p_type);
	REVERSE_L(ph_table->p_offset);
	REVERSE_L(ph_table->p_vaddr);
	REVERSE_L(ph_table->p_paddr);
	REVERSE_L(ph_table->p_filesz);
	REVERSE_L(ph_table->p_memsz);
	REVERSE_L(ph_table->p_flags);
	REVERSE_L(ph_table->p_align);
}

int elf_read_program_header_table(FILE *fd, Elf32_Obj *obj) {
	Elf32_Ehdr *header = obj->header;

	if (header->e_phoff) {
		obj->ph_table = malloc(header->e_phentsize * header->e_phnum);

		fseek(fd, header->e_phoff, 0);

		if (fread(obj->ph_table, header->e_phentsize, header->e_phnum, fd)) {
			if (obj->need_reverse) {
				for (int i = 0; i < header->e_phnum; i++) {
					elf_reverse_ph(obj->ph_table + i);
				}
			}
			return 1;
		}
	}

	return -1;
}
