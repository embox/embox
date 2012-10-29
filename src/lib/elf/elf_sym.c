/**
 * @file
 * @brief
 *
 * @date 29.10.2012
 * @author Anton Bulychev
 */

#include <lib/libelf.h>

Elf32_Addr elf_get_symbol_addr(Elf32_Obj *obj, Elf32_Sym *sym) {
	if (obj->header->e_type == ET_REL) {
		return obj->sh_table[sym->st_shndx].sh_addr + sym->st_value;
	} else {
		return sym->st_value;
	}
}
