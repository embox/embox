/**
 * @file
 * @brief
 *
 * @date 29.10.2012
 * @author Anton Bulychev
 */

#include <errno.h>

#include <lib/libelf.h>

static inline void elf_reverse_sym(Elf32_Sym *sym) {
	REVERSE_L(sym->st_name);
	REVERSE_L(sym->st_value);
	REVERSE_L(sym->st_size);
	REVERSE_S(sym->st_shndx);
}

int elf_read_symbols(Elf32_Obj *obj, unsigned int sh_idx,
		Elf32_Sym **sym_table, char **sym_names) {

	Elf32_Shdr *sh = &obj->sh_table[sh_idx];
	int size, count;

	if (sh->sh_type != SHT_SYMTAB && sh->sh_type != SHT_DYNSYM) {
		return -EINVAL;
	}

	if ((size = elf_read_section(obj, sh_idx, (char **)sym_table)) < 0) {
		return size;
	}

	count = size / sizeof(Elf32_Sym);
	if (obj->need_reverse) {
		for (int i = 0; i < count; i++) {
			elf_reverse_sym(&(*sym_table)[i]);
		}
	}

	if ((size = elf_read_section(obj, sh->sh_link, (char **)sym_names)) < 0) {
		return size;
	}

	return count;
}
