/**
 * @file
 * @brief
 *
 * @date 26.10.2012
 * @author Anton Bulychev
 */

#ifndef LIB_DL_H_
#define LIB_DL_H_

#include <lib/libelf.h>

typedef struct {
	Elf32_Obj *obj;
	Elf32_Sym *sym;
} dl_globsym;

typedef struct {
	dl_globsym *globsym_table;
	int globsym_count;
} dl_data;

extern int dl_proceed(Elf32_Objlist *list, dl_data **data);
extern void dl_free_data(dl_data *data);

extern int dl_fetch_global_symbols(Elf32_Objlist *list, dl_data *data);
extern dl_globsym *dl_find_global_symbol(dl_data *data, const char *name);
extern dl_globsym *dl_find_global_symbol_outside(dl_data *data, Elf32_Obj *me, const char *name);
extern Elf32_Addr dl_get_global_symbol_addr(dl_globsym *globsym);

extern int dl_proceed_all_relocations(Elf32_Objlist *list, dl_data *data);

extern int dl_relocate_rel(dl_data *data, Elf32_Obj *obj, Elf32_Shdr *relsh,
		Elf32_Rel *rel, Elf32_Sym *sym, const char *sym_names);

#endif /* LIB_DL_H_ */
