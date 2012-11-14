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

#include <stdlib.h>

#include <lib/libelf.h>
#include <lib/libdl.h>

int dl_fetch_global_symbols(Elf32_Objlist *list, dl_data *data) {
	Elf32_Objlist_item *item = list->first;
	Elf32_Obj *obj;
	Elf32_Sym *sym;
	int count = 0, p = 0;

	while (item) {
		obj = item->obj;

		elf_read_section_header_table(obj);
		elf_read_symbol_table(obj);
		elf_read_symbol_names(obj);

		for (int j = 0; j < obj->sym_count; j++) {
			sym = &obj->sym_table[j];

			if ((ELF32_ST_BIND(sym->st_info) == STB_GLOBAL)
				&& (sym->st_shndx != SHN_UNDEF)) {
				count++;
			}
		}

		item = item->next;
	}

	data->globsym_count = count;
	if (!(data->globsym_table = malloc(sizeof(dl_globsym) * count))) {
		return -ENOMEM;
	}


	item = list->first;
	while (item) {
		obj = item->obj;
		for (int j = 0; j < obj->sym_count; j++) {
			sym = &obj->sym_table[j];

			if ((ELF32_ST_BIND(sym->st_info) == STB_GLOBAL)
				&& (sym->st_shndx != SHN_UNDEF)) {
				data->globsym_table[p++] = (dl_globsym) {
						.obj = obj,
						.sym = sym,
				};
			}
		}

		item = item->next;
	}

	return count;
}

dl_globsym *dl_find_global_symbol(dl_data *data, const char *name) {
	Elf32_Obj *obj;
	Elf32_Sym *sym;

	for (int i = 0; i < data->globsym_count; i++) {
		obj = data->globsym_table[i].obj;
		sym = data->globsym_table[i].sym;

		// FIXME:
		if ((ELF32_ST_BIND(sym->st_info) == STB_GLOBAL)
			&& (0 == strcmp(name, obj->sym_names + sym->st_name))) {
			return &data->globsym_table[i];
		}
	}

	return NULL;
}

dl_globsym *dl_find_global_symbol_outside(dl_data *data, Elf32_Obj *me, const char *name) {
	Elf32_Obj *obj;
	Elf32_Sym *sym;

	for (int i = 0; i < data->globsym_count; i++) {
		obj = data->globsym_table[i].obj;
		sym = data->globsym_table[i].sym;

		if (obj == me) {
			continue;
		}

		// FIXME:
		if ((ELF32_ST_BIND(sym->st_info) == STB_GLOBAL)
			&& (0 == strcmp(name, obj->sym_names + sym->st_name))) {
			return &data->globsym_table[i];
		}
	}

	return NULL;
}


Elf32_Addr dl_get_global_symbol_addr(dl_globsym *globsym) {
	return elf_get_symbol_addr(globsym->obj, globsym->sym);
}
