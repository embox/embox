/**
 * @file
 * @brief
 *
 * @date 29.10.2012
 * @author Anton Bulychev
 */

#include <errno.h>
#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <lib/libelf.h>
#include <lib/libdl.h>

static dl_data data;

static inline int elf_load_in_mem(dl_element *element) {
	Elf32_Shdr *sh;
	Elf32_Addr addr;
	size_t size = 0;

	FILE *fd = element->fd;
	Elf32_Obj *obj = element->obj;

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

		fseek(fd, sh->sh_offset, 0);
		if (!sh->sh_size) {
			continue;
		}

		if (sh->sh_size != fread((void *) sh->sh_addr, sh->sh_size, 1, fd)) {
			return -EBADF;
		}

		addr += sh->sh_size;
	}

	return ENOERR;
}

#if 0
static int inline load_reloc_elf(const char *name, const int num) {
	Elf32_Obj *elf;
	FILE *elf_file = fopen(name, "r");
	int err;

	if (elf_file == NULL) {
		printf("Cannot open file %s\n", name);
		return -1;
	}

	data[num].fd = elf_file;
	elf = &data[num].obj;

	elf_initialize_object(elf);

	if ((err = elf_read_header(elf_file, elf)) < 0) {
		return err;
	}

	if ((err = elf_read_section_header_table(elf_file, elf)) < 0) {
		return err;
	}

	if ((err = elf_read_symbol_table(elf_file, elf)) < 0) {
		return err;
	}

	if ((err = elf_read_symbol_names(elf_file, elf)) < 0) {
		return err;
	}

	return ENOERR;
}

#endif

void finilize_reloc_elf(const int num) {
//	fclose(data[num].fd);
//	elf_finilize_object(&data[num].obj);
}

void finilize(void) {
//	finilize_reloc_elf(0);
//	finilize_reloc_elf(1);
}

int load(void) {
	dl_element *element;
	int err;

	dl_add_file(&data, "reloc1.o");
	dl_add_file(&data, "reloc2.o");

	if ((err = dl_fetch_global_symbols(&data)) < 0) {
		return err;
	}

	element = data.element_list;
	while (element) {
		if (element->obj->header->e_type == ET_REL) {
			elf_load_in_mem(element);
			elf_relocate(element->fd, element->obj, &data);
		} else {
			return -ENOSYS;
		}

		element = element->next;
	}

	return ENOERR;
}

int __link_elf(void) {
	int (*f_init)(void);
	dl_globsym *globsym;
	int err;

	if ((err = load()) < 0) {
		return err;
	}

	globsym = dl_find_global_symbol(&data, "init");
	f_init = (int (*)(void)) dl_get_global_symbol_addr(globsym);

	assert(f_init() == 3);

	finilize();

	return ENOERR;
}
