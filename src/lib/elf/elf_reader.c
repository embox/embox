/**
 * @file
 * @brief ELF (Executable and Linkable Format) parser.
 *
 * @date 27.04.10
 * @author Aleksandr Kirov
 *         - Initial implementation
 * @author Nikolay Korotky
 *         - Refactoring, fix bugs.
 */

#include <lib/libelf.h>
#include <string.h>
#include <stdlib.h>


#if 0
int32_t elf_read_rela_table(FILE *fd, Elf32_Ehdr *hdr,
	                    Elf32_Shdr *section_hdr_table,
	                    Elf32_Rela *rela_table, int32_t *count) {
	size_t size, i;
	long offset;
	uint8_t rev = hdr->e_ident[EI_DATA];
	if (L_REV(hdr->e_shoff, rev) == 0) {
		return -2;
	}
	*count = 0;

	for (i = 0; i < S_REV(hdr->e_shnum, rev) ; i++) {
		if (L_REV(section_hdr_table[i].sh_type, rev) == SHT_RELA) {
			offset = L_REV(section_hdr_table[i].sh_offset, rev);
			fseek(fd, offset, 0);
			size = L_REV(section_hdr_table[i].sh_size, rev);
			fread(rela_table + *count, size, 1, fd);
			*count += L_REV(section_hdr_table[i].sh_size, rev)
							/ sizeof(Elf32_Rela);
		}
	}
	return *count;
}

int32_t read_name(int8_t *names_array, int32_t index, int8_t *name) {
	int32_t i = 0;

	do {
		name[index - i] = names_array[i];
		i++;
	} while ((names_array[i - 1] != '\0') && ((i - index) < MAX_NAME_LENGTH));

	name[i] = '\0';

	return (i == MAX_NAME_LENGTH) ? 0 : i;
}

#endif

int32_t elf_read_segment(FILE *fd, Elf32_Phdr *program_header, int8_t *dst) {
	size_t size;
	long offset;

	offset = program_header->p_offset;
	size = program_header->p_memsz;

	if (size) {
		fseek(fd, offset, 0);
		return (fread(dst, size, 1, fd)) ? 1 : -1;
	} else {
		/* Empty segment. */
		return 0;
	}
}
