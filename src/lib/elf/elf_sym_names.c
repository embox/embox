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
	if (!obj->sym_names_shidx) {
		return -ENOENT;
	}

	return elf_read_section(obj, obj->sym_names_shidx,
			(char **)&obj->sym_names);
}
