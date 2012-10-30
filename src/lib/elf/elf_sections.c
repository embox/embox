/**
 * @file
 * @brief
 *
 * @date 23.10.2012
 * @author Anton Bulychev
 */

#include <lib/libelf.h>
#include <stdlib.h>
#include <errno.h>

int elf_init_sections(Elf32_Obj *obj) {
	int shnum = obj->header->e_shnum;

	if (!(obj->sections = malloc(sizeof(char*) * shnum))) {
		return -ENOMEM;
	}

	for (int i = 0; i < obj->header->e_shnum; i++) {
		obj->sections[i] = NULL;
	}

	return ENOERR;
}

void elf_free_sections(Elf32_Obj *obj) {
	for (int i = 0; i < obj->header->e_shnum; i++) {
		if (obj->sections[i]) free(obj->sections[i]);
	}

	free(obj->sections);
}

void elf_free_section(Elf32_Obj *obj, char *section_ptr) {
	for (int i = 0; i < obj->header->e_shnum; i++) {
		if (obj->sections[i] == section_ptr) {
			free(obj->sections[i]);
			obj->sections[i] = NULL;
		}
	}

	free(obj->sections);
}

int elf_read_section(Elf32_Obj *obj, unsigned int sh_idx, char **dst) {
	Elf32_Shdr *sh	= &obj->sh_table[sh_idx];

	if (!(obj->sections[sh_idx])) {
		if (!(*dst = malloc(sh->sh_size))) {
			return -ENOMEM;
		}

		fseek(obj->fd, sh->sh_offset, 0);

		if (sh->sh_size != fread(*dst, sh->sh_size, 1, obj->fd)) {
			return -EBADF;
		}

		obj->sections[sh_idx] = *dst;
	}

	return sh->sh_size;
}
