/**
 * @file
 * @brief
 *
 * @date 17.10.2012
 * @author Anton Bulychev
 */

#include <lib/libelf.h>
#include <string.h>
#include <errno.h>

static inline void elf_reverse_dyn(Elf32_Dyn *dyn) {
	REVERSE_L(dyn->d_tag);
	REVERSE_L(dyn->d_un.d_val);
}

int elf_read_dynamic_section(FILE *fd, Elf32_Obj *obj) {
	Elf32_Shdr *sh_table = obj->sh_table;
	char *section_name;
	int res;

	if (!sh_table || !obj->string_table) {
		return -ENOENT;
	}

	for (int i = 0; i < obj->header->e_shnum; i++) {
		if (sh_table[i].sh_type != SHT_DYNAMIC) {
			continue;
		}

		section_name = ((char *)(obj->string_table + sh_table[i].sh_name));
		if (!strcmp(section_name, ".dynamic")) {
			if ((res = elf_read_section(fd, &sh_table[i],
					(char **)&obj->dyn_section)) < 0) {
				return res;
			}

			obj->dyn_count = res / sizeof(Elf32_Dyn);

			if (obj->need_reverse) {
				for (int j = 0 ; j < obj->dyn_count; j++) {
					elf_reverse_dyn(obj->dyn_section + j);
				}
			}

			return obj->dyn_count;
		}
	}

	return -ENOENT;
}
