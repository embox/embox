/**
 * @file ramdisk.h
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
 */

#ifndef RAMDISK_H_
#define RAMDISK_H_

#include <types.h>
#include <fs/node.h>

#define RAMDISK_BLOCK_SIZE  0x400;
#define CONFIG_QUANTITY_RAMDISK 0x05

typedef struct ramdisk_params {
	char           *p_start_addr;
	size_t          size;
	size_t          blocks;
	const char      path[CONFIG_MAX_LENGTH_PATH_NAME];
	const char      fs_name[CONFIG_MAX_LENGTH_FILE_NAME];
	unsigned int    fs_type;
} ramdisk_params_t;

extern int ramdisk_create(void *mkfs_params);
extern ramdisk_params_t *ramdisk_get_param(char *name);

#endif /* RAMDISK_H_ */
