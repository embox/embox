/**
 * @file
 * @brief
 *
 * @date 02.11.2012
 * @author Anton Bulychev
 */

#include <errno.h>
#include <string.h>

#include <lib/libelf.h>
#include <lib/libdl.h>

#include "dl_relocate.h"

int dl_relocate_rel(dl_data *data, Elf32_Obj *obj, Elf32_Shdr *relsh,
		Elf32_Rel *rel, Elf32_Sym *sym, const char *sym_names) {

	dl_globsym *globsym;
	Elf32_Addr addr;

	Elf32_Addr *where = (Elf32_Addr *) elf_get_rel_addr(obj, relsh, rel);

	/* Calculation of address */
	switch (ELF32_R_TYPE(rel->r_info)) {
	case R_386_NONE:
		return ENOERR;

	case R_386_32:
	case R_386_PC32:
	case R_386_GLOB_DAT:
	case R_386_JMP_SLOT:
		globsym = dl_find_global_symbol(data, sym_names + sym->st_name);
		addr = dl_get_global_symbol_addr(globsym);
		break;

	case R_386_COPY:
		globsym = dl_find_global_symbol_outside(data, obj, sym_names + sym->st_name);
		addr = dl_get_global_symbol_addr(globsym);
		if (sym->st_size != globsym->sym->st_size) {
			return -EINVAL;
		}
		break;

	case R_386_GOT32:
	case R_386_PLT32:
	case R_386_RELATIVE:
	case R_386_GOTOFF:
	case R_386_GOTPC:
		return -ENOSYS;

	default:
		return -EINVAL;
	}

	/* Apply relocation */
	switch(ELF32_R_TYPE(rel->r_info)) {
	case R_386_32:
		*where += addr;
		break;
	case R_386_PC32:
		*where += addr - (Elf32_Addr)where;
		break;
	case R_386_GLOB_DAT:
		*where = addr;
		break;
	case R_386_JMP_SLOT:
		*where = addr;
		break;
	case R_386_COPY:
		memcpy((void *)where, (void *) addr, sym->st_size);
		break;
	}

	return ENOERR;
}

int dl_relocate_rela(dl_data *data, Elf32_Obj *obj, Elf32_Shdr *relsh,
		Elf32_Rel *rel, Elf32_Sym *sym, const char *sym_names) {

	return -EINVAL;
}
