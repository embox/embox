/**
 * @file
 * @brief
 *
 * @date 30.10.2012
 * @author Anton Bulychev
 */

#include <errno.h>
#include <stdlib.h>
#include <assert.h>

#include <lib/libelf.h>
#include <lib/libdl.h>
#include <lib/elfloader.h>

static int elf_load_relocatable(Elf32_Obj *obj) {
	Elf32_Shdr *sh;
	Elf32_Addr addr;
	size_t size = 0;
	int err;

	if ((err = elf_read_section_header_table(obj)) < 0) {
		return err;
	}

	for (int i = 0; i < obj->header->e_shnum; i++) {
		size += obj->sh_table[i].sh_size;
	}

	if (!(addr = (Elf32_Addr) malloc(size))) {
		return -ENOMEM;
	}

	// TODO: Replace it somewhere
	for (int i = 0; i < obj->header->e_shnum; i++) {
		sh = &obj->sh_table[i];

		sh->sh_addr = addr;

		fseek(obj->fd, sh->sh_offset, 0);
		if (!sh->sh_size) {
			continue;
		}

		if (sh->sh_size != fread((void *) sh->sh_addr, sh->sh_size, 1, obj->fd)) {
			return -EBADF;
		}

		addr += sh->sh_size;
	}

	return ENOERR;
}

static int elf_load_in_mem(Elf32_Obj *obj) {
	switch(obj->header->e_type) {
	case (ET_REL):
		return elf_load_relocatable(obj);
	case (ET_EXEC):
		return -ENOSYS;
	case (ET_DYN):
		return -ENOSYS;
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

	globsym = dl_find_global_symbol(data, "test_in_1");
	f_init = (int (*)(void)) dl_get_global_symbol_addr(globsym);

	assert(f_init() == 3);

	dl_free_data(data);

	return ENOERR;
}
