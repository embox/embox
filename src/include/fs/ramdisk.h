/*
 * ramdisk.h
 *
 *  Created on: 27.03.2012
 *      Author: Andrey Gazukin
 */

#ifndef RAMDISK_H_
#define RAMDISK_H_

#include <types.h>
#include <fs/node.h>

#define RAMDISK_BLOCK_SIZE  0x400;

typedef struct ramdisk {
	const char   name[CONFIG_MAX_LENGTH_FILE_NAME];
	unsigned size;
	const char   fs_type[CONFIG_MAX_LENGTH_FILE_NAME];
	u16_t *addr;
} ramdisk_t;

extern int ramdisk_create(const char *name, unsigned size, const char *name_fs);

#endif /* RAMDISK_H_ */
