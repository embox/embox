/**
 * @file
 *
 * @date Oct 27, 2019
 * @author Anton Bondarev
 */
#include <mem/misc/pool.h>

#include <fs/fat.h>

#include <framework/mod/options.h>

#define FAT_DESC_QUANTITY    OPTION_GET(NUMBER, fat_descriptor_quantity)
#define FAT_INODE_QAUNTITY   OPTION_GET(NUMBER, inode_quantity)

POOL_DEF(fat_fs_pool, struct fat_fs_info, FAT_DESC_QUANTITY);
POOL_DEF(fat_file_pool, struct fat_file_info, FAT_INODE_QAUNTITY);
POOL_DEF(fat_dirinfo_pool, struct dirinfo, FAT_INODE_QAUNTITY);

struct fat_fs_info *fat_fs_alloc(void) {
	return pool_alloc(&fat_fs_pool);
}

void fat_fs_free(struct fat_fs_info *fsi) {
	pool_free(&fat_fs_pool, fsi);
}

struct fat_file_info *fat_file_alloc(void) {
	return pool_alloc(&fat_file_pool);
}

void fat_file_free(struct fat_file_info *fi) {
	pool_free(&fat_file_pool, fi);
}

struct dirinfo *fat_dirinfo_alloc(void) {
	return pool_alloc(&fat_dirinfo_pool);
}

void fat_dirinfo_free(struct dirinfo *di) {
	pool_free(&fat_dirinfo_pool, di);
}
