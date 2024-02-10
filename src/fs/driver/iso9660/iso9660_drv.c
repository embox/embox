/**
 * @file
 * @brief
 *
 * @date 10.02.2024
 * @author Anton Bondarev
 */

#include <fs/super_block.h>

#include <fs/fs_driver.h>

#include <fs/iso9660.h>

extern int cdfs_clean_sb(struct super_block *sb);

/* File system operations*/
extern  int cdfs_fill_sb(struct super_block *sb, const char *source);

static struct fs_driver cdfsfs_driver = {
	.name = "iso9660",
	.fill_sb = cdfs_fill_sb,
	.clean_sb = cdfs_clean_sb,
};

DECLARE_FILE_SYSTEM_DRIVER(cdfsfs_driver);
