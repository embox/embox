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

static inline int elf_relocate_section_rel(dl_data *data, Elf32_Obj *obj,
		unsigned int sh_idx) {

	Elf32_Rel  *rel;
	Elf32_Sym  *sym;
	Elf32_Shdr *relsh = &obj->sh_table[sh_idx];
	Elf32_Sym  *sym_table;
	char       *sym_names;
	int        err;
	int        rel_count;

	if ((rel_count = elf_read_rel_section(obj, sh_idx, &rel)) < 0) {
		return rel_count;
	}

	if ((err = elf_read_symbols(obj, relsh->sh_link,
			&sym_table, &sym_names)) < 0) {
		return err;
	}

	for (int i = 0 ; i < rel_count; i++) {
		sym = &sym_table[ELF32_R_SYM(rel[i].r_info)];

		if ((err = dl_relocate_rel(data, obj, relsh, &rel[i],
				sym, sym_names)) < 0) {
			return err;
		}
	}

	return ENOERR;
}

static inline int elf_relocate_section_rela(dl_data *data, Elf32_Obj *obj,
		unsigned int sh_idx) {
	/* Have not implemented yet */
	return -ENOSYS;
}

int dl_proceed_obj_relocations(dl_data *data, Elf32_Obj *obj) {
	Elf32_Shdr *sh;
	int err;

	if ((err = elf_read_section_header_table(obj)) < 0) {
		return err;
	}

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
		if ((err = dl_proceed_obj_relocations(data, item->obj)) < 0) {
			return err;
		}
		item = item->next;
	}

	return ENOERR;
}

