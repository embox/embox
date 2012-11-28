/**
 * @file
 * @brief
 *
 * @date 29.10.2012
 * @author Anton Bulychev
 */

#include <lib/libelf.h>

Elf32_Addr elf_get_symbol_addr(Elf32_Obj *obj, Elf32_Sym *sym) {
	switch (obj->header->e_type) {
	case(ET_REL):
		return obj->sh_table[sym->st_shndx].sh_addr + sym->st_value;
	case(ET_DYN):
		return obj->base_addr + sym->st_value;
	case(ET_EXEC):
		return sym->st_value;
	default:
		return 0;
	}
}
