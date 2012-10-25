/**
 * @file
 * @brief
 *
 * @date 16.10.2012
 * @author Anton Bulychev
 */

#include <lib/libelf.h>
#include <string.h>
#include <errno.h>

int32_t elf_read_string_table(FILE *fd, Elf32_Obj *obj) {
	Elf32_Ehdr *header = obj->header;
	size_t idx;

	if (obj->sh_table && header->e_shstrndx) {
		idx = header->e_shstrndx;

		return elf_read_section(fd, &obj->sh_table[idx], &obj->string_table);
	}

	return -ENOENT;
}
