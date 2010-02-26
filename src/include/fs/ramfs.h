/**
 * @file
 *
 * @date 29.06.2009
 * @author Anton Bondarev
 */
#ifndef RAMFS_H_
#define RAMFS_H_

#define MAX_LENCTH_FILE_NAME 0x10
typedef struct _RAMFS_CREATE_PARAM {
	unsigned int start_addr;
	unsigned int size;
	char name[MAX_LENCTH_FILE_NAME];
	unsigned int mode;
}RAMFS_CREATE_PARAM;

extern FSOP_DESCRIPTION ramfsop;

#endif /* RAMFS_H_ */
