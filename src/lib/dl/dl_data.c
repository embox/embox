/**
 * @file
 * @brief
 *
 * @date 26.10.2012
 * @author Anton Bulychev
 */

#include <string.h>
#include <errno.h>
#include <types.h>

#include <lib/dl.h>

int dl_load_data(dl_data *data, const char *filename) {
	FILE *fd = fopen(filename, "r");
	if (fd == NULL) {
		return -EINVAL;
	}

	return dl_init_data(data, fd);
}

int dl_init_data(dl_data *data, FILE *fd) {
	int err;

	if (fd == NULL) {
		return -EINVAL;
	}

	data->next = NULL;
	data->fd = fd;

	elf_initialize_object(data->obj);

	if ((err = elf_read_header(fd, data->obj)) < 0) {
		return err;
	}

	// FIXME: Other depends on type of elf

	if ((err = elf_read_section_header_table(fd, data->obj)) < 0) {
		return err;
	}

	if ((err = elf_read_symbol_table(fd, data->obj)) < 0) {
		return err;
	}

	if ((err = elf_read_symbol_names(fd, data->obj)) < 0) {
		return err;
	}

	return ENOERR;
}

void dl_finilize_data(dl_data *data) {
	fclose(data->fd);
	elf_finilize_obj(data->obj);
}
