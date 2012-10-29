/**
 * @file
 * @brief
 *
 * @date 26.10.2012
 * @author Anton Bulychev
 */

#include <types.h>

#include <lib/dl.h>

void dl_init_info(dl_info *info) {
	info->globsym_count = 0;

	info->data_list     = NULL;
	info->globsym_table = NULL;
}

void dl_finilize_info(dl_info *info) {
	dl_data *data;

	if (info->globsym_table) free(info->globsym_table);
	if (info->data_list) {
		data
		while ()
	}


	info->globsym_count = 0;
	info->data_list     = NULL;
	info->globsym_table = NULL;
}
