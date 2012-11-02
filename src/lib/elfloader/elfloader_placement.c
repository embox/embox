/**
 * @file
 * @brief
 *
 * @date 30.10.2012
 * @author Anton Bulychev
 */

#include <errno.h>
#include <stdlib.h>

#include <lib/libelf.h>
#include <lib/libdl.h>
#include <lib/elfloader.h>

int elfloader_place_relocatable(Elf32_Obj *obj) {
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

int elfloader_place_shared(Elf32_Obj *obj) {
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

	obj->load_offset = addr;

	// TODO: Replace it somewhere
	for (int i = 0; i < obj->header->e_shnum; i++) {
		sh = &obj->sh_table[i];

		sh->sh_addr += obj->load_offset;

		fseek(obj->fd, sh->sh_offset, 0);
		if (!sh->sh_size) {
			continue;
		}

		if (sh->sh_size != fread((void *) sh->sh_addr, sh->sh_size, 1, obj->fd)) {
			return -EBADF;
		}
	}

	return ENOERR;
}

int elfloader_place_executable(Elf32_Obj *obj) {
	Elf32_Shdr *sh;
	int err;

	if ((err = elf_read_section_header_table(obj)) < 0) {
		return err;
	}

#if 0
	for (int i = 0; i < obj->header->e_shnum; i++) {
		size += obj->sh_table[i].sh_size;
	}

	if (!(addr = (Elf32_Addr) malloc(size))) {
		return -ENOMEM;
	}
#endif

	// TODO: Replace it somewhere
	for (int i = 0; i < obj->header->e_shnum; i++) {
		sh = &obj->sh_table[i];

		if (!sh->sh_size || !sh->sh_addr) {
			continue;
		}

		fseek(obj->fd, sh->sh_offset, 0);
		if (sh->sh_size != fread((void *) sh->sh_addr, sh->sh_size, 1, obj->fd)) {
			return -EBADF;
		}
	}

	return ENOERR;
}

