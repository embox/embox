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

#include <util/array.h>
#include <embox/unit.h>
#include <embox/block_dev.h>
#include <mem/objalloc.h>
#include <mem/phymem.h>
#include <mem/sysmalloc.h>

#include <framework/mod/options.h>
#include <module/embox/driver/block.h>

#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/ext2.h>
#include <fs/hlpr_path.h>
#include <fs/file_system.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>

#include "ext3_journal.h"

#define EXT2_NAME "ext2"
#define EXT3_NAME "ext3"
#define EXT3_JOURNAL_SUPERBLOCK_INODE 8
#define EXT3_JOURNAL_CNT 16 // XXX to Mybuild

OBJALLOC_DEF(ext3_journal_cache, ext3_journal_specific_t, EXT3_JOURNAL_CNT);

/* file operations */
static int ext3fs_open(struct node *node, struct file_desc *file_desc,
		int flags);
static int ext3fs_close(struct file_desc *desc);
static size_t ext3fs_read(struct file_desc *desc, void *buf, size_t size);
static size_t ext3fs_write(struct file_desc *desc, void *buf, size_t size);
static int ext3fs_ioctl(struct file_desc *desc, int request, ...);

/* fs operations */
static int ext3fs_init(void * par);
static int ext3fs_format(void *path);
static int ext3fs_mount(void *dev, void *dir);
static int ext3fs_create(struct node *parent_node, struct node *node);
static int ext3fs_delete(struct node *node);
static int ext3fs_truncate(struct node *node, off_t length);
static int ext3fs_umount(void *dir);

static struct fs_driver ext3fs_driver;

/*
 * file_operation
 */
static int ext3fs_open(struct node *node, struct file_desc *desc, int flags) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(EXT2_NAME))) {
		return -1;
	}

	return drv->file_op->open(node, desc, flags);
}

static int ext3fs_close(struct file_desc *desc) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(EXT2_NAME))) {
		return -1;
	}

	return drv->file_op->close(desc);
}

static size_t ext3fs_read(struct file_desc *desc, void *buff, size_t size) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(EXT2_NAME))) {
		return -1;
	}

	return drv->file_op->read(desc, buff, size);
}

static size_t ext3fs_write(struct file_desc *desc, void *buff, size_t size) {
	struct fs_driver *drv;
	int res;
	size_t datablocks;
	struct ext2_fs_info *fsi;
	journal_handle_t *handle;

	if (NULL == (drv = fs_driver_find_drv(EXT2_NAME))) {
		return -1;
	}

	assert(desc->node);
	fsi = desc->node->nas->fs->fsi;
	/* N * SECTOR_SIZE + K bytes of data can dirty N + 2 only if K >= 2  */
	datablocks = (size + SECTOR_SIZE - 2) / SECTOR_SIZE + 1;
	/* TODO recalculate */
	if (!(handle = journal_start(fsi->journal, 4 * ext3_trans_blocks(datablocks)))) {
		return -1;
	}
	res = drv->file_op->write(desc, buff, size);
	journal_stop(handle);

	return res;
}

static int ext3fs_ioctl(struct file_desc *desc, int request, ...) {
	return 0;
}

static int ext3fs_init(void *par) {
	return 0;
};

static int ext3fs_create(struct node *parent_node, struct node *node) {
	struct fs_driver *drv;
	struct ext2_fs_info *fsi;
	journal_handle_t *handle;
	int res = -1;

	if(NULL == (drv = fs_driver_find_drv(EXT2_NAME))) {
		return -1;
	}
	fsi = parent_node->nas->fs->fsi;
	/**
	 * ext3_trans_blocks(1) - to modify parent_node's data block
	 * 2 blocks for child = 1 inode + 1 inode bitmap.
	 * 2 * (ext3_trans_blocks(1) + 2) blocks to create "." and ".."
	 */
	if (!(handle = journal_start(fsi->journal, 3 * (ext3_trans_blocks(1) + 2)))) {
		return -1;
	}
	res = drv->fsop->create_node(parent_node, node);
	journal_stop(handle);

	return res;
}

static int ext3fs_delete(struct node *node) {
	struct fs_driver *drv;
	struct ext2_fs_info *fsi;
	journal_handle_t *handle;
	int res;

	if(NULL == (drv = fs_driver_find_drv(EXT2_NAME))) {
		return -1;
	}

	fsi = node->nas->fs->fsi;
	/**
	 * Same as in ext3fs_create:
	 * ext3_trans_blocks(1) - to modify parent_node's data block
	 * 2 blocks for child = 1 inode + 1 inode bitmap
	 */
	if (!(handle = journal_start(fsi->journal, ext3_trans_blocks(1) + 2))) {
		return -1;
	}
	res = drv->fsop->delete_node(node);
	journal_stop(handle);

	return res;
}

extern int main_mke2fs(int argc, char **argv);

static int ext3fs_format(void *dev) {
	struct node *dev_node;
	int argc = 6;
	char *argv[6];
	char dev_path[64];

	dev_node = dev;

	strcpy(dev_path, "/dev/");
	strcat(dev_path, dev_node->name);

	argv[0] = "mke2fs";
	argv[1] = "-b";
	argv[2] = "1024";
	argv[3] = "-t";
	argv[4] = "ext3";
	argv[5] = dev_path;

	getopt_init();
	return main_mke2fs(argc, argv);
}

static int ext3_journal_load(journal_t *jp, block_dev_t *jdev, block_t start) {
    ext3_journal_superblock_t *sb;
    ext3_journal_specific_t *spec = (ext3_journal_specific_t *)jp->j_fs_specific.data;
    char buf[4096];

    assert(jp);
    assert(jdev);

    jp->j_dev = jdev;
    jp->j_disk_sectorsize = block_dev_ioctl(jdev, IOCTL_GETBLKSIZE, NULL, 0);

    assert(jp->j_disk_sectorsize >= 512);

    /* Load superblock from the log. */
    if (!jp->j_dev->driver->read(jp->j_dev, buf,
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

static int ext3fs_mount(void *dev, void *dir) {
	struct fs_driver *drv;
	struct ext2fs_dinode *dip = sysmalloc(sizeof(struct ext2fs_dinode));
	char buf[SECTOR_SIZE * 2];
	struct ext2_fs_info *fsi;
	int inode_sector, ret, rsize;
	struct node *dev_node = dev;
	struct nas *dir_nas = ((struct node *)dir)->nas;
	journal_t *jp = NULL;
	ext3_journal_specific_t *ext3_spec;
	journal_fs_specific_t spec = {
			.bmap = ext3_journal_bmap,
			.commit = ext3_journal_commit,
			.update = ext3_journal_update,
			.trans_freespace = ext3_journal_trans_freespace
	};

	if (NULL == (drv = fs_driver_find_drv(EXT2_NAME))) {
		return -1;
	}

	if ((ret = drv->fsop->mount(dev, dir)) < 0) {
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
	dir_nas = ((struct node *)dir)->nas;
	fsi = dir_nas->fs->fsi;

	inode_sector = ino_to_fsba(fsi, EXT3_JOURNAL_SUPERBLOCK_INODE);

	rsize = ext2_read_sector(dir_nas, buf, 1, inode_sector);
	if (rsize * fsi->s_block_size != fsi->s_block_size) {
		return -EIO;
	}

	/* set pointer to inode struct in read buffer */
	memcpy(dip, (buf
			+ EXT2_DINODE_SIZE(fsi) * ino_to_fsbo(fsi, EXT3_JOURNAL_SUPERBLOCK_INODE)),
			sizeof(struct ext2fs_dinode));

	/* XXX Hack to use ext2 functions */
	dir_nas->fs->drv = &ext3fs_driver;
	ext3_spec->ext3_journal_inode = dip;
	if (0 > ext3_journal_load(jp, (block_dev_t *) dev_node->nas->fi->privdata,
			fsbtodb(fsi, dip->i_block[0]))) {
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

static int ext3fs_truncate (struct node *node, off_t length) {
	struct nas *nas = node->nas;

	nas->fi->ni.size = length;

	return 0;
}

static int ext3fs_umount(void *dir) {
	struct fs_driver *drv;
	struct ext2_fs_info *fsi;
	ext3_journal_specific_t *data;
	int res;

	fsi = ((struct node *)dir)->nas->fs->fsi;
	data = fsi->journal->j_fs_specific.data;

	if(NULL == (drv = fs_driver_find_drv(EXT2_NAME))) {
		return -1;
	}

	res = drv->fsop->umount(dir);

	journal_delete(fsi->journal);
	sysfree(data->ext3_journal_inode);
	journal_free_block(fsi->journal, data->j_sb_buffer);
	objfree(&ext3_journal_cache, data);

	return res;
}

static struct kfile_operations ext3_fop = {
	.open = ext3fs_open,
	.close = ext3fs_close,
	.read = ext3fs_read,
	.write = ext3fs_write,
	.ioctl = ext3fs_ioctl,
};

static struct fsop_desc ext3_fsop = {
	.init	     = ext3fs_init,
	.format	     = ext3fs_format,
	.mount	     = ext3fs_mount,
	.create_node = ext3fs_create,
	.delete_node = ext3fs_delete,

	.getxattr    = ext2fs_getxattr,
	.setxattr    = ext2fs_setxattr, /* TODO journaling */
	.listxattr   = ext2fs_listxattr,

	.truncate    = ext3fs_truncate, /* TODO journaling */
	.umount      = ext3fs_umount,
};

static struct fs_driver ext3fs_driver = {
	.name = EXT3_NAME,
	.file_op = &ext3_fop,
	.fsop = &ext3_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(ext3fs_driver);
