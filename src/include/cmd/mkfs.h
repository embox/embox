/**
 * @file
 * @brief
 *
 * @date 03.04.2012
 * @author Andrey Gazukin
 */

#ifndef MKFS_H_
#define MKFS_H_

#include <fs/node.h>

#define MKFS_CREATE_RAMDISK 0x00000001
#define MKFS_FORMAT_DEV     0x00000002

typedef struct mkfs_params {
	size_t       blocks;
	char         path[MAX_LENGTH_PATH_NAME];
	const char   fs_name[MAX_LENGTH_FILE_NAME];
	unsigned int fs_type;
	unsigned int operation_flag;
} mkfs_params_t;

extern int mkfs_do_operation(void *_mkfs_params);

#endif /* MKFS_H_ */
