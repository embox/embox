/**
 * @file
 * @brief
 *
 * @date 26.11.2012
 * @author Anton Bulychev
 */

#include <errno.h>
#include <unistd.h>

#include <lib/libelf.h>

int elf_read_header(int fd, Elf32_Ehdr *header) {
	if (sizeof(Elf32_Ehdr) != read(fd, header, sizeof(Elf32_Ehdr))) {
		return -EBADF;
	}

	if (header->e_ident[EI_MAG0] != ELFMAG0 ||
			header->e_ident[EI_MAG1] != ELFMAG1 ||
			header->e_ident[EI_MAG2] != ELFMAG2 ||
			header->e_ident[EI_MAG3] != ELFMAG3) {
                return -EBADF;
	}

	return ENOERR;
}
