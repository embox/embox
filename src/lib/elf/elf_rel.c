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

int elf_read_rel_section(Elf32_Obj *obj, unsigned int sh_idx, Elf32_Rel **rel) {
	size_t size;
	int count = 0;

	if ((size = elf_read_section(obj, sh_idx, (char **)rel)) < 0) {
		return size;
	}

	count = size / sizeof(Elf32_Rel);
	if (obj->need_reverse) {
		for (int i = 0; i < count; i++) {
			elf_reverse_rel(&(*rel[i]));
		}
	}

	return count;
}
