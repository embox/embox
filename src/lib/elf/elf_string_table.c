/**
 * @file
 * @brief
 *
 * @date 16.10.2012
 * @author Anton Bulychev
 */

#include <lib/libelf.h>
#include <string.h>
#include <stdlib.h>

int32_t elf_read_string_table(FILE *fd, Elf32_Obj *obj) {
	Elf32_Ehdr *header = obj->header;
	size_t idx, offset, size;

	if (obj->sh_table && header->e_shstrndx) {
		idx = header->e_shstrndx;
		offset = obj->sh_table[idx].sh_offset;
		size = obj->sh_table[idx].sh_size;
		obj->string_table = malloc(size);

		fseek(fd, offset, 0);
		return (fread(obj->string_table, size, 1, fd)) ? 1 : -1;
	}
	return -1;
}
