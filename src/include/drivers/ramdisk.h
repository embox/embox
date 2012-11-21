/**
 * @file
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

typedef struct dev_ramdisk {
	node_t         *dev_node;
	dev_t           devnum;
	size_t          size;
	char           *p_start_addr;
	size_t          blocks;
	size_t          sector_size;
	const char      path[MAX_LENGTH_PATH_NAME];
	const char      fs_name[MAX_LENGTH_FILE_NAME];
	unsigned int    fs_type;
} dev_ramdisk_t;

extern int ramdisk_create(void *mkfs_params);
extern dev_ramdisk_t *ramdisk_get_param(char *name);
extern int ramdisk_delete(const char *name);

#endif /* RAMDISK_H_ */
