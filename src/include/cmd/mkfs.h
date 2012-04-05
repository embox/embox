/*
 * mkfs.h
 *
 *  Created on: 03.04.2012
 *      Author: Andrey Gazukin
 */

#ifndef MKFS_H_
#define MKFS_H_

typedef struct mkfs_params {
	size_t      blocks;
	char        name[CONFIG_MAX_LENGTH_FILE_NAME];
	const char  fs_name[CONFIG_MAX_LENGTH_FILE_NAME];
	unsigned int fs_type;
} mkfs_params_t;

#endif /* MKFS_H_ */
