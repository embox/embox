/**
 * @file
 * @brief ELF (Executable and Linkable Format) parser.
 *
 * @date 27.04.10
 * @author Aleksandr Kirov
 */

#ifndef LIB_ELF_H_
#define LIB_ELF_H_

#include <hal/arch.h>

#include <lib/elf_consts.h>
#include <lib/elf_types.h>

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

extern int elf_object_init(Elf32_Obj **obj, FILE *fd);
extern void elf_object_fini(Elf32_Obj *obj);

extern int elf_read_header(Elf32_Obj *obj);
extern int elf_read_section_header_table(Elf32_Obj *obj);
extern int elf_read_program_header_table(Elf32_Obj *obj);

extern int elf_read_section(Elf32_Obj *obj, Elf32_Shdr *sh, char **dst);

extern int elf_read_string_table(Elf32_Obj *obj);
extern int elf_read_symbol_table(Elf32_Obj *obj);
extern int elf_read_symbol_names(Elf32_Obj *obj);
extern int elf_read_dynamic_section(Elf32_Obj *obj);

extern Elf32_Addr elf_get_symbol_addr(Elf32_Obj *obj, Elf32_Sym *sym);

extern int elf_read_rel_section(Elf32_Obj *obj, Elf32_Shdr *sh,
		Elf32_Rel **rel);
extern void elf_remove_rel_section(Elf32_Rel *rel);

extern int elf_read_rel_table(Elf32_Obj *obj);


extern int32_t elf_read_rela_table(FILE *fd, Elf32_Ehdr *header,
				Elf32_Shdr *section_header_table,
				Elf32_Rela *rela_table, int32_t *count);

extern int32_t elf_read_segment(Elf32_Phdr *program_header, int8_t *dst);


extern int elf_objlist_init(Elf32_Objlist **list);
extern int elf_objlist_add(Elf32_Objlist *list, Elf32_Obj *obj);


#endif /* LIB_ELF_H_ */
