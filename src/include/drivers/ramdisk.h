/**
 * @file
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
 */

#ifndef RAMDISK_H_
#define RAMDISK_H_

#include <fs/node.h>
#include <stddef.h>

typedef struct ramdisk {
	int             idx;
	node_t         *dev_node;
	struct block_dev *bdev;
	size_t          size;
	char           *p_start_addr;
	size_t          blocks;
	size_t          block_size;
	const char      path[MAX_LENGTH_PATH_NAME];
	const char      fs_name[MAX_LENGTH_FILE_NAME];
	unsigned int    fs_type;
} ramdisk_t;

extern int ramdisk_create(char *path, size_t size);
extern ramdisk_t *ramdisk_get_param(char *path);
extern int ramdisk_delete(const char *name);

#endif /* RAMDISK_H_ */
