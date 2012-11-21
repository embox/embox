/**
 * @file
 * @brief
 *
 * @date 23.10.2012
 * @author Anton Bulychev
 */

#include <lib/libelf.h>
#include <stdlib.h>
#include <errno.h>

int elf_read_section(FILE *fd, Elf32_Shdr *sh, char **dst) {
	if (!sh) {
		return -EINVAL;
	}

	if (!(*dst = malloc(sh->sh_size))) {
		return -ENOMEM;
	}

	fseek(fd, sh->sh_offset, 0);

	if (sh->sh_size != fread(*dst, sh->sh_size, 1, fd)) {
		return -EBADF;
	}

	return sh->sh_size;
}
