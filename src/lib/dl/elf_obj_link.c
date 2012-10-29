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

Elf32_Data data[2];

static inline int elf_load_in_mem(Elf32_Data *data) {
	Elf32_Shdr *sh;
	Elf32_Addr addr;
	size_t size = 0;

	FILE *fd = data->fd;
	Elf32_Obj *obj = &data->obj;

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

void finilize_reloc_elf(const int num) {
	fclose(data[num].fd);
	elf_finilize_object(&data[num].obj);
}

void finilize(void) {
	finilize_reloc_elf(0);
	finilize_reloc_elf(1);
}

int load(void) {
	int err;
	Elf32_Globsym *globsym_table;

	load_reloc_elf("reloc1.o", 0);
	load_reloc_elf("reloc2.o", 1);

	if ((err = elf_load_in_mem(&data[0])) < 0) {
		return err;
	}

	if ((err = elf_load_in_mem(&data[1])) < 0) {
		return err;
	}

	if ((err = elf_fetch_global_symbols(data, &globsym_table))) {
		return err;
	}

	return ENOERR;
}

int __link_elf(void) {
	int err;

	if ((err = load()) < 0) {
		return err;
	}

	finilize();

	return ENOERR;
}
