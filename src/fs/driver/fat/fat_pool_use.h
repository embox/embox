/****************************************************************************
 * fat_pool_use.h -- occupancy of the FAT driver's three fixed pools.
 *
 * Its own header rather than a few lines in fat.h, because fat.h cannot be
 * included from outside the fat module: it resolves FAT_MAX_SECTOR_SIZE
 * through an OPTION macro that only exists in that module's build context.
 * Anything that wants to know why a create failed would then have to declare
 * a copy of this structure and hope it stays in step.
 ****************************************************************************/

#ifndef FS_FAT_POOL_USE_H_
#define FS_FAT_POOL_USE_H_

struct fat_pool_use {
	unsigned fs_live, fs_max, fs_denied;
	unsigned file_live, file_peak, file_max, file_denied;
	unsigned dirinfo_live, dirinfo_peak, dirinfo_max, dirinfo_denied;
};

extern struct fat_pool_use fat_pool_use;

/* An operation on a file whose inode has no private data left.
 *
 * fat_read() and fat_write() begin by reading fi->pointer, at offset 44, and
 * a NULL fi makes that a data abort at address 0x2c -- which is exactly what
 * board boots 60, 63 and 66 died of, in the middle of the unlink loop. The
 * driver lock's guard turns that crash into this
 * record, so the run finishes and names the file instead of stopping with
 * the answer in a register nobody can read. */
struct fat_null_priv {
	unsigned reads;   /* reads refused because the inode had no fi */
	unsigned writes;  /* writes refused for the same reason */
	char last[32];    /* the name of the most recent one */
};

extern struct fat_null_priv fat_null_priv;

#endif /* FS_FAT_POOL_USE_H_ */
