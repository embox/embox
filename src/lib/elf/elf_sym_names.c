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

int elf_read_symbol_names(Elf32_Obj *obj) {
	Elf32_Shdr *sh;

	if (!obj->sym_names_shidx) {
		return -ENOENT;
	}

	sh = &obj->sh_table[obj->sym_names_shidx];
	return elf_read_section(obj, sh, (char **)&obj->sym_names);
}
