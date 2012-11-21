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

#include <kernel/task.h>
#include <mem/mmap.h>

int elfloader_place_relocatable(Elf32_Obj *obj) {
	Elf32_Shdr *sh;
	Elf32_Addr addr;
	size_t size = 0;
	struct marea *marea;
	int err;

	if ((err = elf_read_section_header_table(obj)) < 0) {
		return err;
	}

	for (int i = 0; i < obj->header->e_shnum; i++) {
		size += obj->sh_table[i].sh_size;
	}

	if (!(marea = mmap_alloc_marea(task_self()->mmap, size, 0))) {
		return -ENOMEM;
	}

	addr = marea_get_start(marea);

	// TODO: Replace it somewhere
	for (int i = 0; i < obj->header->e_shnum; i++) {
		sh = &obj->sh_table[i];

		sh->sh_addr = addr;

		fseek(obj->fd, sh->sh_offset, 0);

		if (sh->sh_size != fread((void *) sh->sh_addr, sh->sh_size, 1, obj->fd)) {
			return -EBADF;
		}

		addr += sh->sh_size;
	}

	return ENOERR;
}

/*
 * TODO: Rewrite it using program header table.
 */
int elfloader_place_shared(Elf32_Obj *obj) {
	Elf32_Shdr *sh;
	Elf32_Addr addr;
	size_t size = 0;
	struct marea *marea;
	int err;

	if ((err = elf_read_section_header_table(obj)) < 0) {
		return err;
	}

	for (int i = 0; i < obj->header->e_shnum; i++) {
		if (size < obj->sh_table[i].sh_addr + obj->sh_table[i].sh_size) {
			size = obj->sh_table[i].sh_addr + obj->sh_table[i].sh_size;
		}
	}

	if (!(marea = mmap_alloc_marea(task_self()->mmap, size, 0))) {
		return -ENOMEM;
	}

	addr = marea_get_start(marea);

	obj->load_offset = addr;

	for (int i = 0; i < obj->header->e_shnum; i++) {
		sh = &obj->sh_table[i];

		sh->sh_addr += obj->load_offset;

		if (!sh->sh_size) {
			continue;
		}

		// TODO: Replace it somewhere
		fseek(obj->fd, sh->sh_offset, 0);
		if (sh->sh_size != fread((void *) sh->sh_addr, sh->sh_size, 1, obj->fd)) {
			return -EBADF;
		}
	}

	return ENOERR;
}

int elfloader_place_executable(Elf32_Obj *obj) {
	Elf32_Phdr *ph;
	int err;

	if ((err = elf_read_program_header_table(obj)) < 0) {
		return err;
	}

	for (int i = 0; i < obj->header->e_phnum; i++) {
		ph = &obj->ph_table[i];

		if (!ph->p_memsz || ph->p_type != PT_LOAD) {
			continue;
		}

		if (!mmap_place_marea(task_self()->mmap, ph->p_vaddr, ph->p_vaddr + ph->p_memsz, 0)) {
			return -EFAULT;
		}

		// TODO: Replace it
		fseek(obj->fd, ph->p_offset, 0);
		if (ph->p_memsz != fread((void *) ph->p_vaddr, ph->p_memsz, 1, obj->fd)) {
			return -EBADF;
		}
	}

	return ENOERR;
}

