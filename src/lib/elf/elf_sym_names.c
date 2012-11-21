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

int elf_read_symbol_names(FILE *fd, Elf32_Obj *obj) {
	Elf32_Shdr *sh_table = obj->sh_table;
	char *section_name;

	for (int i = 0; i < obj->header->e_shnum; i++) {
		if (sh_table[i].sh_type != SHT_STRTAB) {
			continue;
		}

		section_name = ((char *)(obj->string_table + sh_table[i].sh_name));
		if (!strcmp(section_name, ".strtab")) {
			/*We found section with name .strtab and type SHT_STRTAB*/
			/*such strings ,must contain symbol names*/

			return elf_read_section(fd, &sh_table[i],
					(char **)&obj->sym_names);
		}
	}

	return -ENOENT;
}
