
/**
 * @date May 1, 2010
 * @author Aleksandr Kirov
 */


#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include "elf_writer.h"



void check_identity(unsigned long start1, unsigned long start2, unsigned long length);

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

	printf("e_entry - Virtual adress first to transfer control and start\
		   process (if no entry point then 0) : %ld \n",
		   elf_reverse_long(header->e_entry, header->e_ident[5]));

	printf("e_phoff - Program header table's file offset\
		   (0 if no such one) : %ld \n",
		   elf_reverse_long(header->e_phoff, header->e_ident[5]));

	printf("e_shoff - Section header table's file offset\
		   (0 if no such one) : %ld \n",
           elf_reverse_long(header->e_shoff, header->e_ident[5]));

	printf("e_flags - Processor specific flags associated\
 		   with file : %ld \n",
		   elf_reverse_long(header->e_flags, header->e_ident[5]));

	printf("e_ehsize - ELF header's size : %hd \n",
 	       elf_reverse_short(header->e_ehsize, header->e_ident[5]));

	printf("e_phentsize - Size of entry in file's program\
		   header table : %hd \n",
		   elf_reverse_short(header->e_phentsize, header->e_ident[5]));

	printf("e_phnum - Number of entries in program header table : %hd \n",
		   elf_reverse_short(header->e_phnum, header->e_ident[5]));

	printf("e_shentsize - Section header's size : %hd \n",
 		   elf_reverse_short(header->e_shentsize, header->e_ident[5]));

	printf("e_shnum - Number of entries in the section header table : %hd \n",
		   elf_reverse_short(header->e_shnum,header->e_ident[5]));

	printf("e_shstrndx - Section table header table index of the entry : %hd \n",
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
	printf("sh_name - name of section : %ld - ",
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

	printf("sh_addr - first byte in memory image of process : %ld \n",
		   elf_reverse_long(section_header->sh_addr,reversed));

	printf("sh_offset - first byte of section in file : %ld \n",
           elf_reverse_long(section_header->sh_offset, reversed));

	printf("sh_size - section size : %ld \n",
		   elf_reverse_long(section_header->sh_size, reversed));

	printf("sh_link - section header table index link: %ld \n",
		   elf_reverse_long(section_header->sh_link, reversed));

	printf("sh_info - extra information: %ld \n",
		   elf_reverse_long(section_header->sh_info, reversed));

	printf("sh_addralign - : %ld \n",
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

	default :
		printf("UNKNOWN (%ld)", elf_reverse_long(segment_header->p_type, reversed));
}
	printf("\n");

	printf("p_offset - first byte of segment in file : %ld \n",
		   elf_reverse_long(segment_header->p_offset, reversed));

	printf("p_vaddr - virtual address for first byte : %ld \n",
		   elf_reverse_long(segment_header->p_vaddr, reversed));

	printf("p_paddr - segments physical address : %ld \n",
		   elf_reverse_long(segment_header->p_paddr, reversed));

	printf("p_filesz - size of file image of the  segment : %ld \n",
		   elf_reverse_long(segment_header->p_filesz, reversed));

	printf("p_memsz - size of memory image of the segment : %ld \n",
		   elf_reverse_long(segment_header->p_memsz, reversed));

	printf("p_flags - flags relevant for the segment : %ld \n",
 		   elf_reverse_long(segment_header->p_flags, reversed));

	printf("p_align - segments are aligned in memory and in the file : %ld \n",
 		   elf_reverse_long(segment_header->p_align, reversed));
}


/*TODO it : look attantivly on string symbol table and taking names from it*/
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
			printf("st_name - name of symbol, index : %ld and name : ",
 				   elf_reverse_long(symbol_table[i].st_name, reversed));

			if (elf_reverse_long(symbol_table[i].st_name, reversed) != 0) {
				elf_print_name(elf_reverse_long(symbol_table[i].st_name, reversed),
						  symbol_string_table);
			}
			printf("\n");

			printf("st_value - value of symbol : %ld \n",
				   elf_reverse_long(symbol_table[i].st_value, reversed));

			printf("st_size - size of object : %ld \n",
				   elf_reverse_long(symbol_table[i].st_size, reversed));

			printf("st_info - type and binding attributes : %c \n",
				   symbol_table[i].st_info);

			printf("st_other - not defined : %c \n",
				   symbol_table[i].st_other);

			printf("st_shndx - index of according section %hd \n",
				   elf_reverse_short(symbol_table[i].st_shndx, reversed));
		}
	}
	else {
		printf("\nList of symbols is empty \n");
	}
}

void elf_print_rel(Elf32_Rel * rel_array, int count, int reversed)
{
	if (count != 0) {
		printf("\n");

		int i = 0;

		for (; i < count; i++ ) {
			printf("\nr_offset : %ld", elf_reverse_long(rel_array[i].r_offset, reversed));
			printf("\nr_info : %ld\n", elf_reverse_long(rel_array[i].r_info, reversed));
		}
	}
	else {
		printf("\nList of Elf32_Rel structures if empty\n");
	}
}

void elf_print_rela(Elf32_Rela * rela_array, int count, int reversed)
{
	if (count != 0) {
		printf("\n");

		int i = 0;

		for (; i < count; i++ ) {
			printf("\nr_offset : %ld", elf_reverse_long(rela_array[i].r_offset, reversed));
			printf("\nr_info : %ld", elf_reverse_long(rela_array[i].r_info, reversed));
			printf("\nr_addend : %ld\n", elf_reverse_long(rela_array[i].r_addend, reversed));
		}
	}
	else {
		printf("\nList of Elf32_Rela structures if empty\n");
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
	Elf32_Shdr section_header_table[MAX_NUMBER_OF_SECTIONS];

	/*Array for headers of segments*/
	Elf32_Phdr segment_header_table[MAX_NUMBER_OF_SECTIONS];

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

	Elf32_Rel rel_array[MAX_REL_ARRAY_LENGTH];
	int real_rel_array_length = 0;

	Elf32_Rela rela_array[MAX_RELA_ARRAY_LENGTH];
	int real_rela_array_length = 0;

	/*data description finished*/

	if (elf_reverse_long(header.e_shoff,header.e_ident[5]) != 0) {

		fseek(fo, elf_reverse_long(header.e_shoff,header.e_ident[5]), 0);

		fread(&section_header_table,
			  elf_reverse_short(header.e_shentsize,header.e_ident[5]),
			  elf_reverse_short(header.e_shnum,header.e_ident[5]), fo);

		if ( header.e_shstrndx != 0 ) {
			fseek(fo,
				  elf_reverse_long(section_header_table[elf_reverse_short(header.e_shstrndx, header.e_ident[5])].sh_offset,
																		 header.e_ident[5]),
				  0);

			fread(&string_table,
				  elf_reverse_long(section_header_table[elf_reverse_short(header.e_shstrndx, header.e_ident[5])].sh_size,
								   header.e_ident[5]),
				  1,
				  fo);

			real_string_table_length = elf_reverse_long(section_header_table[elf_reverse_short(header.e_shstrndx,
																						      header.e_ident[5])].sh_size,
														header.e_ident[5]);
		}

		int i=0;

		for (;i < elf_reverse_short(header.e_shnum,header.e_ident[5]) ; i++ ){

			printf("\n		Section #%d : \n", i+1);
			elf_print_section_header(&(section_header_table[i]),
								header.e_ident[5],
								string_table);

			/*We found SHT_SYMTAB*/
			if (elf_reverse_long(section_header_table[i].sh_type, header.e_ident[5]) == 2) {
				fseek(fo, elf_reverse_long(section_header_table[i].sh_offset,header.e_ident[5]), 0);
				fread(symbol_table, elf_reverse_long(section_header_table[i].sh_size, header.e_ident[5]), 1, fo);
				real_symbol_counter = elf_reverse_long(section_header_table[i].sh_size, header.e_ident[5]) / sizeof(Elf32_Sym);
			}

			/*We found SHT_RELA*/
			if (elf_reverse_long(section_header_table[i].sh_type, header.e_ident[5]) == 4) {
				fseek(fo, elf_reverse_long(section_header_table[i].sh_offset,header.e_ident[5]), 0);
				fread(rela_array + real_rel_array_length / sizeof(Elf32_Rela), elf_reverse_long(section_header_table[i].sh_size, header.e_ident[5]), 1, fo);
				real_rela_array_length += elf_reverse_long(section_header_table[i].sh_size, header.e_ident[5]);
			}


			/*We found SHT_REL*/
			if (elf_reverse_long(section_header_table[i].sh_type, header.e_ident[5]) == 9) {
				fseek(fo, elf_reverse_long(section_header_table[i].sh_offset,header.e_ident[5]), 0);
				fread(rel_array + real_rela_array_length  / sizeof(Elf32_Rel), elf_reverse_long(section_header_table[i].sh_size, header.e_ident[5]), 1, fo);
				real_rel_array_length += elf_reverse_long(section_header_table[i].sh_size, header.e_ident[5]);
			}

			/*TODO analyze of stringsymbol_table search*/
			/*printf("\n %d %d %ld \n",i,elf_reverse_short(header.e_shstrndx,header.e_ident[5]),
					elf_reverse_long(section_header_table[i].sh_type, header.e_ident[5]));*/
			/*if ( (elf_reverse_long(section_header_table[i].sh_type, header.e_ident[5]) == 3)){ /*3:"SHT_STRTAB" for symbols
				printf("\nsymbols table found\n");*/
				/*realsymbol_string_tableLength = elf_reverse_long(section_header_table[i].sh_size, header.e_ident[5]);
				 *fseek(fo, elf_reverse_long(section_header_table[i].sh_offset,header.e_ident[5]), 0);
				 *fread(symbol_string_table, realsymbol_string_tableLength, 1, fo);*/
			/*}*/
		}

		elf_print_symbol_table(real_symbol_counter, header.e_ident[5], symbol_table, symbol_string_table);
	}

	if (elf_reverse_long(header.e_phoff,header.e_ident[5]) != 0) {
		fseek(fo, elf_reverse_long(header.e_phoff,header.e_ident[5]), 0);
		fread(&segment_header_table, elf_reverse_short(header.e_phentsize,
													  header.e_ident[5]),
			  elf_reverse_short(header.e_phnum,header.e_ident[5]), fo);

		int i=0;

		for (;i<elf_reverse_short(header.e_phnum,header.e_ident[5]);i++) {
			printf("\n		Segment #%d : \n", i+1);
			elf_print_segment_header(&(segment_header_table[i]), header.e_ident[5]);
		}
	}

	fclose(fo);

/*FROM HERE*/

	int file_descriptor = open(file_name, O_RDONLY|O_SYNC);
	printf("\nFile open - descriptor: %d", file_descriptor);

	Elf32_Ehdr * EH = malloc(sizeof(Elf32_Ehdr));
	if (sizeof(Elf32_Ehdr) != read (file_descriptor, EH, sizeof(Elf32_Ehdr))) {
		printf("\nOn Elf header reading error : %d\n", errno);
		printf(strerror(errno));
		return 1;
	}

	Elf32_Phdr * EPH = malloc(EH->e_phentsize);

	lseek(file_descriptor, EH->e_phoff, SEEK_SET);

	printf("\nMmap possible error codes - %u %u %u %u %u %u %u %u %u %u\n", EACCES, EAGAIN, EBADF, EMFILE, EINVAL,  ENODEV, ENOMEM, ENOMEM, ENOTSUP, ENXIO, EOVERFLOW);

	int counter = EH->e_phnum;

	while(counter--) {
		int size = EH->e_phentsize;
		int bytes_read = (int)read(file_descriptor, EPH, size);

		printf("\nWe try to read %d bytes and really read %d bytes\n", size, bytes_read);
		printf("Reading finished successfully, type of section: %u \n", EPH->p_type);

		if (EPH->p_type == 1) {
			printf("\n For mapping:\n Virtual address 0x%x but we take address 0x%x, memory size %ld, but we take %ld, in-file offset %ld\n", EPH->p_vaddr, ((EPH->p_vaddr)), 						EPH->p_memsz, ((EPH->p_memsz & ~(sysconf(_SC_PAGE_SIZE) - 1)) + (sysconf(_SC_PAGE_SIZE) * ((EPH->p_memsz & ~(sysconf(_SC_PAGE_SIZE) - 1))==0))),
					EPH->p_offset);

			int pa = (int) mmap((void*)((EPH->p_vaddr)), /*EPH->p_memsz,*/
					(size_t)((EPH->p_memsz & ~(sysconf(_SC_PAGE_SIZE) - 1)) + (sysconf(_SC_PAGE_SIZE) * ((EPH->p_memsz & ~(sysconf(_SC_PAGE_SIZE) - 1))==0))),
					 PROT_EXEC|PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED, file_descriptor, EPH->p_offset);

			printf("\n Results, we got: we wanted to have prog at %ld and got %ld and error code %d\n", EPH->p_vaddr, (long)pa, errno);
		}
	}

	printf("\nFile close - code: %d\n", close(file_descriptor));

	printf("\nData allocated. \nTrying to start at %ld(0x%x)\n", EH->e_entry, EH->e_entry);

	int (*function_main)(int argc, char *argv[]) = EH->e_entry;
	int result = (*function_main) (0, 0);

	printf("\n result : %d\n", result);
}
/*
int lseek(int fd, int position, int startpoint)
int read(int fd, void * ptr, int numbytes)
      include: <unistd.h>
      fd = file descriptor as returned by open
      ptr = address in memory where data is to be stored;
            may be a pointer of any type.
      numbytes = number of bytes to attempt to read
      returns <0 for error, 0 for end-of-file,
              or number of bytes successfully read.
              (for a non-blocking interactive file, that may be zero).
*/

/*

	FILE * stream;
	int buf_size;

	if ((stream = fopen(file_name,"rb")) == NULL) {
		printf("Error on file open\n");
		return 1;
	}

	fseek(stream, 0L, SEEK_END);
 	buf_size = ftell(stream);
	fseek(stream, 0L, SEEK_SET);

	char* bin;
	bin = malloc(buf_size+1);

	stream  = fopen(file_name, "rb");
	fread(bin,buf_size,1,stream);
	bin[buf_size]='\0';


	Elf32_Ehdr * EH = (Elf32_Ehdr *)bin;
	Elf32_Phdr * EPH;

	EPH = (Elf32_Phdr*)(bin + EH->e_phoff);

	int fd = open(file_name, O_RDONLY);

	printf("\nFD: %d\n",(int)fd);


	while (EH->e_phnum--) {
printf("\n2\n%ld",(long)EH->e_phnum);
printf("try to start");
		if (EPH->p_type == 1)
		{printf("start");*/
			/*printf("\n%ld %ld %ld %ld\n",EPH->p_vaddr & ~(sysconf(_SC_PAGE_SIZE)-1),
					bin + EPH->p_offset, sysconf(_SC_PAGE_SIZE), EPH->p_offset);*/
/*
			printf("Sizes %u %u ", (EPH->p_vaddr & ~(sysconf(_SC_PAGE_SIZE)-1)),
					((EPH->p_memsz & ~(sysconf(_SC_PAGE_SIZE) - 1))        +         (sysconf(_SC_PAGE_SIZE) * ((EPH->p_memsz & ~(sysconf(_SC_PAGE_SIZE) - 1))==0))));

			void * addr = mmap((void*)(EPH->p_vaddr & ~(sysconf(_SC_PAGE_SIZE)-1)),
				(size_t)((EPH->p_memsz & ~(sysconf(_SC_PAGE_SIZE) - 1))        +         (sysconf(_SC_PAGE_SIZE) * ((EPH->p_memsz & ~(sysconf(_SC_PAGE_SIZE) - 1))==0))),
				PROT_WRITE, MAP_FIXED|MAP_PRIVATE , fd ,EPH->p_offset);

			printf("\nMmap done %u %u %u %u %u %u %u %u %u\n", EAGAIN, EBADF, EMFILE, ENODEV, ENOMEM, ENOMEM, ENOTSUP, ENXIO, EOVERFLOW);*/

/*			printf("Magic symbol is %c (must be A)\n", *((char *)0x11111111));*/

/*			printf("\nAnd we get %08x\n", (unsigned long) addr);

			printf("After mmap: address %u and error %u, we wanted %u or %u\n", (unsigned long) addr, (long) errno,
					(unsigned long)EPH->p_vaddr & ~(sysconf(_SC_PAGE_SIZE)-1), EPH->p_memsz);

			memcpy ((void*)(EPH->p_vaddr & ~(sysconf(_SC_PAGE_SIZE)-1)), (void*)bin + EPH->p_offset, (size_t)EPH->p_memsz);

			printf("\nCheck memory identity on adresses %u and %u, for %u bytes : ",
				(unsigned long)EPH->p_vaddr, (unsigned long) bin + EPH->p_offset, EPH->p_filesz);

			check_identity((unsigned long)EPH->p_vaddr, (unsigned long) bin + EPH->p_offset, EPH->p_filesz);

			printf("\ncopy");

			printf("\n%ld %ld %ld %ld\n",EPH->p_vaddr, (long int) bin + EPH->p_offset, EPH->p_filesz, EPH->p_offset);

		}
		printf("\nfinish or miss on address %ld\n",(long)EPH);
printf("\n1\n");
        EPH = (Elf32_Phdr *) ( (unsigned char *) EPH + EH->e_phentsize);
printf("\n2\n%ld",(long)EH->e_phnum);
	}*/

	/*printf("\nTrying to start at %ld\n", EH->e_entry);

	void (*fp)();

    fp = EH->e_entry;
	fp();

	printf("\n result : %ld", (long)(*fp));*/


/*	fclose(stream);


	return 0;
}

void check_identity(unsigned long start1, unsigned long start2, unsigned long length) {
	double identity = 0;
	unsigned long position = 0;

	for(; position < length; position++ ) {
		if ( (*((char*)(start1 + position))) == (*((char*)(start2 + position)))) {
			identity += 1;
		}
	}

	printf("\nMemory is identical for %4.3f percents\n", identity / length * 100);
	return;
}*/

