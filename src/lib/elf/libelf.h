/**
 * @file
 * @brief ELF (Executable and Linkable Format) reader.
 *
 * @date 15.02.2013
 * @author Anton Bulychev
 */

#ifndef LIB_ELF_H_
#define LIB_ELF_H_

#include <lib/elf_consts.h>
#include <lib/elf_types.h>

extern int elf_read_header(int fd, Elf32_Ehdr *header);

int elf_read_sh_table(int fd, Elf32_Ehdr *header, Elf32_Shdr *sh_table);
int elf_read_section(int fd, Elf32_Shdr *sh, void *addr);
int elf_read_section_by_index(int fd, Elf32_Ehdr *header, Elf32_Shdr *s_header, unsigned short index);

extern int elf_read_ph_table(int fd, Elf32_Ehdr *header, Elf32_Phdr *ph_table);
extern int elf_read_segment(int fd, Elf32_Phdr *ph, void *addr);

extern int elf_read_interp(int fd, Elf32_Phdr *ph, char *interp);

#endif /* LIB_ELF_H_ */
