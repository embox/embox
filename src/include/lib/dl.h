/**
 * @file
 * @brief
 *
 * @date 26.10.2012
 * @author Anton Bulychev
 */

#ifndef LIB_DL_H_
#define LIB_DL_H_

typedef struct {
	Elf32_Obj *obj;
	Elf32_Sym *sym;
} dl_globsym;

typedef struct dl_element_struct {
	Elf32_Obj                 obj;
	FILE                      *fd;
	struct dl_element_struct  *next;
} dl_element;

typedef struct {
	dl_element *element_list;

	dl_globsym *globsym_table;
	int globsym_count;
} dl_data;


extern int elf_relocate(FILE *fd, Elf32_Obj *obj);

extern int dl_fetch_global_symbols(dl_data *data);

#endif /* LIB_DL_H_ */
