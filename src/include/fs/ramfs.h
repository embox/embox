/**
 * @file
 *
 * @date 29.06.09
 * @author Anton Bondarev
 */

#ifndef FS_RAMFS_H_
#define FS_RAMFS_H_

#include <fs/node.h>


typedef struct _ramfs_create_param {
	struct node *root_node;
	unsigned long start_addr;
	size_t        size;
	char          name[MAX_LENGTH_FILE_NAME];
	unsigned int  mode;
	unsigned int  mtime;
} ramfs_create_param_t;


extern int cpio_unpack(char *pathto);

#endif /* FS_RAMFS_H_ */
