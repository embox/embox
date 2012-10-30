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

static inline void elf_reverse_sym(Elf32_Sym *sym) {
	REVERSE_L(sym->st_name);
	REVERSE_L(sym->st_value);
	REVERSE_L(sym->st_size);
	REVERSE_S(sym->st_shndx);
}

int elf_read_symbol_table(Elf32_Obj *obj) {
	Elf32_Ehdr *header = obj->header;
	Elf32_Shdr *sh_table = obj->sh_table;
	int res;

	for (int i = 0; i < header->e_shnum; i++) {
		if (sh_table[i].sh_type == SHT_SYMTAB) {
			if ((res = elf_read_section(obj, &sh_table[i],
				(char **)&obj->sym_table)) < 0) {
				return res;
			}

			obj->sym_count = res / sizeof(Elf32_Sym);

			if (obj->need_reverse) {
				for (int i = 0; i < obj->sym_count; i++) {
					elf_reverse_sym(obj->sym_table + i);
				}
			}

			/* Save where names store */
			obj->sym_names_shidx = sh_table[i].sh_link;

			return obj->sym_count;
		}
	}

	return -ENOENT;
}
