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
	char *start_addr;
	size_t   size;
	size_t   blocks;
	const char name[CONFIG_MAX_LENGTH_FILE_NAME];
	const char fs_name[CONFIG_MAX_LENGTH_FILE_NAME];
	unsigned int fs_type;
} ramdisk_params_t;

extern int ramdisk_create(void *mkfs_params);

#endif /* RAMDISK_H_ */
