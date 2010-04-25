
//*****32-bit data types and sizes*****

//ELF32_Addr 4
#define Elf32_Addr unsigned long

//ELF32_Half 2
#define Elf32_Half unsigned short

//ELF32_Off 4
#define Elf32_Off unsigned long

//ELF32_Sword 4
#define Elf32_Sword signed long

//ELF32_Word 4
#define Elf32_Word unsigned long

//unsigned char 1
#define uchar unsigned char

#define ulong unsigned long
#define ushort unsigned short

#define MACHINE_DATA_ENCODING 1

#define maxNumberOfSections 100
#define maxStringTableLength 500
#define maxSymbolTableLength 3000
#define maxSymbolStringTableLength 200000
#define EI_NIDENT 16

#define ELF32_ST_BIND(i) ((i))>>4)
#define ELF32_ST_TYPE(i) ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

//ELF Header
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

//ELF Section header
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

//ELF Program Segment header
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

//Symbol table entry
typedef struct{
	Elf32_Word st_name;
	Elf32_Addr st_value;
	Elf32_Word st_size;
	uchar st_info;
	uchar st_other;
	Elf32_Half st_shndx;
} Elf32_Sym;

//Collection of information about elf
typedef struct{
	Elf32_Ehdr header;
	Elf32_Shdr sectionHeaders[maxNumberOfSections];
	Elf32_Phdr segmentHeaders[maxNumberOfSections];
	char stringTable[maxStringTableLength];
} ElfFile;

