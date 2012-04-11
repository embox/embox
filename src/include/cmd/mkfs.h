/*
 * mkfs.h
 *
 *  Created on: 03.04.2012
 *      Author: Andrey Gazukin
 */

#ifndef MKFS_H_
#define MKFS_H_

#define MKFS_CREATE_DEV 0x00000001
#define MKFS_FORMAT_DEV 0x00000002

typedef struct mkfs_params {
	size_t      blocks;
	char        name[CONFIG_MAX_LENGTH_FILE_NAME];
	const char  fs_name[CONFIG_MAX_LENGTH_FILE_NAME];
	unsigned int fs_type;
	unsigned int operation_flag;
} mkfs_params_t;

#endif /* MKFS_H_ */
