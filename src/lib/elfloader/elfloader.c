/**
 * @file
 * @brief
 *
 * @date 30.10.2012
 * @author Anton Bulychev
 */

#include <errno.h>
#include <assert.h>

#include <lib/libelf.h>
#include <lib/libdl.h>
#include <lib/elfloader.h>

static int elf_load_in_mem(Elf32_Obj *obj) {
	switch(obj->header->e_type) {
	case (ET_REL):
		return elfloader_load_relocatable(obj);
	case (ET_DYN):
		return elfloader_load_shared(obj);
	case (ET_EXEC):
		return elfloader_load_executable(obj);
	default:
		return -ENOSYS;
	}
}

int elfloader_load(Elf32_Objlist *list) {
	Elf32_Objlist_item *item = list->first;
	dl_data *data;
	dl_globsym *globsym;
	int (*f_init)(void);

	int err;

	while (item) {
		if ((err = elf_load_in_mem(item->obj)) < 0) {
			return err;
		}
		item = item->next;
	}

	dl_proceed(list, &data);

	globsym = dl_find_global_symbol(data, "func");
	f_init = (int (*)(void)) dl_get_global_symbol_addr(globsym);
	assert(f_init() == 3);

	dl_free_data(data);

	return ENOERR;
}
