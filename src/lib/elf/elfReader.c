#include "elfReader.h"
#include <stdio.h>
#include <string.h>
/*#include <lib/bits/byteswap.h>*/

/* Copied from lib/bits/byteswap.h for the purposes of compiling
    out of embox */
#define __bswap_constant_16(x) \
	((((x) >> 8) & 0xffu) | (((x) & 0xffu) << 8))
#define __bswap_constant_32(x) \
	((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >>  8) |	      \
	(((x) & 0x0000ff00u) <<  8) | (((x) & 0x000000ffu) << 24))


#define REVERSE_SHORT(x) (x) = __bswap_constant_16(x)
#define REVERSE_LONG(x) (x) = __bswap_constant_32(x)

#if 0
ulong reverseLong(ulong num, uchar reversed)
{
	switch(reversed){
		case 0:return(num);break;
		case 1:return(num);break;
		case 2:return(((num&0x000000ff)<<24) + ((num&0x0000ff00)<<8) + ((num&0x00ff0000)>>8) + ((num&0xff000000)>>24));break;
	}
}

ushort reverseShort(ushort num, uchar reversed)
{
	switch(reversed){
		case 0:return(num);break;
		case 1:return(num);break;
		case 2:return(((num&0x00ff)<<8) + ((num&0xff00)>>8));break;
	}
}
#endif

void printHeader(Elf32_Ehdr * header)
{
	printf("\n\ne_ident : ");
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
	switch(header->e_type){
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
	switch(header->e_machine){
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
	switch(header->e_version){
		case 0:printf("EV_NONE - Invalid version");break;
		case 1:printf("EV_CURRENT - Current version");break;
	}
	printf("\n");

	printf("e_entry - Virtual adress first to transfer control and start process (if no entry point then 0) : %ld \n", header->e_entry);

	printf("e_phoff - Program header table's file offset (0 if no such one) : %ld \n", header->e_phoff);

	printf("e_shoff - Section header table's file offset (0 if no such one) : %ld \n", header->e_shoff);

	printf("e_flags - Processor specific flags associated with file : %ld \n", header->e_flags);

	printf("e_ehsize - ELF header's size : %hd \n", header->e_ehsize);

	printf("e_phentsize - Size of entry in file's program header table : %hd \n", header->e_phentsize);

	printf("e_phnum - Number of entries in program header table : %hd \n", header->e_phnum);

	printf("e_shentsize - Section header's size : %hd \n", header->e_shentsize);

	printf("e_shnum - Number of entries in the section header table : %hd \n", header->e_shnum);

	printf("e_shstrndx - Section table header table index of the entry : %hd \n", header->e_shstrndx);
}

#if 0
void printSectionName(int index, char * stringTable)
{
	int offset = 0;
	while( stringTable[index+offset] != '\0' ){
		putchar(stringTable[index+offset]);
		offset++;
	}
}
#endif

void printSectionHeader(Elf32_Shdr * sectionHeader, char * stringTable)
{
	printf("\n");

	printf("sh_name - name of section : %ld - ", sectionHeader->sh_name);
	if (sectionHeader->sh_name == 0){
		printf("no name\n");
	}
	else{
		printf("%s\n" , &(stringTable[sectionHeader->sh_name]));
	}

	printf("sh_type - type of section header : ");
	switch(sectionHeader->sh_type){
		case 0:printf("SHT_NULL");break;
		case 1:printf("SHT_PROGBITS");break;
		case 2:printf("SHT_SYMTAB");break;
		case 3:printf("SHT_STRTAB");break;
		case 4:printf("SHT_RELA");break;
		case 5:printf("SHT_HASH");break;
		case 6:printf("SHT_DYNAMIC");break;
		case 7:printf("SHT_NOTE");break;
		case 8:printf("SHT_NOBITS");break;
		case 9:printf("SHT_REL");break;
		case 10:printf("SHT_SHLIB");break;
		case 11:printf("SHT_DYNSYM");break;
		case 0x70000000:printf("SHT_LOPROC");break;
		case 0x7fffffff:printf("SHT_HIPROC");break;
		case 0x80000000:printf("SHT_LOUSER");break;
		case 0xffffffff:printf("SHT_HIUSER");break;
	}
	printf("\n");

	ulong flags = sectionHeader->sh_flags;
	printf("sh_flags - 1-bit flags : %ld : ", flags);
	if (flags & 0x1) printf("SHF_WRITE ");
	if (flags & 0x2) printf("SHF_ALLOC ");
	if (flags & 0x4) printf("SHF_EXECINSTR ");
	if (flags & 0xf0000000) printf("SHF_MASKPROC ");
	printf("\n");

	printf("sh_addr - first byte in memory image of process : %ld \n", sectionHeader->sh_addr);

	printf("sh_offset - first byte of section in file : %ld \n", sectionHeader->sh_offset);

	printf("sh_size - section size : %ld \n",sectionHeader->sh_size);

	printf("sh_link - section header table index link: %ld \n", sectionHeader->sh_link);

	printf("sh_info - extra information: %ld \n", sectionHeader->sh_info);

	printf("sh_addralign - : %ld \n", sectionHeader->sh_addralign);

	printf("sh_entsize - : %ld \n", sectionHeader->sh_entsize);
}

void printSegmentHeader(Elf32_Phdr * segmentHeader)
{
	printf("\n");

	printf("p_type - type of segment : ");
	switch(segmentHeader->p_type){
		case 0:printf("PT_NULL");break;
		case 1:printf("PT_LOAD");break;
		case 2:printf("PT_DYNAMIC");break;
		case 3:printf("PT_INTERP");break;
		case 4:printf("PT_NOTE");break;
		case 5:printf("PT_SHLIB");break;
		case 6:printf("PT_PHDR");break;
		case 0x70000000:printf("PT_LOPROC");break;
		case 0x7fffffff:printf("PT_HIPROC");break;
	}
	printf("\n");

	printf("p_offset - first byte of segment in file : %ld \n", segmentHeader->p_offset);

	printf("p_vaddr - virtual address for first byte : %ld \n", segmentHeader->p_vaddr);

	printf("p_paddr - segments physical address : %ld \n", segmentHeader->p_paddr);

	printf("p_filesz - size of file image of the  segment : %ld \n", segmentHeader->p_filesz);

	printf("p_memsz - size of memory image of the segment : %ld \n", segmentHeader->p_memsz);

	printf("p_flags - flags relevant for the segment : %ld \n", segmentHeader->p_flags);

	printf("p_align - segments are aligned in memory and in the file : %ld \n", segmentHeader->p_align);
}

#if 0
readSymbolTable(Elf32_Shdr * sectionHeader, char reversed, Elf32_Sym * symbolTable,  char * symbolStringTable)
{
	int i = reverseLong(sectionHeader->sh_size, reversed) / sizeof(Elf32_Sym);
	for (;i>0;i--){


	}
}
#endif

/*
 * This function reads header and returns 1 on success and 0 on failure
 */
int read_header(FILE *f , Elf32_Ehdr *head) {

    fseek(f , 0 , 0);
    if (fread(head , sizeof(Elf32_Ehdr) , 1 , f) != 1) {
        return 0;
    }
    if (head->e_ident[0] != 0x7f || head->e_ident[1] != 'E' || \
            head->e_ident[2] != 'L' || head->e_ident[3] != 'F') {
        return 0; /* Incorrect identification */
    }
    if (head->e_ident[4] != 0x01) {
        return 0; /* Not 32bit => it has another structure */
    }
    if (head->e_ident[6] != 1) {
        return 0; /* Not version 1 */
    }
    if (head->e_ident[5] != 1 && head->e_ident[5] != 2) {
        return 0; /* Unknown data encoding */
    }
    if (head->e_ident[5] != MACHINE_DATA_ENCODING) {
        REVERSE_SHORT(head->e_type);
        REVERSE_SHORT(head->e_machine);
        REVERSE_LONG(head->e_version);
        REVERSE_LONG(head->e_entry);
        REVERSE_LONG(head->e_phoff);
        REVERSE_LONG(head->e_shoff);
        REVERSE_LONG(head->e_flags);
        REVERSE_SHORT(head->e_ehsize);
        REVERSE_SHORT(head->e_phentsize);
        REVERSE_SHORT(head->e_phnum);
        REVERSE_SHORT(head->e_shentsize);
        REVERSE_SHORT(head->e_shnum);
        REVERSE_SHORT(head->e_shstrndx);
    }
    if (head->e_version != 1) {
        return 0; /* Not version 1 */
    }
    return 1;
}

void reverse_section_head(Elf32_Shdr *head) {
    REVERSE_LONG(head->sh_name);
    REVERSE_LONG(head->sh_type);
    REVERSE_LONG(head->sh_flags);
    REVERSE_LONG(head->sh_addr);
    REVERSE_LONG(head->sh_offset);
    REVERSE_LONG(head->sh_size);
    REVERSE_LONG(head->sh_link);
    REVERSE_LONG(head->sh_info);
    REVERSE_LONG(head->sh_addralign);
    REVERSE_LONG(head->sh_entsize);
}

int read_section_table(FILE *f , Elf32_Ehdr *head , Elf32_Shdr sections[]) {
    Elf32_Word cnt , i;

    if (head->e_shoff == 0) {
        return 0; /* No section table */
    }
    fseek(f , head->e_shoff , 0);
    if (fread(&(sections[0]) , sizeof(Elf32_Shdr) , 1 , f) != 1) {
        return 0;
    }
    if (head->e_ident[5] != MACHINE_DATA_ENCODING) {
        reverse_section_head(&(sections[0]));
    }
    if (head->e_shnum != 0) {
        cnt = head->e_shnum;
    }
    else {
        cnt = sections[0].sh_size;
    }
    for (i = 1 ; i < cnt ; i++) {
        fseek(f , head->e_shoff + i * head->e_shentsize , 0);
        if (fread(&(sections[i]) , sizeof(Elf32_Shdr) , 1 , f) !=  1) {
            return 0;
        }
        if (head->e_ident[5] != MACHINE_DATA_ENCODING) {
            reverse_section_head(&(sections[i]));
        }
    }
    return 1;
}

void reverse_segment_head(Elf32_Phdr *head) {
    REVERSE_LONG(head->p_type);
    REVERSE_LONG(head->p_offset);
    REVERSE_LONG(head->p_vaddr);
    REVERSE_LONG(head->p_paddr);
    REVERSE_LONG(head->p_filesz);
    REVERSE_LONG(head->p_memsz);
    REVERSE_LONG(head->p_flags);
    REVERSE_LONG(head->p_align);
}

int read_segment_table(FILE *f , Elf32_Ehdr *head , Elf32_Phdr *segments) {
    int i;

    if (head->e_phoff == 0) {
        return 0;
    }
    for (i = 0 ; i < head->e_phnum ; i++) {
        fseek(f , head->e_phoff + i * head->e_phentsize , 0);
        if (fread(&(segments[i]) , sizeof(Elf32_Phdr) , 1 , f) != 1) {
            return 0;
        }
        if (head->e_ident[5] != MACHINE_DATA_ENCODING) {
            reverse_segment_head(&(segments[i]));
        }
    }
    return 1;
}

int read_string_table(FILE *f , Elf32_Ehdr *head , Elf32_Shdr sections[] , \
        char *stringtable) {
    Elf32_Word index;
    if (head->e_shstrndx != 0) {
        if (head->e_shstrndx != 0xffff) {
            index = head->e_shstrndx;
        }
        else {
            index = sections[0].sh_link;
        }
        //TODO check if speified section exists
        fseek(f , sections[index].sh_offset , 0);
        if (fread(stringtable , sections[index].sh_size , 1 , f) != 1) {
            return 0;
        }
        return 1;
    }
    else {
        return 0;
    }
}

int main(int argc, char *argv[])
{
	char * fileName = argv[1];
        char stringtable_ok;

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

        if (read_header(fo , &header)) {
            printHeader(&header);
        }
        else {
            printf("Error on reading header.\n");
            return;
        }


	Elf32_Shdr sectionHeaderTable[maxNumberOfSections];
	char stringTable[maxStringTableLength];
	Elf32_Sym symbolTable[maxSymbolTableLength];
	char symbolStringTable[maxSymbolStringTableLength];
	if (read_section_table(fo , &header , sectionHeaderTable)){
                stringtable_ok = read_string_table(fo , &header , sectionHeaderTable , stringTable);

		int i=0;
                // TODO replace header.e_shnum with more correct variable
		for (;i < header.e_shnum ; i++ ){
			printf("\n		Section #%d : \n", i+1);
			printSectionHeader(&(sectionHeaderTable[i]) , stringTable);

			if (sectionHeaderTable[i].sh_type == 2){ //2:"SHT_SYMTAB"
				fseek(fo, sectionHeaderTable[i].sh_offset , 0);
				fread(symbolTable, sectionHeaderTable[i].sh_size , 1, fo);
			}
			if ( (sectionHeaderTable[i].sh_type == 3) && ( i !=  header.e_shstrndx) ){ //3:"SHT_STRTAB" for symbols
				fseek(fo, sectionHeaderTable[i].sh_offset , 0);
				fread(symbolStringTable, sectionHeaderTable[i].sh_size , 1, fo);

			}
		}
	}

	Elf32_Phdr segmentHeaderTable[maxNumberOfSections];
	if (read_segment_table(fo , &header , segmentHeaderTable)){

		int i=0;
		for(;i< header.e_phnum ;i++){
			printf("\n		Segment #%d : \n", i+1);
			printSegmentHeader(&(segmentHeaderTable[i]));
		}
	}

	fclose(fo);
}


