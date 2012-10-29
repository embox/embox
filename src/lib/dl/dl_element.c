/**
 * @file
 * @brief
 *
 * @date 29.10.2012
 * @author Anton Bulychev
 */

#include <errno.h>
#include <types.h>
#include <string.h>
#include <stdlib.h>

#include <lib/libdl.h>

static inline int dl_initialize_element(dl_element *element, FILE *fd);

int dl_load_element(dl_element *element, const char *filename) {
	FILE *fd = fopen(filename, "r");
	if (fd == NULL) {
		return -EINVAL;
	}

	return dl_initialize_element(element, fd);
}

static inline int dl_initialize_element(dl_element *element, FILE *fd) {
	int err;

	if (fd == NULL) {
		return -EINVAL;
	}

	element->next = NULL;
	element->fd = fd;

	if (!(element->obj = malloc(sizeof(Elf32_Obj)))) {
		return -ENOMEM;
	}

	elf_initialize_object(element->obj);

	if ((err = elf_read_header(fd, element->obj)) < 0) {
		return err;
	}

	// FIXME: Other depends on type of elf

	if ((err = elf_read_section_header_table(fd, element->obj)) < 0) {
		return err;
	}

	if ((err = elf_read_symbol_table(fd, element->obj)) < 0) {
		return err;
	}

	if ((err = elf_read_symbol_names(fd, element->obj)) < 0) {
		return err;
	}

	return ENOERR;
}

void dl_finilize_element(dl_element *element) {
	fclose(element->fd);
	elf_finilize_object(element->obj);
	free(element->obj);
}
