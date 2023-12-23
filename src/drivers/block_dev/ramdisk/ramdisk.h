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

struct ramdisk {
	struct block_dev *bdev;
	char *p_start_addr;
	int idx;
};

extern struct ramdisk *ramdisk_create(char *path, size_t size);

extern int ramdisk_delete(const char *name);

#endif /* RAMDISK_H_ */
