/**
 * @file
 *
 * @date 29.06.2009
 * @author Anton Bondarev
 */
#ifndef FS_RAMFS_H_
#define FS_RAMFS_H_

#include <fs/rootfs.h>

typedef struct _ramfs_create_param {
	unsigned long start_addr;
	size_t        size;
	char          name[CONFIG_MAX_LENGTH_FILE_NAME];
	unsigned int  mode;
	unsigned int  mtime;
} ramfs_create_param_t;

typedef struct _ramfs_file_description {
        unsigned long start_addr;
        unsigned int  size;
        unsigned int  mode;
        unsigned int  mtime;
        int           cur_pointer;
        int           lock;
} ramfs_file_description_t;

extern int unpack_to_rootfs(void);

#endif /* FS_RAMFS_H_ */
