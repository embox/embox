/**
 * @file
 * @brief
 *
 * @date 10.02.2024
 * @author Anton Bondarev
 */

#include <string.h>

#include <mem/sysmalloc.h>

#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/fs_driver.h>

#include <fs/iso9660.h>

int cdfs_destroy_inode(struct inode *inode) {
	return 0;
}

struct super_block_operations cdfs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = cdfs_destroy_inode,
};

extern struct inode_operations cdfs_iops;
extern struct file_operations cdfsfs_fop;
/* FIXME bdev_by_path is declared in dvfs.h and fs/mount.h */
extern struct block_dev *bdev_by_path(const char *source);


extern int cdfs_mount(struct inode *root_node);

int cdfs_fill_sb(struct super_block *sb, const char *source) {
	struct inode *dest;
	struct block_dev *bdev;
	struct cdfs_fs_info *fsi;
	struct cdfs_file_info *fi;
	int rc;

	bdev = bdev_by_path(source);
	if (NULL == bdev) {
		return -ENODEV;
	}

	sb->bdev = bdev;

	/* allocate this fs info */
	fsi = iso9660_fsi_alloc();
	if (NULL == fsi) {
		return -ENOMEM;
	}
	memset(fsi, 0, sizeof(struct cdfs_fs_info));

	sb->sb_data = fsi;
	sb->sb_ops = &cdfs_sbops;
	sb->sb_iops = &cdfs_iops;
	sb->sb_fops = &cdfsfs_fop;

	dest = sb->sb_root;

	/* allocate this directory info */
	fi = iso9660_fi_alloc();
	if(NULL == fi) {
		rc = -ENOMEM;
		goto error;
	}
	memset(fi, 0, sizeof(struct cdfs_file_info));

	inode_priv_set(dest, fi);
#if 1
	if(0 == (rc = cdfs_mount(dest))) {
		return 0;
	}
#endif

	return 0;
error:
	iso9660_fsi_free(sb->sb_data);

	return rc;
}

int cdfs_clean_sb(struct super_block *sb) {
	struct cdfs_fs_info *fsi = sb->sb_data;

	/* Deallocate file system */
	if (fsi->path_table_buffer) {
		sysfree(fsi->path_table_buffer);
	}
	if (fsi->path_table) {
		sysfree(fsi->path_table);
	}

	iso9660_fsi_free(fsi);

	iso9660_fi_free(inode_priv(sb->sb_root));

	return 0;
}

static struct fs_driver cdfsfs_driver = {
	.name = "iso9660",
	.fill_sb = cdfs_fill_sb,
	.clean_sb = cdfs_clean_sb,
};

DECLARE_FILE_SYSTEM_DRIVER(cdfsfs_driver);
