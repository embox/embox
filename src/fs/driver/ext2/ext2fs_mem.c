/**
 * @file
 * @brief ext file system
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */

#include <stddef.h>

#include <fs/ext2.h>

#include <mem/misc/pool.h>



/* ext filesystem description pool */
POOL_DEF(ext2_fs_pool, struct ext2_fs_info,
		OPTION_GET(NUMBER,ext2_descriptor_quantity));

/* ext file description pool */
POOL_DEF(ext2_file_pool, struct ext2_file_info,
		OPTION_GET(NUMBER,inode_quantity));

struct ext2_fs_info *ext2fs_fsi_alloc(void) {
	return pool_alloc(&ext2_fs_pool);
}

void ext2fs_fsi_free(struct ext2_fs_info *fsi) {
	pool_free(&ext2_fs_pool, fsi);
}

struct ext2_file_info *ext2_fi_alloc(void) {
	return pool_alloc(&ext2_file_pool);
}

void ext2_fi_free(struct ext2_file_info *fi) {
	pool_free(&ext2_file_pool, fi);
}