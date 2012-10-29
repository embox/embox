/**
 * @file
 * @brief
 *
 * @date 26.10.2012
 * @author Anton Bulychev
 */

#include <string.h>
#include <errno.h>
#include <types.h>

#include <lib/libelf.h>
#include <lib/dl.h>


/* TODO: Replace it in arch!!! */

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

int _global_outside = 3;

static inline int elf_relocate_section_rel(FILE *fd, Elf32_Obj *obj,
		Elf32_Shdr *relsh) {

	Elf32_Rel  *rel;
	Elf32_Sym  *sym;
	Elf32_Addr sym_addr;
	Elf32_Addr *where;
	Elf32_Shdr *sh_table = obj->sh_table;
	int rel_count = elf_read_rel_section(fd, obj, relsh, &rel);

	for (int i = 0 ; i < rel_count; i++) {
		where = (Elf32_Addr *)
				(sh_table[relsh->sh_info].sh_addr + rel[i].r_offset);
		sym = &obj->sym_table[ELF32_R_SYM(rel[i].r_info)];
		if (sym->st_shndx != SHN_UNDEF) {
			sym_addr = sym->st_value;
			if (obj->header->e_type == ET_REL) {
				sym_addr += sh_table[sym->st_shndx].sh_addr;
			}
		} else {
//			if (!(sym_addr = find_symbol_addr(obj->sym_names + sym->st_name))) {
//				return -ENOSYS;
//			}
			return -ENOSYS;
		}

		switch (ELF32_R_TYPE(rel[i].r_info)) {
		case R_386_NONE:
			break;
		case R_386_32:
			*where += sym_addr;
			break;
		case R_386_PC32:
			*where = sym_addr - (Elf32_Addr)where;
			break;
		default:
			return -ENOSYS;
		}
	}

	return ENOERR;
}

static inline int elf_relocate_section_rela(FILE *fd, Elf32_Obj *obj,
		Elf32_Shdr *relash) {
	/* Have not implemented yet */
	return -ENOSYS;
}

int elf_relocate(FILE *fd, Elf32_Obj *obj) {
	Elf32_Shdr *sh;
	int err;

	for (int i = 0; i < obj->header->e_shnum; i++) {
		sh = &obj->sh_table[i];

		switch (sh->sh_type) {
		case SHT_REL:
			if ((err = elf_relocate_section_rel(fd, obj, sh)) < 0) {
				return err;
			}
			break;
		case SHT_RELA:
			if ((err = elf_relocate_section_rela(fd, obj, sh)) < 0) {
				return err;
			}
			break;
		}
	}

	return ENOERR;
}
