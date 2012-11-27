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

int elf_read_ph_table(int fd, Elf32_Ehdr *header, Elf32_Phdr *ph_table) {
	size_t size = header->e_phnum * header->e_phentsize;

	lseek(fd, header->e_phoff, SEEK_SET);
	if (size != read(fd, ph_table, size)) {
		return -EBADF;
	}

	return ENOERR;
}
