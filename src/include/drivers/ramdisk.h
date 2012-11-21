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

typedef struct ramdisk_create_params {
	size_t       size;
	char         *path;
	const char   *fs_name;
	unsigned int fs_type;
} ramdisk_create_params_t;

typedef struct ramdisk {
	int             idx;
	node_t         *dev_node;
	dev_t          *dev_id;
	size_t          size;
	char           *p_start_addr;
	size_t          blocks;
	size_t          block_size;
	const char      path[MAX_LENGTH_PATH_NAME];
	const char      fs_name[MAX_LENGTH_FILE_NAME];
	unsigned int    fs_type;
} ramdisk_t;

extern int ramdisk_create(void *params);
extern ramdisk_t *ramdisk_get_param(char *path);
extern int ramdisk_delete(const char *name);

#endif /* RAMDISK_H_ */
