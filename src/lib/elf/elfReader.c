
/**
 * @date Apr 27, 2010
 * @author Aleksandr Kirov
 */

#include "elfReader.h"
#include <stdio.h>
#include <string.h>

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

void elf_print_header(Elf32_Ehdr * header)
{
	printf("\n\ne_ident : ");

	int i = 0;

	for ( ;i < 16; i++) {
		printf("%c ", header->e_ident[i]);
	}
	printf("\n");

	printf("%c %c %c %c \n",header->e_ident[0],header->e_ident[1],
							header->e_ident[2],header->e_ident[3]);

	printf("EI_CLASS - File class : ");
	switch (header->e_ident[4]){
		case 0 : printf("EFLCLASSNONE - Invalid class");break;
		case 1 : printf("ELFCLASS32 - 32-bit objects");break;
		case 2 : printf("ELFCLASS64 - 64-bit objects");break;
	}
	printf("\n");

	printf("EI_DATA - Data encoding : ");
	switch (header->e_ident[5]) {
		case 0 : printf("ELFDATANONE - Invalid data encoding");break;
		case 1 : printf("ELFDATA2LSB");break;
		case 2 : printf("ELFDATA2MSB");break;
	}
	printf("\n");

	printf("EI_VERSION - ELF header version number : ");
	switch (header->e_ident[6]) {
		case 0 : printf("EV_NONE - Invalid version");break;
		case 1 : printf("EV_CURRENT - Current version");break;
	}
	printf("\n");

	printf("e_type - object file type : ");
	switch (elf_reverse_short(header->e_type, header->e_ident[5])) {
		case 0 : printf("ET_NONE - No file type");break;
		case 1 : printf("ET_REL - Relocatable file");break;
		case 2 : printf("ET_EXEC - Executable file");break;
		case 3 : printf("ET_DYN - Shared object file");break;
		case 4 : printf("ET_CORE - Core file");break;
		case 0xff00 : printf("ET_LOPROC - Processor specific");break;
		case 0xffff : printf("ET_HIPROC - Processor specific");break;
	}
	printf("\n");

	printf("e_machine - required architecture for an individual file : ");
	switch (elf_reverse_short(header->e_machine,header->e_ident[5])) {
		case 0 : printf("EM_NONE - No machine");break;
		case 1 : printf("EM_M32 - AT&T WE 32100");break;
		case 2 : printf("EM_SPARC - SPARC");break;
		case 3 : printf("EM_386 - Intel 80386");break;
		case 4 : printf("EM_68K - Motorola 68000");break;
		case 5 : printf("EM_88K - Motorola 88000");break;
		case 7 : printf("EM_860 - Intel 80860");break;
		case 8 : printf("EM_MIPS - MIPS RS3000");break;
	}
	printf("\n");

	printf("e_version - Object file version : ");
	switch (elf_reverse_long(header->e_version,header->e_ident[5])) {
		case 0 : printf("EV_NONE - Invalid version");break;
		case 1 : printf("EV_CURRENT - Current version");break;
	}
	printf("\n");

	printf("e_entry - Virtual adress first to transfer control and start \
		   process (if no entry point then 0) : %ld \n", \
		   elf_reverse_long(header->e_entry, header->e_ident[5]));

	printf("e_phoff - Program header table's file offset \
		   (0 if no such one) : %ld \n", \
		   elf_reverse_long(header->e_phoff, header->e_ident[5]));

	printf("e_shoff - Section header table's file offset \
		   (0 if no such one) : %ld \n", \
           elf_reverse_long(header->e_shoff, header->e_ident[5]));

	printf("e_flags - Processor specific flags associated \
 		   with file : %ld \n", \
		   elf_reverse_long(header->e_flags, header->e_ident[5]));

	printf("e_ehsize - ELF header's size : %hd \n", \
 	       elf_reverse_short(header->e_ehsize, header->e_ident[5]));

	printf("e_phentsize - Size of entry in file's program \
		   header table : %hd \n", \
		   elf_reverse_short(header->e_phentsize, header->e_ident[5]));

	printf("e_phnum - Number of entries in program header table : %hd \n", \
		   elf_reverse_short(header->e_phnum, header->e_ident[5]));

	printf("e_shentsize - Section header's size : %hd \n", \
 		   elf_reverse_short(header->e_shentsize, header->e_ident[5]));

	printf("e_shnum - Number of entries in the section header table : %hd \n", \
		   elf_reverse_short(header->e_shnum,header->e_ident[5]));

	printf("e_shstrndx - Section table header table index of the entry : %hd \n", \
		   elf_reverse_short(header->e_shstrndx, header->e_ident[5]));
}

void elf_print_name(int index, char * string_table)
{
	int offset = 0;

	while (string_table[index+offset] != '\0') {
		putchar(string_table[index + offset]);
		offset++;
	}
}

void elf_print_section_header(Elf32_Shdr * section_header,
							  uchar reversed,
							  char * string_table)
{
	printf("\n");
	printf("sh_name - name of section : %ld - ", \
		   elf_reverse_long(section_header->sh_name,reversed));

	if (elf_reverse_long(section_header->sh_name, reversed) == 0) {
		printf("no name");
	}
	else {
		elf_print_name(elf_reverse_long(section_header->sh_name,reversed), string_table);
	}
	printf("\n");

	printf("sh_type - type of section header : ");
	switch (elf_reverse_long(section_header->sh_type, reversed)) {
		case 0 : printf("SHT_NULL");break;
		case 1 : printf("SHT_PROGBITS");break;
		case 2 : printf("SHT_SYMTAB");break;
		case 3 : printf("SHT_STRTAB");break;
		case 4 : printf("SHT_RELA");break;
		case 5 : printf("SHT_HASH");break;
		case 6 : printf("SHT_DYNAMIC");break;
		case 7 : printf("SHT_NOTE");break;
		case 8 : printf("SHT_NOBITS");break;
		case 9 : printf("SHT_REL");break;
		case 10 : printf("SHT_SHLIB");break;
		case 11 : printf("SHT_DYNSYM");break;
		case 0x70000000 : printf("SHT_LOPROC");break;
		case 0x7fffffff : printf("SHT_HIPROC");break;
		case 0x80000000 : printf("SHT_LOUSER");break;
		case 0xffffffff : printf("SHT_HIUSER");break;
	}
	printf("\n");

	ulong flags = elf_reverse_long(section_header->sh_flags, reversed);

	printf("sh_flags - 1-bit flags : %ld : ", flags);
	if (flags & 0x1) printf("SHF_WRITE ");
	if (flags & 0x2) printf("SHF_ALLOC ");
	if (flags & 0x4) printf("SHF_EXECINSTR ");
	if (flags & 0xf0000000) printf("SHF_MASKPROC ");
	printf("\n");

	printf("sh_addr - first byte in memory image of process : %ld \n", \
		   elf_reverse_long(section_header->sh_addr,reversed));

	printf("sh_offset - first byte of section in file : %ld \n", \
           elf_reverse_long(section_header->sh_offset, reversed));

	printf("sh_size - section size : %ld \n", \
		   elf_reverse_long(section_header->sh_size, reversed));

	printf("sh_link - section header table index link: %ld \n", \
		   elf_reverse_long(section_header->sh_link, reversed));

	printf("sh_info - extra information: %ld \n", \
		   elf_reverse_long(section_header->sh_info, reversed));

	printf("sh_addralign - : %ld \n", \
           elf_reverse_long(section_header->sh_addralign, reversed));

	printf("sh_entsize - : %ld \n",
           elf_reverse_long(section_header->sh_entsize, reversed));
}

void elf_print_segment_header(Elf32_Phdr * segment_header, uchar reversed)
{
	printf("\n");

	printf("p_type - type of segment : ");
	switch (elf_reverse_long(segment_header->p_type, reversed)) {
		case 0 : printf("PT_NULL");break;
		case 1 : printf("PT_LOAD");break;
		case 2 : printf("PT_DYNAMIC");break;
		case 3 : printf("PT_INTERP");break;
		case 4 : printf("PT_NOTE");break;
		case 5 : printf("PT_SHLIB");break;
		case 6 : printf("PT_PHDR");break;
		case 0x70000000 : printf("PT_LOPROC");break;
		case 0x7fffffff : printf("PT_HIPROC");break;
	}
	printf("\n");

	printf("p_offset - first byte of segment in file : %ld \n", \
		   elf_reverse_long(segment_header->p_offset, reversed));

	printf("p_vaddr - virtual address for first byte : %ld \n", \
		   elf_reverse_long(segment_header->p_vaddr, reversed));

	printf("p_paddr - segments physical address : %ld \n", \
		   elf_reverse_long(segment_header->p_paddr, reversed));

	printf("p_filesz - size of file image of the  segment : %ld \n", \
		   elf_reverse_long(segment_header->p_filesz, reversed));

	printf("p_memsz - size of memory image of the segment : %ld \n", \
		   elf_reverse_long(segment_header->p_memsz, reversed));

	printf("p_flags - flags relevant for the segment : %ld \n", \
 		   elf_reverse_long(segment_header->p_flags, reversed));

	printf("p_align - segments are aligned in memory and in the file : %ld \n", \
 		   elf_reverse_long(segment_header->p_align, reversed));
}

void elf_print_symbol_table(int counter,
					   char reversed,
					   Elf32_Sym * symbol_table,
					   char * symbol_string_table)
{
	printf("\n");

	int i = 0;

	if ( counter != 0 ) {
		for (; i < counter ; i++) {
			printf("\n Symbol #%d \n", i);
			printf("st_name - name of symbol, index : %ld and name : ", \
 				   elf_reverse_long(symbol_table[i].st_name, reversed));

			if (elf_reverse_long(symbol_table[i].st_name, reversed) != 0) {
				elf_print_name(elf_reverse_long(symbol_table[i].st_name, reversed), \
						  symbol_string_table);
			}
			printf("\n");

			printf("st_value - value of symbol : %ld \n", \
				   elf_reverse_long(symbol_table[i].st_value, reversed));

			printf("st_size - size of object : %ld \n", \
				   elf_reverse_long(symbol_table[i].st_size, reversed));

			printf("st_info - type and binding attributes : %c \n", \
				   symbol_table[i].st_info);

			printf("st_other - not defined : %c \n", \
				   symbol_table[i].st_other);

			printf("st_shndx - index of according section %hd \n", \
				   elf_reverse_short(symbol_table[i].st_shndx, reversed));
		}
	}
	else {
		printf("\nList of symbols is empty \n");
	}
}

int main(int argc, char *argv[])
{
	char * file_name = argv[1];

	Elf32_Ehdr header;

	if (argc != 2) {
		printf("You didn't wrote a file name\n");
		return 1;
	}

	FILE * fo;
	if ((fo = fopen(file_name,"rb")) == NULL) {
		printf("Error on file open\n");
		return 1;
	}

	/*prototype : size_t fread(void * buf, size_t num, size_t count, FILE * F);*/
	fread(&header, sizeof(Elf32_Ehdr), 1, fo);
	elf_print_header(&header);

	/*Array for headers of sections*/
	Elf32_Shdr section_headerTable[MAX_NUMBER_OF_SECTIONS];

	/*Array for headers of segments*/
	Elf32_Phdr segment_headerTable[MAX_NUMBER_OF_SECTIONS];

	/* Array for symbols - consist of structures*/
	Elf32_Sym symbol_table[MAX_SYMBOL_TABLE_LENGTH];
	/* counter of elements in symbol_table because it is static length*/
	int real_symbol_counter = 0;

	/*Array for names */
	char string_table[MAX_STRING_TABLE_LENGTH];
	int real_string_table_length = 0;

	/*Array for names of symbols from symbol string table*/
	char symbol_string_table[MAX_SYMBOL_STRING_TABLE_LENGTH];
	int realsymbol_string_tableLength = 0;

	if (elf_reverse_long(header.e_shoff,header.e_ident[5]) != 0) {

		fseek(fo, elf_reverse_long(header.e_shoff,header.e_ident[5]), 0);

		fread(&section_headerTable,
			  elf_reverse_short(header.e_shentsize,header.e_ident[5]),
			  elf_reverse_short(header.e_shnum,header.e_ident[5]), fo);

		if ( header.e_shstrndx != 0 ) {
			fseek(fo,
				  elf_reverse_long(section_headerTable[elf_reverse_short(header.e_shstrndx, header.e_ident[5])].sh_offset,
																		 header.e_ident[5]),
				  0);

			fread(&string_table,
				  elf_reverse_long(section_headerTable[elf_reverse_short(header.e_shstrndx, header.e_ident[5])].sh_size,
								   header.e_ident[5]),
				  1,
				  fo);

			real_string_table_length = elf_reverse_long(section_headerTable[elf_reverse_short(header.e_shstrndx,
																						      header.e_ident[5])].sh_size,
														header.e_ident[5]);
		}

		int i=0;

		for (;i < elf_reverse_short(header.e_shnum,header.e_ident[5]) ; i++ ){

			printf("\n		Section #%d : \n", i+1);
			elf_print_section_header(&(section_headerTable[i]),
								header.e_ident[5],
								string_table);

			/*2:"SHT_SYMTAB"*/
			if (elf_reverse_long(section_headerTable[i].sh_type, header.e_ident[5]) == 2) {
				fseek(fo, elf_reverse_long(section_headerTable[i].sh_offset,header.e_ident[5]), 0);
				fread(symbol_table, elf_reverse_long(section_headerTable[i].sh_size, header.e_ident[5]), 1, fo);
				real_symbol_counter = elf_reverse_long(section_headerTable[i].sh_size, header.e_ident[5]) / sizeof(Elf32_Sym);
			}

			/*TODO analyze of stringsymbol_table search*/
			/*printf("\n %d %d %ld \n",i,elf_reverse_short(header.e_shstrndx,header.e_ident[5]),
					elf_reverse_long(section_headerTable[i].sh_type, header.e_ident[5]));*/
			/*if ( (elf_reverse_long(section_headerTable[i].sh_type, header.e_ident[5]) == 3)){ /*3:"SHT_STRTAB" for symbols
				printf("\nsymbols table found\n");*/
				/*realsymbol_string_tableLength = elf_reverse_long(section_headerTable[i].sh_size, header.e_ident[5]);
				 *fseek(fo, elf_reverse_long(section_headerTable[i].sh_offset,header.e_ident[5]), 0);
				 *fread(symbol_string_table, realsymbol_string_tableLength, 1, fo);*/
			/*}*/
		}

		elf_print_symbol_table(real_symbol_counter, header.e_ident[5], symbol_table, symbol_string_table);
	}

	if (elf_reverse_long(header.e_phoff,header.e_ident[5]) != 0) {
		fseek(fo, elf_reverse_long(header.e_phoff,header.e_ident[5]), 0);
		fread(&segment_headerTable, elf_reverse_short(header.e_phentsize,
													  header.e_ident[5]),
			  elf_reverse_short(header.e_phnum,header.e_ident[5]), fo);

		int i=0;

		for (;i<elf_reverse_short(header.e_phnum,header.e_ident[5]);i++) {
			printf("\n		Segment #%d : \n", i+1);
			elf_print_segment_header(&(segment_headerTable[i]), header.e_ident[5]);
		}
	}

	fclose(fo);
}

