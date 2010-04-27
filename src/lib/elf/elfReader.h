
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

#if 0
/*Collection of information about elf*/
typedef struct{
	Elf32_Ehdr header;
	Elf32_Shdr sectionHeaders[MAX_NUMBER_OF_SECTIONS];
	Elf32_Phdr segmentHeaders[MAX_NUMBER_OF_SECTIONS];
	char stringTable[MAX_STRING_TABLE_LENGTH];
} ElfFile;
#endif

/******LIBRARY INTERFACE******/

/*function reverses order of bytes in received integer with size 4-bytes if reversed value is 2 - according
 * specification and big-low type of bytes order in numbers
 *
 * @param num - integer to reverse
 * @param reversed - integer from specification - property talking about data type
 */
ulong elf_reverse_long(ulong num, uchar reversed);


/*function reverses order of bytes in received integer with size 2-bytes if reversed value is 2 - according
 * specification and big-low type of bytes order in numbers
 *
 * @param num - integer to reverse
 * @param reversed - integer from specification - property talking about data type
 */
ushort elf_reverse_short(ushort num, uchar reversed);

/*function prints consistens of elf header according the specification with comments and meaning writing
 *
 * @param header - head structure which describe common file information
 */
void elf_print_header(Elf32_Ehdr * header);

/**
 * finds index in array string-table and starts printing name of sm-th begining from that index till the nearest "/0"
 * string_table must start and finish with "/0"
 *
 * @param index - index in array which to start from
 * @param string_table  - array which consist names devided by "/0"
 */
void elf_print_name(int index, char * string_table);

/**
 * Prints given section header using information about reversing of byte-order
 * (big-endianess) with comments and some description of meanings
 *
 * @param section_header - struct to be print
 * @param reversed - info about big-low-endianess data type - according specification
 * @param string_table - an array given by user, got by according sectiong
 *        which func can get names of sections from
 */
void elf_print_section_header(Elf32_Shdr * section_header,
							  uchar reversed,
							  char * string_table);

/**
 * Prints given segment header using information about reversing of byte-order
 * (big-endianess) with comments and some description of meanings
 *
 * @param segment_header - struct to be print
 * @param reversed - info about big-low-endianess data type - according specification
 */
void elf_print_segment_header(Elf32_Phdr * segment_header, uchar reversed);

/*Oo TODO it*/
void elf_print_symbol_table(int counter, char reversed, Elf32_Sym * symbol_table, char * symbol_string_table);



#endif /* elf_elfReader_H_ */
