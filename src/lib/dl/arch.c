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

#define R_386_NONE      0
#define R_386_32        1
#define R_386_PC32      2
#define R_386_GOT32     3
#define R_386_PLT32     4
#define R_386_COPY      5
#define R_386_GLOB_DAT  6
#define R_386_JMP_SLOT  7
#define R_386_RELATIVE  8
#define R_386_GOTOFF    9
#define R_386_GOTPC     10

int dl_relocate_rel(dl_data *data, Elf32_Obj *obj, Elf32_Shdr *relsh,
		Elf32_Rel *rel, Elf32_Sym *sym, const char *sym_names) {

	dl_globsym *globsym;
	Elf32_Addr addr;

	Elf32_Addr *where = (Elf32_Addr *) elf_get_rel_addr(obj, relsh, rel);

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
	default:
		return -ENOSYS;
	}

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
