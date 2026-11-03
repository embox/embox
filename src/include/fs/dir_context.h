/**
 * @file
 *
 * @date Dec 26, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_INCLUDE_FS_DIR_CONTEXT_H_
#define SRC_INCLUDE_FS_DIR_CONTEXT_H_

struct dir_ctx {
	int   flags;
	void *fs_ctx;
	/* The reader's position, not the directory's: readers do not share
	 * it. */
	unsigned long fs_pos[3];
};

#endif /* SRC_INCLUDE_FS_DIR_CONTEXT_H_ */
