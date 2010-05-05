
/**
 * @date Apr 27, 2010
 * @author Aleksandr Kirov
 */

#include "elf_reader.h"
#include <string.h>

uint32_t elf_reverse_long(uint32_t num, uint8_t reversed) {
	switch (reversed) {
		case 0 : return(num);break;
		case 1 : return(num);break;
		case 2 : return(((num & 0x000000ff) << 24)
					  + ((num & 0x0000ff00) << 8)
					  + ((num & 0x00ff0000) >> 8)
				      + ((num & 0xff000000) >> 24)
					   );
				 break;
	}
	return num;
}

uint16_t elf_reverse_short(uint16_t num, uint8_t reversed) {
	switch (reversed) {
		case 0 : return(num);break;
		case 1 : return(num);break;
		case 2 : return(((num & 0x00ff ) << 8) + ((num & 0xff00) >> 8));break;
	}
	return num;
}

int32_t elf_read_header(FILE * fo, Elf32_Ehdr * header) {
	if (fo != NULL) {
		fread(header, sizeof(Elf32_Ehdr), 1, fo);
		return 0;
	}
	else {
		return 1;
	}
}

int32_t elf_read_sections_table(FILE * fo, Elf32_Ehdr * header,
							Elf32_Shdr * section_header_table) {
	if (elf_reverse_long(header->e_shoff, header->e_ident[5]) != 0) {
		fseek(fo, elf_reverse_long(header->e_phoff, header->e_ident[5]), 0);
		fread(&section_header_table,
			  elf_reverse_short(header->e_shentsize,header->e_ident[5]),
			  elf_reverse_short(header->e_shnum,header->e_ident[5]), fo);
		return 0;
	}
	else {/*Table doesn't exist*/
		return 1;
	}
}

int32_t elf_read_segments_table(FILE * fo, Elf32_Ehdr * header,
							Elf32_Phdr * segment_header_table) {
	if (elf_reverse_long(header->e_phoff, header->e_ident[5]) != 0) {
		fseek(fo, elf_reverse_long(header->e_phoff, header->e_ident[5]), 0);
		fread(segment_header_table, elf_reverse_short(header->e_phentsize,
													  header->e_ident[5]),
			  elf_reverse_short(header->e_phnum,header->e_ident[5]), fo);
		return 0;
	}
	else {/*Table doesn't exist*/
		return 1;
	}
}

int32_t elf_read_string_table(FILE * fo, Elf32_Ehdr * header,
						  Elf32_Shdr * section_header_table,
						  int8_t * names, int32_t * length) {
	if (elf_reverse_long(header->e_shoff,header->e_ident[5]) != 0) {
		if ( header->e_shstrndx != 0 ) {
			fseek(fo, elf_reverse_long(section_header_table[elf_reverse_short
				  (header->e_shstrndx, header->e_ident[5])].sh_offset,
				  header->e_ident[5]), 0);
			fread(names,elf_reverse_long(section_header_table[elf_reverse_short
				  (header->e_shstrndx, header->e_ident[5])].sh_size,
				  header->e_ident[5]), 1, fo);
			*length = elf_reverse_long(section_header_table[elf_reverse_short
					  (header->e_shstrndx, header->e_ident[5])].sh_size,
					  header->e_ident[5]);
			return 0;
		}
		else {/*Not found*/
			return 1;
		}
	}
	else {/*Empty table*/
		return 2;
	}
}

int32_t elf_read_symbol_table(FILE * fo, Elf32_Ehdr * header,
						  Elf32_Shdr * section_header_table,
						  Elf32_Sym * symbol_table, int32_t * count) {
	if (elf_reverse_long(header->e_shoff,header->e_ident[5]) != 0) {
		int32_t i=0;
		*count = 0;

		for (;i < elf_reverse_short(header->e_shnum,header->e_ident[5]) ;i++) {
			/*We find SHT_SYMTAB - sh_type with value - 2*/
			if (elf_reverse_long(section_header_table[i].sh_type,
				header->e_ident[5]) == 2) {
				fseek(fo, elf_reverse_long(section_header_table[i].sh_offset,
					  header->e_ident[5]), 0);
				fread(symbol_table, elf_reverse_long(
					  section_header_table[i].sh_size, header->e_ident[5]),
					  1, fo);
				*count = elf_reverse_long(section_header_table[i].sh_size,
						 header->e_ident[5]) / sizeof(Elf32_Sym);
				return 0;
			}
		}
		if (*count == 0) { /*nothing found*/
			return 1;
		}
	}
	else { /*empty table*/
		return 2;
	}
	return 3;
}

int32_t elf_read_rel_table(FILE * fo, Elf32_Ehdr * header,
				       Elf32_Shdr * section_header_table,
					   Elf32_Rel * rel_table, int32_t * count) {
	if (elf_reverse_long(header->e_shoff,header->e_ident[5]) != 0) {
		int32_t i=0;
		*count = 0;

		for (;i < elf_reverse_short(header->e_shnum,header->e_ident[5]) ;i++) {
			/*We find SHT_REL - sh_type with value - 9*/
			if (elf_reverse_long(section_header_table[i].sh_type,
				header->e_ident[5]) == 9) {
				fseek(fo, elf_reverse_long(section_header_table[i].sh_offset,
					  header->e_ident[5]), 0);
				fread(rel_table + *count,
					  elf_reverse_long(section_header_table[i].sh_size,
					  header->e_ident[5]), 1, fo);
				*count += elf_reverse_long(section_header_table[i].sh_size,
						  header->e_ident[5]) / sizeof(Elf32_Rel);
			}
		}

		if (*count != 0) {
			return 0;
		}
		else {/*Nothing found*/
			return 1;
		}
	}
	else {/*Section header table is empty*/
		return 2;
	}
}

int32_t elf_read_rela_table(FILE * fo, Elf32_Ehdr * header,
				       Elf32_Shdr * section_header_table,
					   Elf32_Rela * rela_table, int32_t * count) {
	if (elf_reverse_long(header->e_shoff,header->e_ident[5]) != 0) {
		int32_t i=0;
		*count = 0;

		for (;i < elf_reverse_short(header->e_shnum,header->e_ident[5]) ;i++) {
			/*We find SHT_RELA - sh_type with value - 4*/
			if (elf_reverse_long(section_header_table[i].sh_type,
				header->e_ident[5]) == 4) {
				fseek(fo, elf_reverse_long(section_header_table[i].sh_offset,
					  header->e_ident[5]), 0);
				fread(rela_table + *count,
					  elf_reverse_long(section_header_table[i].sh_size,
					  header->e_ident[5]), 1, fo);
				*count += elf_reverse_long(section_header_table[i].sh_size,
						  header->e_ident[5]) / sizeof(Elf32_Rela);
			}
		}

		if (*count != 0) {
			return 0;
		}
		else {/*Nothing found*/
			return 1;
		}
	}
	else {/*Section header table is empty*/
		return 2;
	}
}

int32_t read_name(int8_t * names_array, int32_t index, int8_t * name) {
	int32_t i = index;

	do {
		name[i-index] = names_array[i];
		i++;
	} while ((names_array[i-1] != '/0') &&
			 (i < MAX_NAME_LENGTH));

	if ( i == MAX_NAME_LENGTH ) {
		return 0;
	}
	else {
		return i;
	}
}

int32_t elf_read_symbol_string_table(FILE * fo, Elf32_Ehdr * header,
								 Elf32_Shdr * section_header_table,
								 int8_t * sections_names,
								 int8_t * names, int32_t * ret_length) {
	if(sections_names == 0) {
		return 3;
	}

	if (elf_reverse_long(header->e_shoff,header->e_ident[5]) != 0) {
		int32_t i=0;
		*ret_length = 0;

		for (;i < elf_reverse_short(header->e_shnum,header->e_ident[5]) ;i++) {
			/*We find SHT_STRTAB - sh_type with value - 3*/
			if (elf_reverse_long(section_header_table[i].sh_type,
				header->e_ident[5]) == 2 ) {
				int16_t section_name[100];

				int32_t length = read_name(sections_names ,elf_reverse_long(
						section_header_table[i].sh_name,
						header->e_ident[5]), section_name);

				if((length != 0) && (strstr(section_name,".strtab") > 0)) {
					/*We found section with name .strtab and type SHT_STRTAB*/
					/*such strings ,must contain symbol names*/

					fseek(fo, elf_reverse_long(section_header_table[i].sh_offset,
						  header->e_ident[5]), 0);

					fread(names, elf_reverse_long(
						  section_header_table[i].sh_size, header->e_ident[5]),
						  1, fo);
					*ret_length = elf_reverse_long(section_header_table[i].sh_size,
							 header->e_ident[5]);
					return 0;
				}
			}
		}
		if (*ret_length == 0) { /*nothing found*/
			return 1;
		}
	}
	else { /*empty table*/
		return 2;
	}

	return 1;
}
