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
	size_t size;
	char *section_name;
	long offset;
	Elf32_Shdr *sh_table = obj->sh_table;

	if (!obj->sh_table || !obj->string_table) {
		return -1;
	}

	for (int i = 0; i < obj->header->e_shnum; i++) {
		if (sh_table[i].sh_type != SHT_STRTAB) {
			continue;
		}

		section_name = ((char *)(obj->string_table + sh_table[i].sh_name));
		if (!strcmp(section_name, ".strtab")) {
			/*We found section with name .strtab and type SHT_STRTAB*/
			/*such strings ,must contain symbol names*/
			offset = sh_table[i].sh_offset;
			size = sh_table[i].sh_size;
			obj->sym_names = malloc(size);

			fseek(fd, offset, 0);

			return (!fread(obj->sym_names, size, 1, fd)) ? -1 : size;
		}
	}

	return -1;
}
