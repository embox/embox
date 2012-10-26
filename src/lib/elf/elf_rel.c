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

int elf_read_rel_section(FILE *fd, Elf32_Obj *obj, Elf32_Shdr *sh, Elf32_Rel **rel) {
	size_t size = sh->sh_size;
	int count = 0;
	*rel = malloc(size);

	fseek(fd, sh->sh_offset, 0);
	if (!fread((void *) *rel, size, 1, fd)) {
		return -EBADF;
	}

	count = size / sizeof(Elf32_Rel);
	if (obj->need_reverse) {
		for (int i = 0; i < count; i++) {
			elf_reverse_rel(&(*rel[i]));
		}
	}

	return count;
}

void elf_remove_rel_section(Elf32_Rel *rel) {
	free(rel);
}
