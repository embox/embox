/**
 * @file
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
 */

#ifndef RAMDISK_H_
#define RAMDISK_H_


#include <stddef.h>
//#include <limits.h>

struct node;

typedef struct ramdisk {
//	int               idx;

	struct block_dev *bdev;
//	size_t            size;
	char             *p_start_addr;
	size_t            blocks;
	size_t            block_size;
//	const char        path[PATH_MAX];
} ramdisk_t;

extern struct ramdisk *ramdisk_create(char *path, size_t size);
extern struct ramdisk *ramdisk_get_param(char *path);
extern int ramdisk_delete(const char *name);

#endif /* RAMDISK_H_ */
