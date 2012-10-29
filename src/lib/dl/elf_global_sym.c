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

#include <stdlib.h>

#include <lib/libelf.h>
#include <lib/dl.h>

int elf_fetch_global_symbols(Elf32_Data *data, Elf32_Globsym **globsym_table) {
	Elf32_Obj *obj;
	Elf32_Sym *sym;
	int count = 0, p = 0;

	for (int i = 0; i < 2; i++) {
		obj = &data[i].obj;
		for (int j = 0; j < obj->sym_count; j++) {
			sym = &obj->sym_table[j];

			if ((ELF32_ST_BIND(sym->st_info) == STB_GLOBAL)
				&& (sym->st_shndx != SHN_UNDEF)) {
				count++;
			}
		}
	}

	*globsym_table = malloc(sizeof(Elf32_Globsym) * count);

	for (int i = 0; i < 2; i++) {
		obj = &data[i].obj;
		for (int j = 0; j < obj->sym_count; j++) {
			sym = &obj->sym_table[j];

			if ((ELF32_ST_BIND(sym->st_info) == STB_GLOBAL)
				&& (sym->st_shndx != SHN_UNDEF)) {
				*globsym_table[p++] = (Elf32_Globsym) {
						.obj = obj,
						.sym = sym,
				};
			}
		}
	}

	return count;
}

Elf32_Globsym *elf_find_global_symbol(const char *name) {
	for (int i = 0; i < kernel.sym_count; i++) {
		sym = &kernel.sym_table[i];
		// FIXME:
		if ((ELF32_ST_BIND(sym->st_info) == STB_GLOBAL)
			&& (0 == strcmp(name, kernel.sym_names + sym->st_name))) {
			// FIXME: if relocatable object - another address
			return sym->st_value;
		}
	}
}

void finilize_global_symbols(Elf32_Globsym *globsym_table) {
	free(globsym_table);
}

