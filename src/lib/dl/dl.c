/**
 * @file
 * @brief
 *
 * @date 30.10.2012
 * @author Anton Bulychev
 */

#include <stdlib.h>

#include <lib/libelf.h>
#include <lib/libdl.h>

static inline void dl_init_data(dl_data *data) {
	*data = (dl_data) {
		.globsym_table = NULL,
		.globsym_count = 0,
	};
}

static inline int dl_link(Elf32_Objlist *list, dl_data *data) {
	int err;

	if ((err = dl_fetch_global_symbols(list, data)) < 0) {
		return err;
	}

	return dl_proceed_all_relocations(list, data);
}

int dl_proceed(Elf32_Objlist *list, dl_data **data) {
	if (!(*data = malloc(sizeof(dl_data)))) {
		return -ENOMEM;
	}

	dl_init_data(*data);

	return dl_link(list, *data);
}

void dl_free_data(dl_data *data) {
	if (data->globsym_table) free(data->globsym_table);
	free(data);
}
