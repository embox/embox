/**
 * @file
 * @brief
 *
 * @date 15.02.2013
 * @author Anton Bulychev
 */

#include <errno.h>
#include <unistd.h>
#include <string.h>

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

int elf_read_interp(int fd, Elf32_Phdr *ph, char *interp) {
	lseek(fd, ph->p_offset, SEEK_SET);
	if (ph->p_filesz != read(fd, interp, ph->p_filesz)) {
		return -EBADF;
	}

	return ENOERR;
}

int elf_read_ph_table(int fd, Elf32_Ehdr *header, Elf32_Phdr *ph_table) {
	size_t size = header->e_phnum * header->e_phentsize;

	lseek(fd, header->e_phoff, SEEK_SET);
	if (size != read(fd, ph_table, size)) {
		return -EBADF;
	}

	return ENOERR;
}

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
