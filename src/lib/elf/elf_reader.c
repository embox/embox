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

int32_t elf_read_header(FILE *fd, Elf32_Ehdr *head) {
	size_t size;
	size = fread(head, sizeof(Elf32_Ehdr), 1, fd);
	if (head->e_ident[EI_MAG0] != ELFMAG0 ||
            head->e_ident[EI_MAG1] != ELFMAG1 ||
            head->e_ident[EI_MAG2] != ELFMAG2 ||
            head->e_ident[EI_MAG3] != ELFMAG3) {
                TRACE("Not an ELF file: wrong magic bytes at the start\n");
                return -1;
        }
        return size;
}

int32_t elf_read_sections_table(FILE *fd, Elf32_Ehdr *head,
				Elf32_Shdr *sh_table) {
	size_t size, nmemb;
	uint8_t rev = head->e_ident[EI_DATA];
	long offset = L_REV(head->e_shoff, rev);
	if (offset) {
		fseek(fd, offset, 0);
		size  = S_REV(head->e_shentsize, rev);
		nmemb = S_REV(head->e_shnum, rev);
		return fread(sh_table, size, nmemb, fd);
	} else {/*Table doesn't exist*/
		return -1;
	}
}

int32_t elf_read_segments_table(FILE *fd, Elf32_Ehdr *head,
	                        Elf32_Phdr *st_table) {
	size_t size, nmemb;
	uint8_t rev = head->e_ident[EI_DATA];
	long offset = L_REV(head->e_phoff, rev);
	if (offset) {
		fseek(fd, offset, 0);
		size  = S_REV(head->e_shentsize, rev);
		nmemb = S_REV(head->e_shnum, rev);
		return fread(st_table, size, nmemb, fd);
	} else {/*Table doesn't exist*/
		return -1;
	}
}

int32_t elf_read_string_table(FILE *fd, Elf32_Ehdr *head,
		Elf32_Shdr *sh_table, int8_t *string_table) {
	size_t idx, size;
	long offset;
	uint8_t rev = head->e_ident[EI_DATA];
	if (L_REV(head->e_shoff, rev) == 0) {
		return -2;
	}
	idx = S_REV(head->e_shstrndx, rev);
	if (idx) {
		offset = L_REV(sh_table[idx].sh_offset, rev);
		fseek(fd, offset, 0);
		size = L_REV(sh_table[idx].sh_size, rev);
		return fread(string_table, size, 1, fd);
	} else {/*Not found*/
		return -1;
	}
}

int32_t elf_read_symbol_table(FILE *fd, Elf32_Ehdr *hdr,
	                      Elf32_Shdr *section_header_table,
	                      Elf32_Sym *symbol_table, int32_t *count) {
	size_t size, i;
	long offset;
	uint8_t rev = hdr->e_ident[EI_DATA];
	if (L_REV(hdr->e_shoff, rev) == 0) {
		return -2;
	}

	*count = 0;

	for (i = 0; i < S_REV(hdr->e_shnum, rev); i++) {
		if (L_REV(section_header_table[i].sh_type, rev) == SHT_SYMTAB) {
			offset = L_REV(section_header_table[i].sh_offset, rev);
			fseek(fd, offset, 0);
			size = L_REV(section_header_table[i].sh_size, rev);
			fread(symbol_table, size, 1, fd);
			*count = L_REV(section_header_table[i].sh_size, rev)
							    / sizeof(Elf32_Sym);
			return *count;
		}
	}
	return (*count == 0) ? -1 : -3;
}

int32_t elf_read_rel_table(FILE *fd, Elf32_Ehdr *hdr,
	                   Elf32_Shdr *section_hdr_table,
	                   Elf32_Rel *rel_table, int32_t *count) {
	size_t size, i;
	long offset;
	uint8_t rev = hdr->e_ident[EI_DATA];
	if (L_REV(hdr->e_shoff, rev) == 0) {
		return -2;
	}
	*count = 0;

	for (i = 0; i < S_REV(hdr->e_shnum, rev) ; i++) {
		if (L_REV(section_hdr_table[i].sh_type, rev) == SHT_REL) {
			offset = L_REV(section_hdr_table[i].sh_offset, rev);
			fseek(fd, offset, 0);
			size = L_REV(section_hdr_table[i].sh_size, rev);
			fread(rel_table + *count, size, 1, fd);
			*count += L_REV(section_hdr_table[i].sh_size, rev)
							/ sizeof(Elf32_Rel);
		}
	}
	return *count;
}

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
	int32_t i = index;

	do {
		name[i - index] = names_array[i];
		i++;
	} while ((names_array[i - 1] != '\0') && (i < MAX_NAME_LENGTH));

	return (i == MAX_NAME_LENGTH) ? 0 : i;
}

int32_t elf_read_symbol_string_table(FILE *fd, Elf32_Ehdr *hdr,
			Elf32_Shdr *section_hdr_table, int8_t *sections_names,
	                             int8_t *symb_names, int32_t *ret_length) {
	size_t size, i, length;
	int8_t section_name[100];
	long offset;
	uint8_t rev = hdr->e_ident[EI_DATA];

	if (sections_names == 0) {
		return -3;
	}

	if (L_REV(hdr->e_shoff, rev) == 0) {
		/*empty table*/
		return -2;
	}

	*ret_length = 0;

	for (i = 0; i < S_REV(hdr->e_shnum, rev) ; i++) {
		if (L_REV(section_hdr_table[i].sh_type, rev) != SHT_STRTAB) {
			continue;
		}
		length = read_name(sections_names ,L_REV(
		                           section_hdr_table[i].sh_name,
		                           rev), section_name);

		if (length && strstr((const char*) section_name, ".strtab")) {
			/*We found section with name .strtab and type SHT_STRTAB*/
			/*such strings ,must contain symbol names*/
			offset = L_REV(section_hdr_table[i].sh_offset, rev);
			fseek(fd, offset, 0);

			size = L_REV(section_hdr_table[i].sh_size, rev);
			fread(symb_names, size, 1, fd);
			*ret_length = L_REV(section_hdr_table[i].sh_size, rev);
			return *ret_length;
		}
	}
	return -1;
}

