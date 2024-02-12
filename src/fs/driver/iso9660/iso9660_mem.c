/**
 * @file
 * @brief
 *
 * @date 20.08.2012
 * @author Andrey Gazukin
 */

#include <mem/misc/pool.h>

#include <fs/iso9660.h>

#include <framework/mod/options.h>


/* cdfs filesystem description pool */
POOL_DEF(cdfs_fs_pool, struct cdfs_fs_info, OPTION_GET(NUMBER,cdfs_descriptor_quantity));

/* cdfs file description pool */
POOL_DEF(cdfs_file_pool, struct cdfs_file_info, OPTION_GET(NUMBER,inode_quantity));


struct cdfs_fs_info *iso9660_fsi_alloc(void) {
   return pool_alloc(&cdfs_fs_pool);
}

void iso9660_fsi_free(struct cdfs_fs_info *fs) {
   return pool_free(&cdfs_fs_pool, fs);
}

struct cdfs_file_info *iso9660_fi_alloc(void) {
   return pool_alloc(&cdfs_file_pool);
}

void iso9660_fi_free(struct cdfs_file_info *fs) {
   return pool_free(&cdfs_file_pool, fs);
}
