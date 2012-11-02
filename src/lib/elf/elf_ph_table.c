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

static inline void elf_reverse_ph(Elf32_Phdr *ph_table) {
	REVERSE_L(ph_table->p_type);
	REVERSE_L(ph_table->p_offset);
	REVERSE_L(ph_table->p_vaddr);
	REVERSE_L(ph_table->p_paddr);
	REVERSE_L(ph_table->p_filesz);
	REVERSE_L(ph_table->p_memsz);
	REVERSE_L(ph_table->p_flags);
	REVERSE_L(ph_table->p_align);
}

int elf_read_program_header_table(Elf32_Obj *obj) {
	Elf32_Ehdr *header = obj->header;
	size_t size;

	if (!header->e_phoff) {
		return -ENOENT;
	}

	size = header->e_phentsize * header->e_phnum;
	if (!(obj->ph_table = malloc(size))) {
		return -ENOMEM;
	}

	fseek(obj->fd, header->e_phoff, 0);
	if (size != fread(obj->ph_table, 1, size, obj->fd)) {
		return -EBADF;
	}

	if (obj->need_reverse) {
		for (int i = 0; i < header->e_phnum; i++) {
			elf_reverse_ph(obj->ph_table + i);
		}
	}

	return ENOERR;
}
