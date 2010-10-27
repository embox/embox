/**
 * @file
 * @brief ELF (Executable and Linkable Format) parser.
 *
 * @date 27.04.10
 * @author Aleksandr Kirov
 */

#ifndef LIB_ELF_READER_H_
#define LIB_ELF_READER_H_

#include <stdio.h>
#include <types.h>
#include <errno.h>

#define Elf32_Addr         unsigned long
#define Elf32_Half         unsigned short
#define Elf32_Off          unsigned long
#define Elf32_Sword        signed long
#define Elf32_Word         unsigned long

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

#if 0
/*Collection of information about elf*/
typedef struct {
	Elf32_Ehdr header;
	Elf32_Shdr sectionHeaders[MAX_NUMBER_OF_SECTIONS];
	Elf32_Phdr segmentHeaders[MAX_NUMBER_OF_SECTIONS];
	char stringTable[MAX_STRING_TABLE_LENGTH];
} ElfFile;
#endif

/**
 * Function reverses order of bytes in received integer with size 4-bytes
 * if reversed value is 2 - according
 * specification and big-low type of bytes order in numbers
 *
 * @param num - integer to reverse
 * @param reversed - integer from specification
 *                 - property talking about data type
 */
extern uint32_t elf_reverse_long(uint32_t num, uint8_t reversed);

/**
 * Function reverses order of bytes in received integer with size 2-bytes
 * if reversed value is 2 - according
 * specification and big-low type of bytes order in numbers
 *
 * @param num - integer to reverse
 * @param reversed - integer from specification
 *                 - property talking about data type
 */
extern uint16_t elf_reverse_short(uint16_t num, uint8_t reversed);

/**
 * Read header from fo and put it in header, returns error code
 *
 * @param fd - file descriptor to get information from
			   recomended : fopen(<file_name>,"rb")
 * @param header - head structure in file, describing main elements positions
 *
 * @return the number of items successfully read or error code.
 */
extern int32_t elf_read_header(FILE *fd, Elf32_Ehdr *header);

/**
 * Read table of section's headers
 *
 * @param fd - file descriptor to get information from
 * @param header - head structure in file, describing main elements positions
 * @param section_header_table - array where will section's headers be stored
 *
 * @return the number of items successfully read or error code.
 */
extern int32_t elf_read_sections_table(FILE *fd, Elf32_Ehdr *header,
					Elf32_Shdr *section_header_table);

/**
 * Read table of program segment's headers
 *
 * @param fd - file descriptor to get information from
 * @param header - head structure in file, describing main elements positions
 * @param segment_header_table - array where will section's headers be stored
 *
 * @return the number of items successfully read or error code.
 */
extern int32_t elf_read_segments_table(FILE *fd, Elf32_Ehdr *header,
					Elf32_Phdr *segment_header_table);

/**
 * Read string table from sections and put it into names,
 * real length - to length
 *
 * @param fd - file descriptor to get information from
 * @param header - head structure in file, describing main elements positions
 * @param section_header_table - array where func take info about sections
 * @param names - array where will be stored string, starts and ends with "\0"
 * @param length - really used bytes in names
 *
 * @return the number of items successfully read or error code.
 * @retval -1 - not found in sections, "length" must be 0
 * @retval -2 - section header's table is empty
 */
extern int32_t elf_read_string_table(FILE * fd, Elf32_Ehdr *header,
	Elf32_Shdr *section_header_table, int8_t *names, int32_t *length);

/**
 * Read symbol string table from sections and put it into names. Find
 * SHT_STRTAB and read it to array, number of symbols - to length. Symbol
 * string table contains names of symbols
 *
 * @param fd - file descriptor to get information from
 * @param header - head structure in file, describing main elements positions
 * @param section_header_table - array where func take info about sections
 * @param names - array where will be stored string, starts and ends with "/0"
 * 		  which are names of symbols - with is return value
 * @param section_names array with names of sections
 * @param ret_length - really used bytes in names
 *
 * @return really used bytes in names or error code.
 * @retval -1 - not found in sections, "length" must be 0
 * @retval -2 - section header's table is empty
 * @retval -3 - empty array with sections name
 */
extern int32_t elf_read_symbol_string_table(FILE *fd, Elf32_Ehdr *header,
		Elf32_Shdr *section_header_table,
		int8_t *sections_names,  int8_t *names, int32_t *ret_length);

/**
 * return name of string from names_array, starting from index index
 * and till first '/0', put it into name
 *
 * @param names_array - consist of strings(names)
 * @param index - index of substring in array
 * @param name - returns name in this array
 *
 * @return length of name
 * @retval 0 - more then MAX_NAME_LENGTH name - error
 * @retval >0 - real length
 */
extern int32_t read_name(int8_t * names_array, int32_t index, int8_t * name);

/**
 * Find SHT_SYMTAB and write it to array, number of symbols - to count
 *
 * @param fd - file descriptor to get information from
 * @param header - head structure in file, describing main elements positions
 * @param section_header_table - array where func take info about sections
 * @param symbol_table - array, will store structures describing symbols
 * @param count - really used number of structures stored
 *
 * @return the number of items successfully read or error code.
 * @retval -1 - not found in sections, "count" must be 0
 * @retval -2 - section header's table is empty
 */
extern int32_t elf_read_symbol_table(FILE *fd, Elf32_Ehdr * header,
		Elf32_Shdr * section_header_table,
		 Elf32_Sym * symbol_table, int32_t * count);

/**
 * find and read ALL rel structures from file and number of them puts to count
 *
 * @param fd - file descriptor to get information from
 * @param header - head structure in file, describing main elements positions
 * @param section_header_table - array where func take info about sections
 * @param rel_table - array, will store structures describing relocations
 * @param count - really used number of structures stored
 *
 * @return really used number of structures stored or error code.
 * @retval -1 - not found in sections, "count" must be 0
 * @retval -2 - section header's table is empty
 * @retval -3 - must be never returned
 */
extern int32_t elf_read_rel_table(FILE *fd, Elf32_Ehdr *header,
			Elf32_Shdr *section_header_table,
			Elf32_Rel *rel_table, int32_t *count);

/**
 * find and read ALL rela structures from file and number of them puts to count
 *
 * @param fd - file descriptor to get information from
 * @param header - head structure in file, describing main elements positions
 * @param section_header_table - array where func take info about sections
 * @param rela_table - array, will store structures describing relocations
 * @param count - really used number of structures stored
 *
 * @return really used number of structures stored or error code.
 * @retval -1 - not found in sections, "count" must be 0
 * @retval -2 - section header's table is empty
 */
extern int32_t elf_read_rela_table(FILE *fd, Elf32_Ehdr *header,
				Elf32_Shdr *section_header_table,
				Elf32_Rela *rela_table, int32_t *count);

#endif /* LIB_ELF_READER_H_ */

