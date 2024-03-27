/**
 * @
 *
 * @date Dec 23, 2019
 * @author Anton Bondarev
 */

#include <errno.h>
#include <string.h>

#include <fs/inode.h>

#include <mem/misc/pool.h>

#include <framework/mod/options.h>

#include "initfs.h"

POOL_DEF(initfs_file_pool, struct initfs_file_info, OPTION_GET(NUMBER,file_quantity));

struct initfs_file_info *initfs_alloc_inode(void) {
	return pool_alloc(&initfs_file_pool);
}

void initfs_free_inode(struct initfs_file_info *fi) {
	pool_free(&initfs_file_pool, fi);
}
