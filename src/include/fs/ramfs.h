/**
 * @file
 *
 * @date 29.06.2009
 * @author Anton Bondarev
 */
#ifndef FS_RAMFS_H_
#define FS_RAMFS_H_

#include <fs/rootfs.h>

typedef struct _RAMFS_CREATE_PARAM {
	unsigned long start_addr;
	size_t size;
	char name[CONFIG_MAX_LENGTH_FILE_NAME];
	unsigned int mode;
} RAMFS_CREATE_PARAM;

typedef struct _FILE_DESC {
        unsigned long start_addr;
        unsigned int size;
        char name[CONFIG_MAX_LENGTH_FILE_NAME];
        unsigned int mode;
        unsigned int is_busy;
} FILE_DESC;

typedef struct _FILE_HANDLER {
        FILEOP *fileop;
        FILE_DESC *fdesc;
        int cur_pointer;
        unsigned int mode;
} FILE_HANDLER;

extern FSOP_DESCRIPTION ramfsop;

extern int init_rootfs(void);

#endif /* FS_RAMFS_H_ */
