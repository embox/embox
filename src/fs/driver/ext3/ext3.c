/**
 * @file
 * @brief ext3 file system
 *
 * @date 12.03.2013
 * @author Andrey Gazukin
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <lib/libds/array.h>
#include <util/err.h>
#include <util/getopt.h>
#include <embox/unit.h>
#include <drivers/block_dev.h>
#include <mem/objalloc.h>
#include <mem/phymem.h>
#include <mem/sysmalloc.h>

#include <framework/mod/options.h>

#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/ext2.h>
#include <fs/hlpr_path.h>
#include <fs/super_block.h>
#include <fs/file_desc.h>

#include "ext3_journal.h"

#define EXT2_NAME "ext2"
#define EXT3_NAME "ext3"
#define EXT3_JOURNAL_SUPERBLOCK_INODE 8
#define EXT3_JOURNAL_CNT 16 // XXX to Mybuild

OBJALLOC_DEF(ext3_journal_cache, ext3_journal_specific_t, EXT3_JOURNAL_CNT);

/* file operations */
static struct idesc *ext3fs_open(struct inode *node, struct idesc *idesc, int __oflag);
static int ext3fs_close(struct file_desc *desc);
static size_t ext3fs_read(struct file_desc *desc, void *buf, size_t size);
static size_t ext3fs_write(struct file_desc *desc, void *buf, size_t size);

/* fs operations */
static int ext3fs_format(struct block_dev *bdev, void *priv);
static int ext3fs_fill_sb(struct super_block *sb, const char *source);
//static int ext3fs_mount(struct super_block *sb, struct inode *dest);
static int ext3fs_create(struct inode *node, struct inode *parent_node, int mode);
//static int ext3fs_delete(struct inode *dir, struct inode *node);
static int ext3fs_truncate(struct inode *node, off_t length);
//static int ext3fs_umount_entry(struct inode *node);

static const struct fs_driver *ext2fs_driver;
static struct fs_driver ext3fs_driver;

static int ext3fs_destroy_inode(struct inode *inode) {
	return 0;
}

static struct super_block_operations ext3fs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = &ext3fs_destroy_inode,
};

extern struct file_operations ext2_fop;
/*
 * file_operation
 */
static struct idesc *ext3fs_open(struct inode *node, struct idesc *idesc, int __oflag) {
	return ext2_fop.open(node, idesc, __oflag);
}

static int ext3fs_close(struct file_desc *desc) {
	return ext2_fop.close(desc);
}

static size_t ext3fs_read(struct file_desc *desc, void *buff, size_t size) {
	return ext2_fop.read(desc, buff, size);
}

static size_t ext3fs_write(struct file_desc *desc, void *buff, size_t size) {
	int res;
	size_t datablocks;
	struct ext2_fs_info *fsi;
	journal_handle_t *handle;

	assert(desc->f_inode);
	fsi = desc->f_inode->i_sb->sb_data;
	/* N * SECTOR_SIZE + K bytes of data can dirty N + 2 only if K >= 2  */
	datablocks = (size + SECTOR_SIZE - 2) / SECTOR_SIZE + 1;
	/* TODO recalculate */
	if (!(handle = journal_start(fsi->journal, 4 * ext3_trans_blocks(datablocks)))) {
		return -1;
	}
	res = ext2_fop.write(desc, buff, size);
	journal_stop(handle);

	return res;
}

extern struct inode_operations ext2_iops;

static int ext3fs_create(struct inode *node, struct inode *parent_node, int mode) {
	struct ext2_fs_info *fsi;
	journal_handle_t *handle;
	int res = -1;

	fsi = parent_node->i_sb->sb_data;
	/**
	 * ext3_trans_blocks(1) - to modify parent_node's data block
	 * 2 blocks for child = 1 inode + 1 inode bitmap.
	 * 2 * (ext3_trans_blocks(1) + 2) blocks to create "." and ".."
	 */
	if (!(handle = journal_start(fsi->journal, 3 * (ext3_trans_blocks(1) + 2)))) {
		return -1;
	}
	res = ext2_iops.ino_create(node, parent_node, node->i_mode);
	journal_stop(handle);

	return res;
}

static int ext3fs_delete(struct inode *dir, struct inode *node) {
	struct ext2_fs_info *fsi;
	journal_handle_t *handle;
	int res;

	fsi = node->i_sb->sb_data;
	/**
	 * Same as in ext3fs_create:
	 * ext3_trans_blocks(1) - to modify parent_node's data block
	 * 2 blocks for child = 1 inode + 1 inode bitmap
	 */
	if (!(handle = journal_start(fsi->journal, ext3_trans_blocks(1) + 2))) {
		return -1;
	}
	res = ext2_iops.ino_remove(dir, node);
	journal_stop(handle);

	return res;
}

extern int main_mke2fs(int argc, char **argv);

static int ext3fs_format(struct block_dev *bdev, void *priv) {
	int argc = 6;
	char *argv[6];
	char dev_path[64];

	strcpy(dev_path, "/dev/");
	strcat(dev_path, block_dev_name(bdev));

	argv[0] = "mke2fs";
	argv[1] = "-b";
	argv[2] = "1024";
	argv[3] = "-t";
	argv[4] = "ext3";
	argv[5] = dev_path;

	getopt_init();
	return main_mke2fs(argc, argv);
}

static int ext3_journal_load(journal_t *jp, struct block_dev *jdev, block_t start) {
    ext3_journal_superblock_t *sb;
    ext3_journal_specific_t *spec = (ext3_journal_specific_t *)jp->j_fs_specific.data;
    char buf[4096];

    assert(jp);
    assert(jdev);

    jp->j_dev = jdev;
    jp->j_disk_sectorsize = block_dev_block_size(jdev);

    assert(jp->j_disk_sectorsize >= 512);

    /* Load superblock from the log. */
    if (!jp->j_dev->driver->bdo_read(jp->j_dev, buf,
    		4096, start)) {
    	return -1;
    }

    sb = (ext3_journal_superblock_t *)buf;

    assert(sb->s_blocksize);

    jp->j_maxlen         = ntohl(sb->s_maxlen);
    jp->j_blocksize      = ntohl(sb->s_blocksize);
    jp->j_blk_offset     = journal_db2jb(jp, start);
    jp->j_first          = 1;
    jp->j_last           = jp->j_maxlen;
    spec->j_format_version = ntohl(sb->s_header.h_blocktype);

    /* Initialize transaction specific data */
    jp->j_head                 = jp->j_first;
    jp->j_tail                 = jp->j_head;
    jp->j_free                 = jp->j_last - jp->j_first;
    jp->j_tail_sequence        = 1;
    jp->j_transaction_sequence = 1;
    jp->j_running_transaction  = journal_new_trans(jp);
    dlist_init(&jp->j_checkpoint_transactions);

    /* Update journal superblock */
    spec->j_sb_buffer  = journal_new_block(jp, jp->j_blk_offset);
    spec->j_superblock = (ext3_journal_superblock_t *)spec->j_sb_buffer->data;
    memcpy(spec->j_sb_buffer->data, buf, jp->j_blocksize);
    jp->j_fs_specific.update(jp);

    return 0;
}

static struct file_operations ext3_fop;
struct inode_operations ext3_iops;

static int ext3fs_create_jornal(struct super_block *sb, struct inode *dest) {
	struct ext2fs_dinode *dip;
	char buf[SECTOR_SIZE * 2];
	struct ext2_fs_info *fsi;
	int inode_sector, rsize;
	journal_t *jp = NULL;
	ext3_journal_specific_t *ext3_spec;
	journal_fs_specific_t spec = {
			.bmap = ext3_journal_bmap,
			.commit = ext3_journal_commit,
			.update = ext3_journal_update,
			.trans_freespace = ext3_journal_trans_freespace
	};


	if (NULL == (ext3_spec = objalloc(&ext3_journal_cache))) {
		return -1;
	}

	spec.data = ext3_spec;

	if (NULL == (jp = journal_create(&spec))) {
		objfree(&ext3_journal_cache, ext3_spec);
		return -1;
	}

	/* Getting first block for inode number EXT3_JOURNAL_SUPERBLOCK_INODE */
	fsi = sb->sb_data;

	inode_sector = ino_to_fsba(fsi, EXT3_JOURNAL_SUPERBLOCK_INODE);

	rsize = ext2_read_sector(sb, buf, 1, inode_sector);
	if (rsize * fsi->s_block_size != fsi->s_block_size) {
		return -EIO;
	}

	/* set pointer to inode struct in read buffer */
	dip = sysmalloc(sizeof(struct ext2fs_dinode));
	memcpy(dip,
		(buf + EXT2_DINODE_SIZE(fsi) * ino_to_fsbo(fsi, EXT3_JOURNAL_SUPERBLOCK_INODE)),
		sizeof(struct ext2fs_dinode));

	/* XXX Hack to use ext2 functions */
	ext3_spec->ext3_journal_inode = dip;
	if (0 > ext3_journal_load(jp, sb->bdev, fsbtodb(fsi, dip->i_block[0]))) {
		return -EIO;
	}
	/*
	 * FIXME Now journal supports block size only equal to filesystem block size
	 * It is not critical but not flexible enough
	 */
	assert(jp->j_blocksize == fsi->s_block_size);
	fsi->journal = jp;

	return 0;
}

static int ext3fs_fill_sb(struct super_block *sb, const char *source) {
	int ret = 0;

	ext2fs_driver = fs_driver_find(EXT2_NAME);
	if (NULL == ext2fs_driver) {
		return -1;
	}

	if ((ret = ext2fs_driver->fill_sb(sb, source)) < 0) {
		return ret;
	}

	sb->sb_ops = &ext3fs_sbops;
	sb->sb_fops = &ext3_fop;
	sb->sb_iops = &ext3_iops;


	ret = ext3fs_create_jornal(sb, sb->sb_root);

	return ret;
}
#if 0
static int ext3fs_mount(struct super_block *sb, struct inode *dest) {
#if 0
	struct ext2fs_dinode *dip;
	char buf[SECTOR_SIZE * 2];
	struct ext2_fs_info *fsi;
	int inode_sector, ret, rsize;
	journal_t *jp = NULL;
	ext3_journal_specific_t *ext3_spec;
	journal_fs_specific_t spec = {
			.bmap = ext3_journal_bmap,
			.commit = ext3_journal_commit,
			.update = ext3_journal_update,
			.trans_freespace = ext3_journal_trans_freespace
	};

	if ((ret = ext2fs_driver->fsop->mount(sb, dest)) < 0) {
		return ret;
	}

	if (NULL == (ext3_spec = objalloc(&ext3_journal_cache))) {
		return -1;
	}

	spec.data = ext3_spec;

	if (NULL == (jp = journal_create(&spec))) {
		objfree(&ext3_journal_cache, ext3_spec);
		return -1;
	}

	/* Getting first block for inode number EXT3_JOURNAL_SUPERBLOCK_INODE */
	fsi = sb->sb_data;

	inode_sector = ino_to_fsba(fsi, EXT3_JOURNAL_SUPERBLOCK_INODE);

	rsize = ext2_read_sector(sb, buf, 1, inode_sector);
	if (rsize * fsi->s_block_size != fsi->s_block_size) {
		return -EIO;
	}

	/* set pointer to inode struct in read buffer */
	dip = sysmalloc(sizeof(struct ext2fs_dinode));
	memcpy(dip,
		(buf + EXT2_DINODE_SIZE(fsi) * ino_to_fsbo(fsi, EXT3_JOURNAL_SUPERBLOCK_INODE)),
		sizeof(struct ext2fs_dinode));

	/* XXX Hack to use ext2 functions */
	ext3_spec->ext3_journal_inode = dip;
	if (0 > ext3_journal_load(jp, sb->bdev, fsbtodb(fsi, dip->i_block[0]))) {
		return -EIO;
	}
	/*
	 * FIXME Now journal supports block size only equal to filesystem block size
	 * It is not critical but not flexible enough
	 */
	assert(jp->j_blocksize == fsi->s_block_size);
	fsi->journal = jp;
#endif
	return 0;
}
#endif

/* TODO ext2fs_truncate()? */
static int ext3fs_truncate (struct inode *node, off_t length) {

	inode_size_set(node, length);

	return 0;
}

static int ext3fs_clean_sb(struct super_block *sb) {
	int res;
	struct ext2_fs_info *fsi;
	ext3_journal_specific_t *data;

	fsi = sb->sb_data;
	data = fsi->journal->j_fs_specific.data;

	res = ext2fs_driver->clean_sb(sb);

	journal_delete(fsi->journal);
	sysfree(data->ext3_journal_inode);
	journal_free_block(fsi->journal, data->j_sb_buffer);
	objfree(&ext3_journal_cache, data);

	return res;
}
#if 0
static int ext3fs_umount_entry(struct inode *node) {
	return 0;
	//return ext2fs_driver->fsop->umount_entry(node);
}
#endif

static struct file_operations ext3_fop = {
	.open = ext3fs_open,
	.close = ext3fs_close,
	.read = ext3fs_read,
	.write = ext3fs_write,
};

extern int ext2_iterate(struct inode *next, char *next_name, struct inode *parent, struct dir_ctx *dir_ctx);
extern struct inode *ext2fs_lookup(struct inode *node, char const *name, struct inode const *dir);
struct inode_operations ext3_iops = {
	.ino_create  = ext3fs_create,
	.ino_remove  = ext3fs_delete,
	.ino_iterate = ext2_iterate,
	.ino_lookup = ext2fs_lookup,
	.ino_truncate     = ext3fs_truncate, /* TODO journaling */

	.ino_getxattr     = ext2fs_getxattr,
	.ino_setxattr     = ext2fs_setxattr, /* TODO journaling */
	.ino_listxattr    = ext2fs_listxattr,
};
#if 0
static struct fsop_desc ext3_fsop = {
	//.mount	      = ext3fs_mount,

	//.umount_entry = ext3fs_umount_entry,
};
#endif
static struct fs_driver ext3fs_driver = {
	.name = EXT3_NAME,
	.format   = ext3fs_format,
	.fill_sb = ext3fs_fill_sb,
	.clean_sb = ext3fs_clean_sb,

	//.fsop = &ext3_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(ext3fs_driver);
