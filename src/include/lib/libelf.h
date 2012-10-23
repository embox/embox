/**
 * @file
 * @brief ELF (Executable and Linkable Format) parser.
 *
 * @date 27.04.10
 * @author Aleksandr Kirov
 */

#ifndef LIB_ELF_H_
#define LIB_ELF_H_

#include <lib/elf_types.h>
#include <hal/arch.h>

#if defined(__LITTLE_ENDIAN)
#define NEED_REVERSE(rev) \
	((rev) == ELFDATA2MSB)
#elif defined(__BIG_ENDIAN)
#define NEED_REVERSE(rev) \
	((rev) == ELFDATA2LSB)
#endif


#define __REV_L(num) \
	((((num) & 0x000000ff) << 24)   \
	+ (((num) & 0x0000ff00) << 8)   \
	+ (((num) & 0x00ff0000) >> 8)   \
	+ (((num) & 0xff000000) >> 24))

#define __REV_S(num) \
	((((num) & 0x00ff ) << 8)  \
	+ (((num) & 0xff00) >> 8))

#define REVERSE_L(num) num = __REV_L(num)
#define REVERSE_S(num) num = __REV_S(num)


#if 0
/**
 * Function reverses order of bytes in received integer with size 4-bytes
 * if reversed value is 2 - according
 * specification and big-low type of bytes order in numbers
 *
 * @param num - integer to reverse
 * @param reversed - integer from specification
 *                 - property talking about data type
 */

#define L_REV(num, rev) \
	(NEED_REV(rev) ?                     \
	((((num) & 0x000000ff) << 24)   \
	+ (((num) & 0x0000ff00) << 8)   \
	+ (((num) & 0x00ff0000) >> 8)   \
	+ (((num) & 0xff000000) >> 24)) \
	: (num))

/**
 * Function reverses order of bytes in received integer with size 2-bytes
 * if reversed value is 2 - according
 * specification and big-low type of bytes order in numbers
 *
 * @param num - integer to reverse
 * @param reversed - integer from specification
 *                 - property talking about data type
 */
#define S_REV(num, rev) \
	(NEED_REV(rev) ?                \
	((((num) & 0x00ff ) << 8)  \
	+ (((num) & 0xff00) >> 8)) \
	: (num))

#endif

extern void elf_initialize_object(Elf32_Obj *obj);
extern void elf_finilize_object(Elf32_Obj *obj);

extern int elf_read_header(FILE *fd, Elf32_Obj *obj);
extern int elf_read_section_header_table(FILE *fd, Elf32_Obj *obj);
extern int elf_read_program_header_table(FILE *fd, Elf32_Obj *obj);

extern int elf_read_section(FILE *fd, Elf32_Shdr *sh, char **dst);

extern int elf_read_string_table(FILE *fd, Elf32_Obj *obj);
extern int elf_read_symbol_table(FILE *fd, Elf32_Obj *obj);
extern int elf_read_symbol_names(FILE *fd, Elf32_Obj *obj);
extern int elf_read_dynamic_section(FILE *fd, Elf32_Obj *obj);
extern int elf_read_rel_table(FILE *fd, Elf32_Obj *obj);


extern int32_t elf_read_rela_table(FILE *fd, Elf32_Ehdr *header,
				Elf32_Shdr *section_header_table,
				Elf32_Rela *rela_table, int32_t *count);

extern int32_t elf_read_segment(FILE *fd, Elf32_Phdr *program_header, int8_t *dst);


#endif /* LIB_ELF_H_ */
