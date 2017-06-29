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

int elf_read_sh_table(int fd, Elf32_Ehdr *header, Elf32_Shdr *sh_table) {
	size_t size = header->e_shnum * header->e_shentsize;

	lseek(fd, header->e_shoff, SEEK_SET);
	if (size != read(fd, sh_table, size)) {
		return -EBADF;
	}

	return ENOERR;
}

int elf_read_section(int fd, Elf32_Shdr *sh, void *addr) {
	lseek(fd, sh->sh_offset, SEEK_SET);
	if (sh->sh_size != read(fd, addr, sh->sh_size)) {
		return -EBADF;
	}

	return ENOERR;
}

int elf_read_section_by_index(int fd, Elf32_Ehdr *header, Elf32_Shdr *s_header, unsigned short index) {
	int offset = header->e_shoff + index * sizeof(Elf32_Shdr);

	lseek(fd, offset, SEEK_SET);
	if (sizeof(Elf32_Shdr) != read(fd, s_header, sizeof(Elf32_Shdr))) {
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

int elf_read_interp(int fd, Elf32_Phdr *ph, char *interp) {
	lseek(fd, ph->p_offset, SEEK_SET);
	if (ph->p_filesz != read(fd, interp, ph->p_filesz)) {
		return -EBADF;
	}

	return ENOERR;
}
