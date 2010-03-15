/**
 * @file
 *
 * @date 29.06.2009
 * @author Anton Bondarev
 */
#ifndef FS_RAMFS_H_
#define FS_RAMFS_H_

typedef struct _RAMFS_CREATE_PARAM {
	unsigned int start_addr;
	size_t size;
	char name[CONFIG_MAX_LENGTH_FILE_NAME];
	unsigned int mode;
}RAMFS_CREATE_PARAM;

extern FSOP_DESCRIPTION ramfsop;

#endif /* FS_RAMFS_H_ */
