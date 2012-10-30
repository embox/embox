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
#include <lib/elf_consts.h>

#define __libelf_u32_t    __u32
#define __libelf_u16_t    __u16
#define __libelf_i32_t    __s32

typedef __libelf_u32_t    Elf32_Addr;
typedef __libelf_u16_t    Elf32_Half;
typedef __libelf_u32_t    Elf32_Off;
typedef __libelf_i32_t    Elf32_Sword;
typedef __libelf_u32_t    Elf32_Word;

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

typedef struct {
	Elf32_Sword d_tag;
	union {
		Elf32_Word d_val;
		Elf32_Addr d_ptr;
	} d_un;
} Elf32_Dyn;

/* Collection of information about elf. */
typedef struct {
	FILE         *fd;

	/* Should we reverse bytes when reading */
	int          need_reverse;

	/* Header of the file */
	Elf32_Ehdr   *header;

	/* Section table */
	Elf32_Shdr   *sh_table;

	/* Program table */
	Elf32_Phdr   *ph_table;

	/* String table. Contains section names */
	char         *string_table;

	/* Symbol table */
	Elf32_Sym    *sym_table;
	/* Section header index of the associated string table for symbol table */
	int          sym_names_shidx;

	/* Symbol names */
	char         *sym_names;
	unsigned int sym_count;

	/* Elf .dynamic section */
	Elf32_Dyn    *dyn_section;
	unsigned int dyn_count;

	/* Elf relocation array */
	Elf32_Rel    *rel_array;
	unsigned int rel_count;

	Elf32_Addr   load_offset; /* For shared libraries */
} Elf32_Obj;

typedef struct elf_objlist_item {
	Elf32_Obj                *obj;
	struct elf_objlist_item  *next;
} Elf32_Objlist_item;

typedef struct {
	Elf32_Objlist_item *first;
} Elf32_Objlist;

#endif /* LIB_ELF_TYPES_H_ */
