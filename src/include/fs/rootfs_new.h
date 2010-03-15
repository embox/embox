/**
 * @file
 *
 * @date 21.07.2009
 * @author Roman Evstifeev
 */
#ifndef ROOTFS_H_
#define ROOTFS_H_

#include <fsop.h>
#include <stdio.h>

typedef struct  {
	const char* mountpath;
	const FSOP* fsop;
} MNT_FSOP;

#define FS_LIST_SIZE    array_len(mnt_fops)

typedef struct  {
	short fs_idx;
	short fs_file_idx;
	int cur_pos;
} OPENED_FILE;

#define MAX_OPENED_FILES 100

#define IS_FDESC_VALID(fdesc) ((fdesc>=0) && (fdesc<MAX_OPENED_FILES)) ? true : false
#define IS_OFILE_USED(ofile) (ofile.fs_idx!=IDX_INVALID) ? true : false
#define RELEASE_OFILE(ofile) ofile.fs_idx=IDX_INVALID

static short get_fs_idx_by_path(const char *path);

#endif /* ROOTFS_H_ */
