/*
 * ramdisk.c
 *
 *  Created on: 27.03.2012
 *      Author: Andrey Gazukin
 */

#include <types.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fs/ramfs.h>
#include <fs/fs.h>
#include <fs/vfs.h>
#include <fs/node.h>
#include <fs/ramdisk.h>
#include <fs/driver_registry.h>
#include <util/array.h>
#include <embox/unit.h>
#include <stdlib.h>
#include <mem/page.h>

static ramdisk_t ramdisk;

EMBOX_UNIT_INIT(unit_init);

int ramdisk_create(const char *name, unsigned size, const char *name_fs) {
	node_t *ramdisk_node;

	if (NULL == (ramdisk_node = vfs_add_path(name, NULL))) {
		printf("Ramdisk already exist\n");
		return 0;/*file already exist*/
		};

	memcpy ((void *)&ramdisk.name, (const void *)name, (size_t)strlen(name));
	ramdisk.size = size * CONFIG_PAGE_SIZE;
	memcpy ((void *)&ramdisk.fs_type,\
			(const void *)name_fs, (size_t)strlen(name_fs));

	if(NULL == (ramdisk.addr = page_alloc(size))){
		printf("Out of memory\n");
	}
	else {
		printf("Create ramdisk %s, size %d, filesistem %s\n",\
				ramdisk.name, ramdisk.size, ramdisk.fs_type);
	}
	return 0;
}

static int unit_init(void) {
	return 0;
}
