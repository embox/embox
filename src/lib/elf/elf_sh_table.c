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

static inline void elf_reverse_sh(Elf32_Shdr *sh) {
	REVERSE_L(sh->sh_name);
	REVERSE_L(sh->sh_type);
	REVERSE_L(sh->sh_flags);
	REVERSE_L(sh->sh_addr);
	REVERSE_L(sh->sh_offset);
	REVERSE_L(sh->sh_size);
	REVERSE_L(sh->sh_link);
	REVERSE_L(sh->sh_info);
	REVERSE_L(sh->sh_addralign);
	REVERSE_L(sh->sh_entsize);
}

int elf_read_section_header_table(FILE *fd, Elf32_Obj *obj) {
	Elf32_Ehdr *header = obj->header;

	if (header->e_shoff) {
		obj->sh_table = malloc(header->e_shentsize * header->e_shnum);

		fseek(fd, header->e_shoff, 0);

		if (fread(obj->sh_table, header->e_shentsize, header->e_shnum, fd)) {
			if (obj->need_reverse) {
				for (int i = 0; i < header->e_shnum; i++) {
					elf_reverse_sh(obj->sh_table + i);
				}
			}
			return 1;
		}
	}
	return -1;
}
