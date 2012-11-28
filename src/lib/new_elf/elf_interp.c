/**
 * @file
 * @brief
 *
 * @date 28.11.2012
 * @author Anton Bulychev
 */

#include <errno.h>
#include <unistd.h>

#include <lib/libelf.h>

int elf_read_interp(int fd, Elf32_Phdr *ph, char *interp) {
	lseek(fd, ph->p_offset, SEEK_SET);
	if (ph->p_filesz != read(fd, interp, ph->p_filesz)) {
		return -EBADF;
	}

	return ENOERR;
}
