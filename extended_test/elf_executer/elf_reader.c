
/**
 * @date Apr 27, 2010
 * @author Aleksandr Kirov
 */

#include "elf_reader.h"

ulong elf_reverse_long(ulong num, uchar reversed)
{
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
}

ushort elf_reverse_short(ushort num, uchar reversed)
{
	switch (reversed) {
		case 0 : return(num);break;
		case 1 : return(num);break;
		case 2 : return(((num & 0x00ff ) << 8) + ((num & 0xff00) >> 8));break;
	}
}

int elf_read_header(FILE * fo, Elf32_Ehdr * header)
{
	if (fo != NULL) {
		fread(header, sizeof(Elf32_Ehdr), 1, fo);
		return 0;
	}
	else {
		return 1;
	}
}

int elf_read_sections_table(FILE * fo, Elf32_Ehdr * header,
							Elf32_Shdr * section_header_table)
{
	if (elf_reverse_long(header->e_shoff, header->e_ident[5]) != 0) {
		fseek(fo, elf_reverse_long(header->e_shoff, header->e_ident[5]), 0);
		fread(&section_header_table,
			  elf_reverse_short(header->e_shentsize,header->e_ident[5]),
			  elf_reverse_short(header->e_shnum,header->e_ident[5]), fo);
		return 0;
	}
	else {/*Table doesn't exist*/
		return 1;
	}
}

int elf_read_segments_table(FILE * fo, Elf32_Ehdr * header,
							Elf32_Phdr * segment_header_table)
{
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

int elf_read_string_table(FILE * fo, Elf32_Ehdr * header,
						  Elf32_Shdr * section_header_table,
						  char * names, int * length)
{
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

int elf_read_symbol_table(FILE * fo, Elf32_Ehdr * header,
						  Elf32_Shdr * section_header_table,
						  Elf32_Sym * symbol_table, int * count)
{
	if (elf_reverse_long(header->e_shoff,header->e_ident[5]) != 0) {
		int i=0;
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
}

int elf_read_rel_table(FILE * fo, Elf32_Ehdr * header,
				       Elf32_Shdr * section_header_table,
					   Elf32_Rel * rel_table, int * count)
{
	if (elf_reverse_long(header->e_shoff,header->e_ident[5]) != 0) {
		int i=0;
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

int elf_read_rela_table(FILE * fo, Elf32_Ehdr * header,
				       Elf32_Shdr * section_header_table,
					   Elf32_Rela * rela_table, int * count)
{
	if (elf_reverse_long(header->e_shoff,header->e_ident[5]) != 0) {
		int i=0;
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

/*TODO*/
int elf_read_symbol_string_table(FILE * fo, Elf32_Ehdr * header,
								 Elf32_Shdr * section_header_table,
								 char * names, int * length)
{
	return 0;//works not properly

	if (elf_reverse_long(header->e_shoff,header->e_ident[5]) != 0) {
		int i=0;

		for (;i < elf_reverse_short(header->e_shnum,header->e_ident[5]) ;i++) {
		/*3:"SHT_STRTAB" not  only for symbols*/
			if ( (elf_reverse_long(section_header_table[i].sh_type,
				  header->e_ident[5]) == 3)){
				*length = elf_reverse_long(section_header_table[i].sh_size,
											header->e_ident[5]);
				fseek(fo, elf_reverse_long(section_header_table[i].sh_offset,
						header->e_ident[5]), 0);
				fread(names, *length, 1, fo);
			}
		}

		if (*length != 0) {
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

