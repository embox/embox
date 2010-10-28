/**
 * @file
 * @brief Representation of ELF data types.
 *
 * @date 28.10.10
 * @author Nikolay Korotky
 */

#ifndef LIB_ELF_TYPES_H_
#define LIB_ELF_TYPES_H_

#include <stdio.h>
#include <types.h>
#include <errno.h>

#define __libelf_u32_t    __u32
#define __libelf_u16_t    __u16
#define __libelf_i32_t    __s32

typedef __libelf_u32_t    Elf32_Addr;
typedef __libelf_u16_t    Elf32_Half;
typedef __libelf_u32_t    Elf32_Off;
typedef __libelf_i32_t    Elf32_Sword;
typedef __libelf_u32_t    Elf32_Word;

/**
 * Until dynamic memory applied array length constrain used
 */
#define MAX_NUMBER_OF_SECTIONS          100
#define MAX_STRING_TABLE_LENGTH         500
#define MAX_SYMBOL_TABLE_LENGTH         3000
#define MAX_SYMBOL_STRING_TABLE_LENGTH  50000
#define MAX_REL_ARRAY_LENGTH            10000
#define MAX_RELA_ARRAY_LENGTH           10000
#define MAX_NAME_LENGTH                 150

#define MAX_SEGMENTS     100
#define MAX_SYMB         1000
#define MAX_SYMB_NAMES   100000

/**
 * Length in bytes of main field in Elf-header structure
 */
#define EI_NIDENT           16

/**
 * Code of information in symbols descriptions. Watch Elf-specification.
 */
#define ELF32_ST_BIND(i)    ((i))>>4)

/**
 * Code of information in symbols descriptions. Watch Elf-specification.
 */
#define ELF32_ST_TYPE(i)    ((i)&0xf)

/**
 * Code of information in symbols descriptions. Watch Elf-specification.
 */
#define ELF32_ST_INFO(b,t)  (((b)<<4)+((t)&0xf))

/**
 * Code of information in Relocation table. Watch Elf-specification.
 */
#define ELF32_R_SYM(i)      ((i)>>8)

/**
 * Code of information in Relocation table. Watch Elf-specification.
 */
#define ELF32_R_TYPE(i)     ((unsigned char) (i))

/**
 * Code of information in Relocation table. Watch Elf-specification.
 */
#define ELF32_R_INFO(s,t)   (((s)<<8)+(unsigned char)(t))

/**
 * p_type
 */
#define PT_NULL         0
#define PT_LOAD         1
#define PT_DYNAMIC      2
#define PT_INTERP       3
#define PT_NOTE         4
#define PT_SHLIB        5
#define PT_PHDR         6
#define PT_TLS          7
#define PT_NUM          8
#define PT_LOOS         0x60000000
#define PT_HIOS         0x6fffffff
#define PT_LOPROC       0x70000000
#define PT_HIPROC       0x7fffffff

/**
 * ELF Header. Code style from specification.
 */
typedef struct {
	uint8_t    e_ident[EI_NIDENT];
	Elf32_Half e_type;
	Elf32_Half e_machine;
	Elf32_Word e_version;
	Elf32_Addr e_entry;
	Elf32_Off  e_phoff;
	Elf32_Off  e_shoff;
	Elf32_Word e_flags;
	Elf32_Half e_ehsize;
	/**
	 * This member holds the size in bytes of one entry in the
	 * file’s program header table; all entries are the same size.
	 */
	Elf32_Half e_phentsize;
	Elf32_Half e_phnum;
	Elf32_Half e_shentsize;
	Elf32_Half e_shnum;
	Elf32_Half e_shstrndx;
} Elf32_Ehdr;

/*
 * e_ident
 */
#define ELFMAG0         0x7f
#define ELFMAG1         'E'
#define ELFMAG2         'L'
#define ELFMAG3         'F'

/*
 * e_type
 */
#define ET_NONE         0
#define ET_REL          1
#define ET_EXEC         2
#define ET_DYN          3
#define ET_CORE         4

/*
 * e_machine
 */
#define EM_NONE         0       /* No machine */
#define EM_SPARC        2       /* SPARC */

/*
 * e_ident[EI_VERSION], e_version
 */
#define EV_NONE         0
#define EV_CURRENT      1

/**
 * ELF Section header. Code style from specification
 */
typedef struct {
	Elf32_Word sh_name;
	Elf32_Word sh_type;
	Elf32_Word sh_flags;
	Elf32_Addr sh_addr;
	Elf32_Off  sh_offset;
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize;
} Elf32_Shdr;

/**
 * ELF Program Segment header. Code style from specification
 */
typedef struct {
	Elf32_Word p_type;
	Elf32_Off  p_offset;
	Elf32_Addr p_vaddr;
	Elf32_Addr p_paddr;
	Elf32_Word p_filesz;
	Elf32_Word p_memsz;
	Elf32_Word p_flags;
	Elf32_Word p_align;
} Elf32_Phdr;

/**
 * Symbol table entry. Code style from specification
 */
typedef struct {
	Elf32_Word st_name;
	Elf32_Addr st_value;
	Elf32_Word st_size;
	uint8_t    st_info;
	uint8_t    st_other;
	Elf32_Half st_shndx;
} Elf32_Sym;

/**
 * Relocation table entry in sections with SHT_REL type.
 * Code style from specification
 */
typedef struct {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
} Elf32_Rel;

/**
 * Relocation table entry in sections with SHT_RELA type.
 * Code style from specification
 */
typedef struct{
	Elf32_Addr  r_offset;
	Elf32_Word  r_info;
	Elf32_Sword r_addend;
} Elf32_Rela;

#if 0
/*Collection of information about elf*/
typedef struct {
	Elf32_Ehdr header;
	Elf32_Shdr sectionHeaders[MAX_NUMBER_OF_SECTIONS];
	Elf32_Phdr segmentHeaders[MAX_NUMBER_OF_SECTIONS];
	char stringTable[MAX_STRING_TABLE_LENGTH];
} ElfFile;
#endif

#endif /* LIB_ELF_TYPES_H_ */

