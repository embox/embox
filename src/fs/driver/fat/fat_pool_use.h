/****************************************************************************
 * fat_pool_use.h -- occupancy of the FAT driver's three fixed pools.
 *
 * Separate from fat.h, which can only be included inside the fat module.
 ****************************************************************************/

#ifndef FS_FAT_POOL_USE_H_
#define FS_FAT_POOL_USE_H_

struct fat_pool_use {
	unsigned fs_live, fs_max, fs_denied;
	unsigned file_live, file_peak, file_max, file_denied;
	unsigned dirinfo_live, dirinfo_peak, dirinfo_max, dirinfo_denied;
};

extern struct fat_pool_use fat_pool_use;

/* Reads and writes refused because the inode had no private data. */
struct fat_null_priv {
	unsigned reads;   /* reads refused because the inode had no fi */
	unsigned writes;  /* writes refused for the same reason */
	char last[32];    /* the name of the most recent one */
};

extern struct fat_null_priv fat_null_priv;

#endif /* FS_FAT_POOL_USE_H_ */
