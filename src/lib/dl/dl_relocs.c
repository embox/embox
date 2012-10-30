/**
 * @file
 * @brief
 *
 * @date 29.10.2012
 * @author Anton Bulychev
 */

#include <string.h>
#include <errno.h>
#include <types.h>

#include <lib/libelf.h>
#include <lib/libdl.h>


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

static inline int elf_relocate_section_rel(dl_data *data, Elf32_Obj *obj,
		unsigned int sh_idx) {

	Elf32_Rel  *rel;
	Elf32_Sym  *sym;
	Elf32_Addr sym_addr;
	Elf32_Addr *where;
	Elf32_Shdr *sh_table = obj->sh_table;
	dl_globsym *globsym;
	Elf32_Shdr *relsh = &obj->sh_table[sh_idx];

	Elf32_Sym *sym_table;
	char      *sym_names;

	int err;

	int rel_count = elf_read_rel_section(obj, sh_idx, &rel);

	if ((err = elf_read_symbols(obj, relsh->sh_link,
			&sym_table, &sym_names)) < 0) {
		return err;
	}

	for (int i = 0 ; i < rel_count; i++) {
		where = (Elf32_Addr *)
				(sh_table[relsh->sh_info].sh_addr + rel[i].r_offset);

		sym = &sym_table[ELF32_R_SYM(rel[i].r_info)];
		if (sym->st_shndx != SHN_UNDEF) {
			sym_addr = elf_get_symbol_addr(obj, sym);
		} else {
			globsym = dl_find_global_symbol(data, sym_names + sym->st_name);
			if (globsym == NULL) {
				return -ENOENT;
			}
			sym_addr = elf_get_symbol_addr(globsym->obj, globsym->sym);
		}

		// Should be apply_relocate in arch
		switch (ELF32_R_TYPE(rel[i].r_info)) {
		case R_386_NONE:
			break;
		case R_386_32:
			*where += sym_addr;
			break;
		case R_386_PC32:
			*where += sym_addr - (Elf32_Addr)where;
			break;
		case R_386_GLOB_DAT:
			*where = sym_addr;
			break;
		default:
			return -ENOSYS;
		}
	}

	return ENOERR;
}

static inline int elf_relocate_section_rela(dl_data *data, Elf32_Obj *obj,
		unsigned int sh_idx) {
	/* Have not implemented yet */
	return -ENOSYS;
}

int dl_proceed_relocations(dl_data *data, Elf32_Obj *obj) {
	Elf32_Shdr *sh;
	int err;

	for (int i = 0; i < obj->header->e_shnum; i++) {
		sh = &obj->sh_table[i];

		switch (sh->sh_type) {
		case SHT_REL:
			if ((err = elf_relocate_section_rel(data, obj, i)) < 0) {
				return err;
			}
			break;
		case SHT_RELA:
			if ((err = elf_relocate_section_rela(data, obj, i)) < 0) {
				return err;
			}
			break;
		}
	}

	return ENOERR;
}

int dl_proceed_all_relocations(Elf32_Objlist *list, dl_data *data) {
	Elf32_Objlist_item *item = list->first;
	int err;

	while(item) {
		if ((err = dl_proceed_relocations(data, item->obj)) < 0) {
			return err;
		}
		item = item->next;
	}

	return ENOERR;
}

