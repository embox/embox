/**
 * @file
 * @brief
 *
 * @date 26.10.2012
 * @author Anton Bulychev
 */

#include <lib/libelf.h>
#include <string.h>
#include <errno.h>
#include <types.h>

/* TODO: Replace it in arch!!! */

#define R_386_NONE      0
#define R_386_32        1
#define R_386_PC32      2
#define R_386_GOT32     3
#define R_386_PLT32     4
#define R_386_COPY      5
#define R_386_GLOB_DAT  6
#define R_386_JMP_SLOT  7
#define R_386_RELATIVE  8
#define R_386_GOTOFF    9
#define R_386_GOTPC     10

static inline int elf_relocate_section_rel(FILE *fd, Elf32_Obj *obj,
		Elf32_Addr *offsets, Elf32_Shdr *relsh) {

	Elf32_Rel  *rel;
	Elf32_Sym  *sym;
	Elf32_Addr sym_addr;
	Elf32_Addr *where;
	int rel_count = elf_read_rel_section(fd, obj, relsh, &rel);

	for (int i = 0 ; i < rel_count; i++) {
		sym = &obj->sym_table[ELF32_R_SYM(rel[i].r_info)];
		sym_addr = offsets[sym->st_shndx] + sym->st_value;
		where = (Elf32_Addr *) (offsets[relsh->sh_info] + rel[i].r_offset);

		switch (ELF32_R_TYPE(rel[i].r_info)) {
		case R_386_NONE:
			break;
		case R_386_32:
			*where += sym_addr;
			break;
		case R_386_PC32:
			*where = sym_addr - (Elf32_Addr)where;
			break;
		default:
			return -ENOSYS;
		}
	}

	return ENOERR;
}

static inline int elf_relocate_section_rela(FILE *fd, Elf32_Obj *obj,
	uint32_t *offsets, Elf32_Shdr *relash) {
	/* Have not implemented yet */
	return -ENOSYS;
}

int elf_relocate(FILE *fd, Elf32_Obj *obj, uint32_t *offsets) {
	Elf32_Shdr *sh;
	int err;

	for (int i = 0; i < obj->header->e_shnum; i++) {
		sh = &obj->sh_table[i];

		switch (sh->sh_type) {
		case SHT_REL:
			if ((err = elf_relocate_section_rel(fd, obj, offsets, sh)) < 0) {
				return err;
			}
			return ENOERR;
			break;
		case SHT_RELA:
			if ((err = elf_relocate_section_rela(fd, obj, offsets, sh)) < 0) {
				return err;
			}
			break;
		}
	}

	return ENOERR;
}
