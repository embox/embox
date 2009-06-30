/**
 * \file ramfs.h
 * \date Jun 29, 2009
 * \author anton
 * \details
 */
#ifndef RAMFS_H_
#define RAMFS_H_

typedef struct _RAMFS_CREATE_PARAM {
    unsigned int start_addr;
    unsigned int size;
    char name[MAX_LENCTH_FILE_NAME];
    unsigned int mode;
}RAMFS_CREATE_PARAM;

extern FSOP_DESCRIPTION ramfs_fsop;

#endif /* RAMFS_H_ */
