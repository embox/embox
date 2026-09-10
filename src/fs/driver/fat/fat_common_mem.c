/**
 * @file
 *
 * @date Oct 27, 2019
 * @author Anton Bondarev
 */
#include <mem/misc/pool.h>

#include <fs/fat.h>
#include <fs/fat_pool_use.h>

#include <framework/mod/options.h>

#define FAT_DESC_QUANTITY    OPTION_GET(NUMBER, fat_descriptor_quantity)
#define FAT_INODE_QAUNTITY   OPTION_GET(NUMBER, inode_quantity)

POOL_DEF(fat_fs_pool, struct fat_fs_info, FAT_DESC_QUANTITY);
POOL_DEF(fat_file_pool, struct fat_file_info, FAT_INODE_QAUNTITY);
POOL_DEF(fat_dirinfo_pool, struct dirinfo, FAT_INODE_QAUNTITY);

/* How many of each are out on loan, and how many times a request was
 * refused. A pool that runs out surfaces as -ENOMEM from fat_create and
 * then as a plain failed open(), which says nothing about the cause. */
struct fat_pool_use fat_pool_use = {
	.fs_max = FAT_DESC_QUANTITY,
	.file_max = FAT_INODE_QAUNTITY,
	.dirinfo_max = FAT_INODE_QAUNTITY,
};

/* Filled in by the guard in fat_fops.c; see fat_pool_use.h. */
struct fat_null_priv fat_null_priv;

struct fat_fs_info *fat_fs_alloc(void) {
	struct fat_fs_info *p = pool_alloc(&fat_fs_pool);

	if (p) {
		fat_pool_use.fs_live++;
	}
	else {
		fat_pool_use.fs_denied++;
	}
	return p;
}

void fat_fs_free(struct fat_fs_info *fsi) {
	if (fat_pool_use.fs_live) {
		fat_pool_use.fs_live--;
	}
	pool_free(&fat_fs_pool, fsi);
}

struct fat_file_info *fat_file_alloc(void) {
	struct fat_file_info *p = pool_alloc(&fat_file_pool);

	if (p) {
		fat_pool_use.file_live++;
		if (fat_pool_use.file_live > fat_pool_use.file_peak) {
			fat_pool_use.file_peak = fat_pool_use.file_live;
		}
	}
	else {
		fat_pool_use.file_denied++;
	}
	return p;
}

void fat_file_free(struct fat_file_info *fi) {
	if (fat_pool_use.file_live) {
		fat_pool_use.file_live--;
	}
	pool_free(&fat_file_pool, fi);
}

struct dirinfo *fat_dirinfo_alloc(void) {
	struct dirinfo *p = pool_alloc(&fat_dirinfo_pool);

	if (p) {
		fat_pool_use.dirinfo_live++;
		if (fat_pool_use.dirinfo_live > fat_pool_use.dirinfo_peak) {
			fat_pool_use.dirinfo_peak = fat_pool_use.dirinfo_live;
		}
	}
	else {
		fat_pool_use.dirinfo_denied++;
	}
	return p;
}

void fat_dirinfo_free(struct dirinfo *di) {
	if (fat_pool_use.dirinfo_live) {
		fat_pool_use.dirinfo_live--;
	}
	pool_free(&fat_dirinfo_pool, di);
}
