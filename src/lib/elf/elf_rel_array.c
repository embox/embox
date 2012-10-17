/**
 * @file
 * @brief
 *
 * @date 17.10.2012
 * @author Anton Bulychev
 */

#include <lib/libelf.h>
#include <string.h>
#include <stdlib.h>

static void inline elf_reverse_rel(Elf32_Rel *rel) {
	REVERSE_L(rel->r_offset);
	REVERSE_L(rel->r_info);
}

int elf_read_rel_table(FILE *fd, Elf32_Obj *obj) {
	size_t size = 0;
	long offset = 0;
	Elf32_Shdr *sh_table = obj->sh_table;

	if (!sh_table) {
		return -1;
	}

	for (int i = 0; i < obj->header->e_shnum; i++) {
		if (sh_table[i].sh_type == SHT_REL) {
			size += sh_table[i].sh_size;
		}
	}

	obj->rel_array = malloc(size);

	for (int i = 0; i < obj->header->e_shnum; i++) {
		if (sh_table[i].sh_type == SHT_REL) {
			fseek(fd, sh_table[i].sh_offset, 0);
			if (!fread(((void *) obj->rel_array) + offset, sh_table[i].sh_size, 1, fd)) {
				return -1;
			}
			offset += sh_table[i].sh_size;
		}
	}

	obj->rel_count = size / sizeof(Elf32_Rel);
	if (obj->need_reverse) {
		for (int i = 0; i < obj->rel_count; i++) {
			elf_reverse_rel(obj->rel_array + i);
		}
	}

	return obj->rel_count;
}
