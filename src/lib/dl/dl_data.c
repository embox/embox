/**
 * @file
 * @brief
 *
 * @date 29.10.2012
 * @author Anton Bulychev
 */

#include <types.h>
#include <errno.h>
#include <stdlib.h>

#include <lib/libdl.h>

void dl_initialize_data(dl_data *data) {
	data->globsym_count = 0;

	data->element_list  = NULL;
	data->globsym_table = NULL;
}

int dl_add_file(dl_data *data, const char *filename) {
	dl_element *element;
	int err;

	if (!(element = malloc(sizeof(dl_element)))) {
		return -ENOMEM;
	}

	if ((err = dl_load_element(element, filename)) < 0) {
		return err;
	}

	element->next = data->element_list;
	data->element_list = element;

	return ENOERR;
}

void dl_finilize_data(dl_data *data) {
	dl_element *element, *next;

	if (data->globsym_table) free(data->globsym_table);
	if (data->element_list) {
		element = data->element_list;
		while (element) {
			next = element->next;
			dl_finilize_element(element);
			free(element);
			element = next;
		}
	}

	data->globsym_count = 0;
	data->element_list  = NULL;
	data->globsym_table = NULL;
}
