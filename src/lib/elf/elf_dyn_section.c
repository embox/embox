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

static inline void elf_reverse_dyn(Elf32_Dyn *dyn) {
	REVERSE_L(dyn->d_tag);
	REVERSE_L(dyn->d_un.d_val);
}

int elf_read_dynamic_section(FILE *fd, Elf32_Obj *obj) {
	size_t size;
	char *section_name;
	long offset;
	Elf32_Shdr *sh_table = obj->sh_table;

	if (!sh_table || !obj->string_table) {
		return -1;
	}

	for (int i = 0; i < obj->header->e_shnum; i++) {
		if (sh_table[i].sh_type != SHT_DYNAMIC) {
			continue;
		}

		section_name = ((char *)(obj->string_table + sh_table[i].sh_name));
		if (!strcmp(section_name, ".dynamic")) {
			offset = sh_table[i].sh_offset;
			size = sh_table[i].sh_size;
			obj->dyn_section = malloc(size);

			fseek(fd, offset, 0);

			if (fread(obj->dyn_section, size, 1, fd)) {
				obj->dyn_count = size / sizeof(Elf32_Dyn);

				if (obj->need_reverse) {
					for (int j = 0 ; j < obj->dyn_count; j++) {
						elf_reverse_dyn(obj->dyn_section + j);
					}
				}

				return obj->dyn_count;
			} else {
				return -1;
			}
		}
	}

	return -1;
}
