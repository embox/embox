/**
 * @file
 * @brief
 *
 * @date 31.07.2012
 * @author Andrey Gazukin
 */

#include <stdio.h>
#include <string.h>

#include <fs/fs_drv.h>
#include <fs/vfs.h>
#include <util/array.h>
#include <embox/block_dev.h>

ARRAY_SPREAD_DEF(const block_dev_module_t, __block_dev_registry);

block_dev_module_t *block_dev_find(char *name) {
	block_dev_module_t *b_dev;
	size_t i;

	for(i = 0; i < ARRAY_SPREAD_SIZE(__block_dev_registry); i++) {
		if(!strcmp(__block_dev_registry[i].name, name)) {
			b_dev = (block_dev_module_t *) &(__block_dev_registry[i]);
			return b_dev;
		}
	}

	return NULL;
}

