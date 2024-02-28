/**
 * @file
 * @brief ext file system
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <stdint.h>
#include <sys/types.h>

#include <fs/fs_driver.h>
#include <fs/inode.h>
#include <fs/mount.h>
#include <fs/super_block.h>

#include <fs/ext2.h>


#define FS_NAME "ext2"

/* TODO link counter */

extern struct file_operations ext2_fop;
extern struct inode_operations ext2_iops;

static int e2fs_destroy_inode(struct inode *inode) {
	return 0;
}

static struct super_block_operations e2fs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = e2fs_destroy_inode,
};

extern int ext2_read_sblock(struct super_block *sb);
extern int ext2_read_gdblock(struct super_block *sb);
extern int ext2_read_inode(struct inode *node, uint32_t);

extern struct ext2_fs_info *ext2fs_fsi_alloc(void);
extern void ext2fs_fsi_free(struct ext2_fs_info *fsi);
extern struct ext2_file_info *ext2_fi_alloc(void);
extern void ext2_fi_free(struct ext2_file_info *fi);

extern void ext2_dflt_sb(struct ext2sb *sb, size_t dev_size, float dev_factor);

extern int ext2_mark_bitmap(void *bdev, struct ext2sb *sb,
							struct ext2_gd *gd, char *buff, float dev_factor) ;
extern void ext2_dflt_gd(struct ext2sb *sb, struct ext2_gd *gd);


extern void ext2_dflt_root_inode(struct ext2fs_dinode *di);

extern void ext2_dflt_root_entry(char *point);


static int ext2fs_format(struct block_dev *bdev, void *priv) {
	struct ext2sb sb;
	struct ext2_gd gd;
	struct ext2fs_dinode *di;
	char buff[SBSIZE];
	uint64_t dev_size;
	size_t dev_bsize;
	int sector;
	float dev_factor;

	memset(&sb, 0, sizeof(struct ext2sb));
	memset(&gd, 0, sizeof(struct ext2_gd));

	dev_size = block_dev_size(bdev);
	dev_bsize = block_dev_block_size(bdev);
	dev_factor = SBSIZE / dev_bsize;

	ext2_dflt_sb(&sb, dev_size, dev_factor);
	ext2_dflt_gd(&sb, &gd);

	sector = 1 * dev_factor;
	if (0 > block_dev_write(bdev, (char *) &sb, SBSIZE, sector)) {
		return -1;
	}

	memset(buff, 0, SBSIZE);
	memcpy(buff, &gd, sizeof(struct ext2_gd));
	sector = START_BLOCK * dev_factor;
	if (0 > block_dev_write(bdev, buff, SBSIZE, sector)) {
		return -1;
	}

	ext2_mark_bitmap(bdev, &sb, &gd, buff, dev_factor);

	/* set root inode */
	memset(buff, 0, SBSIZE);
	di = (struct ext2fs_dinode *) (buff + sb.s_inode_size);
	ext2_dflt_root_inode(di);
	di->i_block[0] = sb.s_blocks_count - sb.s_free_blocks_count - 2;
	sector = gd.inode_table * dev_factor;
	if (0 > block_dev_write(bdev, buff, SBSIZE, sector)) {
		return -1;
	}

	/* write root entry */
	sector = di->i_block[0] * dev_factor;
	memset(buff, 0, SBSIZE);
	ext2_dflt_root_entry(buff);

	if (0 > block_dev_write(bdev, buff, SBSIZE, sector)) {
		return -1;
	}

	return 0;
}

static int ext2_fill_sb(struct super_block *sb, const char *source) {
	struct ext2_file_info *fi;
	struct inode *dest;
	struct block_dev *bdev;
	struct ext2_fs_info *fsi = NULL;
	int rc = 0;

	bdev = bdev_by_path(source);
	if (NULL == bdev) {
		return -ENODEV;
	}
	sb->bdev = bdev;

	fsi = ext2fs_fsi_alloc();
	if (NULL == fsi) {
		return -ENOMEM;
	}

	memset(fsi, 0, sizeof(struct ext2_fs_info));
	sb->sb_data = fsi;
	sb->sb_ops = &e2fs_sbops;
	sb->sb_iops = &ext2_iops;
	sb->sb_data = fsi;
	sb->sb_fops = &ext2_fop;

	/* presetting that we think */
	fsi->s_block_size = SBSIZE;
	fsi->s_sectors_in_block = fsi->s_block_size / 512;

	rc = ext2_read_sblock(sb);
	if (0 != rc) {
		goto error1;
	}

	fsi->e2fs_gd = ext2_buff_alloc(fsi, sizeof(struct ext2_gd) * fsi->s_ncg);
	if (NULL == fsi->e2fs_gd) {
		rc = ENOMEM;
		goto error1;
	}

	rc = ext2_read_gdblock(sb);
	if (0 != rc) {
		goto error2;
	}

	dest = sb->sb_root;
	fi= ext2_fi_alloc();
	if (!fi) {
		rc = ENOMEM;
		goto error2;
	}

	memset(fi, 0, sizeof(struct ext2_file_info));

	inode_size_set(dest, 0);
	inode_priv_set(dest, fi);

	fi->f_buf = ext2_buff_alloc(fsi, fsi->s_block_size);
	if (NULL == fi) {
		rc = ENOMEM;
		goto error3;
	}

	rc = ext2_read_inode(dest, EXT2_ROOTINO);
	if (0 != rc) {
		goto error4;
	}

	ext2_buff_free(sb->sb_data, fi->f_buf);

	dest->i_mode = fi->f_di.i_mode;
	dest->i_owner_id = fi->f_di.i_uid;
	dest->i_group_id = fi->f_di.i_gid;

	return 0;

error4:
	ext2_buff_free(sb->sb_data, fi->f_buf);

error3:
	ext2_fi_free(fi);

error2:
	ext2_buff_free(fsi, (void *) fsi->e2fs_gd);

error1:
	ext2fs_fsi_free(fsi);	

	return -rc;
}

static int ext2_clean_sb(struct super_block *sb) {
	struct ext2_fs_info *fsi = sb->sb_data;

	if (NULL != fsi) {
		if (NULL != fsi->e2fs_gd) {
			ext2_buff_free(fsi, (char *) fsi->e2fs_gd);
		}
		
		ext2fs_fsi_free(fsi);
	}

	ext2_fi_free(inode_priv(sb->sb_root));

	return 0;
}

static struct fs_driver ext2fs_driver = {
	.name     = FS_NAME,
	.format   = ext2fs_format,
	.fill_sb  = ext2_fill_sb,
	.clean_sb = ext2_clean_sb,
};

DECLARE_FILE_SYSTEM_DRIVER(ext2fs_driver);