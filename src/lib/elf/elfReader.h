
/**
 * @date Apr 27, 2010
 * @author Aleksandr Kirov
 */

#ifndef elf_elfReader_H_
#define elf_elfReader_H_

/******32-BIT DATA TYPES AND SIZES******/

/*ELF32_Addr 4*/
#define Elf32_Addr unsigned long

/*ELF32_Half 2*/
#define Elf32_Half unsigned short

/*ELF32_Off 4*/
#define Elf32_Off unsigned long

/*ELF32_Sword 4*/
#define Elf32_Sword signed long

/*ELF32_Word 4*/
#define Elf32_Word unsigned long

/*unsigned char 1*/
#define uchar unsigned char

/******OTHER DEFINES******/

#define ulong unsigned long
#define ushort unsigned short

/******CONSTANTS******/

#define MAX_NUMBER_OF_SECTIONS 100
#define MAX_STRING_TABLE_LENGTH 500
#define MAX_SYMBOL_TABLE_LENGTH 3000
#define MAX_SYMBOL_STRING_TABLE_LENGTH 50000
#define EI_NIDENT 16

/******FUNCTIONS FOR DATA******/

#define ELF32_ST_BIND(i) ((i))>>4)
#define ELF32_ST_TYPE(i) ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

#define ELF32_R_SYM(i) ((i)>>8)
#define ELF32_R_TYPE(i) ((unsigned char) (i))
#define ELF32_R__INFO(s,t) (((s)<<8)+(unsigned char)(t))

/******DATA STRUCTS******/

/*ELF Header*/
typedef struct{
	uchar e_ident[EI_NIDENT];
	Elf32_Half e_type;
	Elf32_Half e_machine;
	Elf32_Word e_version;
	Elf32_Addr e_entry;
	Elf32_Off e_phoff;
	Elf32_Off e_shoff;
	Elf32_Word e_flags;
	Elf32_Half e_ehsize;
	Elf32_Half e_phentsize;
	Elf32_Half e_phnum;
	Elf32_Half e_shentsize;
	Elf32_Half e_shnum;
	Elf32_Half e_shstrndx;
}Elf32_Ehdr;

/*ELF Section header*/
typedef struct{
	Elf32_Word sh_name;
	Elf32_Word sh_type;
	Elf32_Word sh_flags;
	Elf32_Addr sh_addr;
	Elf32_Off sh_offset;
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize;
}Elf32_Shdr;

/*ELF Program Segment header*/
typedef struct{
	Elf32_Word p_type;
	Elf32_Off p_offset;
	Elf32_Addr p_vaddr;
	Elf32_Addr p_paddr;
	Elf32_Word p_filesz;
	Elf32_Word p_memsz;
	Elf32_Word p_flags;
	Elf32_Word p_align;
} Elf32_Phdr;

/*Symbol table entry*/
typedef struct{
	Elf32_Word st_name;
	Elf32_Addr st_value;
	Elf32_Word st_size;
	uchar st_info;
	uchar st_other;
	Elf32_Half st_shndx;
} Elf32_Sym;

typedef struct{
	Elf32_Addr r_offset;
	Elf32_Word r_info;
} ELF32_Rel;

typedef struct{
	Elf32_Addr r_offset;
	Elf32_Word r_info;
	Elf32_Sword r_addend;
} Elf32_Rela;

/*Collection of information about elf*/
typedef struct{
	Elf32_Ehdr header;
	Elf32_Shdr sectionHeaders[MAX_NUMBER_OF_SECTIONS];
	Elf32_Phdr segmentHeaders[MAX_NUMBER_OF_SECTIONS];
	char stringTable[MAX_STRING_TABLE_LENGTH];
} ElfFile;

#endif /* elf_elfReader_H_ */

