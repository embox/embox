/**
 * @file
 * @brief
 *
 * @date 26.11.2012
 * @author Anton Bulychev
 */

#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <lib/libelf.h>

#define MASK (0x1000UL - 1)

int elf_read_segment(int fd, Elf32_Phdr *ph, void *addr) {
	lseek(fd, ph->p_offset, SEEK_SET);
	if (ph->p_filesz != read(fd, addr, ph->p_filesz)) {
		return -EBADF;
	}

	if (ph->p_memsz > ph->p_filesz) {
		memset(addr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
	}

	return ENOERR;
}
