/**
 * @file
 * @brief ext3 file system
 *
 * @date 12.03.2013
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <util/getopt.h>

#include <mem/sysmalloc.h>

#include <fs/fs_driver.h>
#include <fs/inode.h>
#include <fs/mount.h>
#include <fs/super_block.h>
 
#include <fs/ext2.h>


#include "ext3_journal.h"

#define EXT2_NAME "ext2"
#define EXT3_NAME "ext3"
#define EXT3_JOURNAL_SUPERBLOCK_INODE 8


/* fs operations */
static int ext3fs_format(struct block_dev *bdev, void *priv);
static int ext3fs_fill_sb(struct super_block *sb, const char *source);


static const struct fs_driver *ext2fs_driver;
static struct fs_driver ext3fs_driver;

int ext3fs_destroy_inode(struct inode *inode) {
	return 0;
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

extern struct file_operations ext3_fop;
extern struct inode_operations ext3_iops;

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


	if (NULL == (ext3_spec = ext3_journal_cache_alloc())) {
		return -1;
	}

	spec.data = ext3_spec;

	if (NULL == (jp = journal_create(&spec))) {
		ext3_journal_cache_free(ext3_spec);
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

extern struct super_block_operations ext3fs_sbops;

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
	ext3_journal_cache_free(data);

	return res;
}

static struct fs_driver ext3fs_driver = {
	.name = EXT3_NAME,
	.format   = ext3fs_format,
	.fill_sb = ext3fs_fill_sb,
	.clean_sb = ext3fs_clean_sb,
};

DECLARE_FILE_SYSTEM_DRIVER(ext3fs_driver);
