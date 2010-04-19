/**
 * @file
 *
 * @date 19.4.10
 * @author Aleksandr Kirov
 */

#include "elfReader.h"
#include <stdio.h>
#include <string.h>

//the only argument is file name
int main(int argc, char *argv[])
{
	char * fileName = argv[1];

	Elf32_Ehdr header;

	if (argc != 2){
		printf("You didn't wrote a file name\n");
		return 1;
	}

	FILE * fo;
	if ((fo = fopen(fileName,"rb"))==NULL) {
		printf("Error on file open\n");
		return 1;
	}

	//size_t fread(void * buffer, size_t numberOfBytes, size_t counter, FILE * FL);
	fread(&header, sizeof(Elf32_Ehdr), 1, fo);

	printHeader(&header);

	fclose(fo);
}

//reverse byte-order in 4-byte integer
unsigned long reverseLong(unsigned long num, uchar reversed)
{
	switch(reversed){
		case 0 : return(num);break;
		case 1 : return(num);break;
		case 2 : return(((num&0x000000ff)<<24) + ((num&0x0000ff00)<<8) + ((num&0x00ff0000)>>8) + ((num&0xff000000)>>24));break;
	}
}

//reverse bytes order in 2-byte integer
unsigned short reverseShort(unsigned short num, uchar reversed)
{
	switch(reversed){
		case 0 : return(num);break;
		case 1 : return(num);break;
		case 2 : return(((num&0x00ff)<<8) + ((num&0xff00)>>8));break;
	}
}

//print ELF-header on screen
void printHeader(Elf32_Ehdr * header)
{
	printf("e_ident : ");
	int i = 0;
	for(;i<16;i++){
		printf("%c ", header->e_ident[i]);
	}
	printf("\n");

	printf("%c %c %c %c \n",header->e_ident[0],header->e_ident[1],header->e_ident[2],header->e_ident[3]);

	printf("EI_CLASS - File class : ");
	switch(header->e_ident[4]){
		case 0:printf("EFLCLASSNONE - Invalid class");break;
		case 1:printf("ELFCLASS32 - 32-bit objects");break;
		case 2:printf("ELFCLASS64 - 64-bit objects");break;
	}
	printf("\n");

	printf("EI_DATA - Data encoding : ");
	switch(header->e_ident[5]){
		case 0:printf("ELFDATANONE - Invalid data encoding");break;
		case 1:printf("ELFDATA2LSB");break;
		case 2:printf("ELFDATA2MSB");break;
	}
	printf("\n");

	printf("EI_VERSION - ELF header version number : ");
	switch(header->e_ident[6]){
		case 0:printf("EV_NONE - Invalid version");break;
		case 1:printf("EV_CURRENT - Current version");break;
	}
	printf("\n");

	printf("e_type - object file type : ");
	switch(reverseShort(header->e_type,header->e_ident[5])){
		case 0:printf("ET_NONE - No file type");break;
		case 1:printf("ET_REL - Relocatable file");break;
		case 2:printf("ET_EXEC - Executable file");break;
		case 3:printf("ET_DYN - Shared object file");break;
		case 4:printf("ET_CORE - Core file");break;
		case 0xff00:printf("ET_LOPROC - Processor specific");break;
		case 0xffff:printf("ET_HIPROC - Processor specific");break;
	}
	printf("\n");

	printf("e_machine - required architecture for an individual file : ");
	switch(reverseShort(header->e_machine,header->e_ident[5])){
		case 0:printf("EM_NONE - No machine");break;
		case 1:printf("EM_M32 - AT&T WE 32100");break;
		case 2:printf("EM_SPARC - SPARC");break;
		case 3:printf("EM_386 - Intel 80386");break;
		case 4:printf("EM_68K - Motorola 68000");break;
		case 5:printf("EM_88K - Motorola 88000");break;
		case 7:printf("EM_860 - Intel 80860");break;
		case 8:printf("EM_MIPS - MIPS RS3000");break;
	}
	printf("\n");

	printf("e_version - Object file version : ");
	switch(reverseLong(header->e_version,header->e_ident[5])){
		case 0:printf("EV_NONE - Invalid version");break;
		case 1:printf("EV_CURRENT - Current version");break;
	}
	printf("\n");

	printf("e_entry - Virtual adress first to transfer control and start process (if no entry point then 0) : %ld \n", reverseLong(header->e_entry,header->e_ident[5]));

	printf("e_phoff - Program header table's file offset (0 if no such one) : %ld \n", reverseLong(header->e_phoff,header->e_ident[5]));

	printf("e_shoff - Section header table's file offset (0 if no such one) : %ld \n", reverseLong(header->e_shoff,header->e_ident[5]));

	printf("e_flags - Processor specific flags associated with file : %ld \n", reverseLong(header->e_flags,header->e_ident[5]));

	printf("e_ehsize - ELF header's size : %hd \n", reverseShort(header->e_ehsize,header->e_ident[5]));

	printf("e_phentsize - Size of entry in file's program header table : %hd \n", reverseShort(header->e_phentsize,header->e_ident[5]));

	printf("e_phnum - Number of entries in program header table : %hd \n", reverseShort(header->e_phnum,header->e_ident[5]));

	printf("e_shentsize - Section header's size : %hd \n", reverseShort(header->e_shentsize,header->e_ident[5]));

	printf("e_shnum - Number of entries in the section header table : %hd \n", reverseShort(header->e_shnum,header->e_ident[5]));

	printf("e_shstrndx - Section table header table index of the entry : %hd \n", reverseShort(header->e_shstrndx,header->e_ident[5]));
}
