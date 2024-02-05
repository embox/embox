/**
 * @file
 * @brief ext4 driver
 *
 * @date 10.09.2013
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#include <lib/libds/array.h>
#include <util/err.h>
#include <util/getopt.h>
#include <embox/unit.h>
#include <drivers/block_dev.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>
#include <lib/crypt/crc16.h>


#include <fs/journal.h>
#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/ext2.h>
#include <fs/ext4.h>
#include <fs/hlpr_path.h>
#include <fs/mount.h>
#include <fs/super_block.h>
#include <fs/file_desc.h>
#include <fs/dir_context.h>
#include <fs/inode_operation.h>


#include <mem/sysmalloc.h>


/*
 * Copyright (c) 1997 Manuel Bouyer.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*-
 * Copyright (c) 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * The Mach Operating System project at Carnegie-Mellon University.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *
 * Copyright (c) 1990, 1991 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Author: David Golub
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

static int ext4_read_inode(struct inode *node, uint32_t);
static int ext4_block_map(struct inode *node, int32_t, uint32_t *);
static int ext4_buf_read_file(struct inode *node, char **, size_t *);
static size_t ext4_write_file(struct inode *node, char *buf_p, size_t size);
static int ext4_new_block(struct inode *nnodeas, long position);
static int ext4_search_directory(struct inode *node, const char *, int, uint32_t *);
static int ext4_read_sblock(struct inode *node);
static int ext4_read_gdblock(struct inode *node);
static int ext4_mount_entry(struct inode *node);
static int ext4_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *dir_ctx);

int ext4_write_gdblock(struct inode *node);

static void ext4_rw_inode(struct inode *node, struct ext4_inode *fdi, int rw_flag);

int ext4_write_map(struct inode *i_new, long position, uint32_t new_block, int op);

/* ext filesystem description pool */
POOL_DEF(ext4_fs_pool, struct ext4_fs_info, 128);

/* ext file description pool */
POOL_DEF(ext4_file_pool, struct ext4_file_info, 128);

#define EXT4_NAME "ext4"

/* TODO link counter */

static struct idesc *ext4fs_open(struct inode *node, struct idesc *idesc, int __oflag);
static int ext4fs_close(struct file_desc *desc);
static size_t ext4fs_read(struct file_desc *desc, void *buf, size_t size);
static size_t ext4fs_write(struct file_desc *desc, void *buf, size_t size);
static int ext4fs_create(struct inode *node, struct inode *parent_node, int mode);
static int ext4fs_delete(struct inode *node);
static int ext4fs_truncate(struct inode *node, off_t length);

static int ext4fs_destroy_inode(struct inode *inode) {
	return 0;
}

static struct super_block_operations ext4fs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = ext4fs_destroy_inode,
};

struct inode_operations ext4_iops = {
	.ino_create  = ext4fs_create,
	.ino_remove  = ext4fs_delete,
	.ino_iterate = ext4_iterate,
	.ino_truncate = ext4fs_truncate,

	.ino_getxattr     = ext2fs_getxattr,
	.ino_setxattr     = ext2fs_setxattr,
	.ino_listxattr    = ext2fs_listxattr,
};

static struct file_operations ext4_fop = {
	.open = ext4fs_open,
	.close = ext4fs_close,
	.read = ext4fs_read,
	.write = ext4fs_write,
};

/* Calculates the physical block from a given logical block and extent */
static uint64_t ext4_extent_get_block_from_ees(struct ext4_extent *ee,
		uint32_t n_ee, uint32_t lblock, uint32_t *len) {
	uint32_t block_ext_index = 0;
	uint32_t block_ext_offset = 0;
	uint32_t i;

	/* Skip to the right extent entry */
	for (i = 0; i < n_ee; i++) {
		if (ee[i].ee_block + ee[i].ee_len > lblock) {
			block_ext_index = i;
			block_ext_offset = lblock - ee[i].ee_block;
			if (len) {
				*len = ee[i].ee_block + ee[i].ee_len - lblock;
			}
			return ee[block_ext_index].ee_start_lo + block_ext_offset;
		}
	}

	return 0;
}

/* Fetches a block that stores extent info and returns an array of extents
 * _with_ its header. */
static void *ext4_extent_get_extents_in_block(struct inode *node, uint32_t block) {
	struct ext4_extent_header eh;
	void *exts;
	uint32_t extents_len;
	struct ext4_fs_info *fsi;

	fsi = node->i_sb->sb_data;

	block_dev_read_buffered(node->i_sb->bdev, (char*)&eh,
			sizeof(struct ext4_extent_header), block * fsi->s_block_size);

	extents_len = eh.eh_entries * sizeof(struct ext4_extent)
			+ sizeof(struct ext4_extent_header);

	exts = sysmalloc(extents_len);

	block_dev_read_buffered(node->i_sb->bdev, exts, extents_len,
			block * fsi->s_block_size);

	return exts;
}

/* Returns the physical block number */
static uint64_t ext4_extent_get_pblock(struct inode *node, void *extents, uint32_t lblock,
		uint32_t *len) {
	struct ext4_extent_header *eh = extents;
	struct ext4_extent *ee_array;
	uint64_t ret;

	if (eh->eh_depth == 0) {
		ee_array = extents + sizeof(struct ext4_extent_header);
		ret = ext4_extent_get_block_from_ees(ee_array, eh->eh_entries, lblock,
				len);
	} else {
		struct ext4_extent_idx *ei_array = extents
				+ sizeof(struct ext4_extent_header);
		struct ext4_extent_idx *recurse_ei = NULL;
		void *leaf_extents;

		for (int i = 0; i < eh->eh_entries; i++) {
			assert(ei_array[i].ei_leaf_hi == 0);

			if (ei_array[i].ei_block > lblock) {
				break;
			}

			recurse_ei = &ei_array[i];
		}

		assert(recurse_ei);

		leaf_extents = ext4_extent_get_extents_in_block(node,
				recurse_ei->ei_leaf_lo);
		ret = ext4_extent_get_pblock(node, leaf_extents, lblock, len);
		sysfree(leaf_extents);
	}

	return ret;
}

static void ext4_extent_add_block(struct inode *node, uint32_t lblock, uint64_t pblock) {
	struct ext4_file_info *fi = inode_priv(node);
	void *extents = fi->f_di.i_block;
	struct ext4_extent_header *eh = extents;
	struct ext4_extent *ee_array;
	int current_ee;

	ee_array = extents + sizeof(struct ext4_extent_header);

	eh->eh_depth = 0;
	current_ee = eh->eh_entries++;

	ee_array[current_ee].ee_block = lblock;
	ee_array[current_ee].ee_len = 1;
	ee_array[current_ee].ee_start_lo = pblock;
	ee_array[current_ee].ee_start_hi = 0;
}

static void *ext4_buff_alloc(struct ext4_fs_info *fsi, size_t size) {

	if (size < fsi->s_block_size) {
		size = fsi->s_block_size;
	}

	fsi->s_page_count = size / PAGE_SIZE();
	if (0 != size % PAGE_SIZE()) {
		fsi->s_page_count++;
	}
	if (0 == fsi->s_page_count) {
		fsi->s_page_count++;
	}

	return phymem_alloc(fsi->s_page_count);
}

static int ext4_buff_free(struct ext4_fs_info *fsi, char *buff) {
	if ((0 != fsi->s_page_count) && (NULL != buff)) {
		phymem_free(buff, fsi->s_page_count);
	}
	return 0;
}

static int ext4_read_sector(struct inode *node, char *buffer, uint32_t count,
		uint32_t sector) {
	struct ext4_fs_info *fsi;
	fsi = node->i_sb->sb_data;

	if (0 > block_dev_read(node->i_sb->bdev, (char *) buffer,
					count * fsi->s_block_size, fsbtodb(fsi, sector))) {
		return -1;
	}
	else {
		return count;
	}
}

int ext4_write_sector(struct inode *node, char *buffer, uint32_t count,
		uint32_t sector) {
	struct ext4_fs_info *fsi;

	fsi = node->i_sb->sb_data;

	if (!strcmp(node->i_sb->fs_drv->name, "ext3")) {
		/* EXT3 */
		int i = 0;
		journal_t *jp = fsi->journal;
		journal_block_t *b;

		assert(jp);

		for (i = 0 ; i < count; i++) {
			b = journal_new_block(jp, sector + i);
			if (!b) {
				return -1;
			}
			memcpy(b->data, buffer + i * fsi->s_block_size, fsi->s_block_size);
			journal_dirty_block(jp, b);
		}
	} else {
		/* EXT2 */
		if (0 > block_dev_write(node->i_sb->bdev, (char *) buffer,
						count * fsi->s_block_size, fsbtodb(fsi, sector))) {
			return -1;
		}
	}

	return count;
}

/*
 * Calculate indirect block levels.
 *
 * We note that the number of indirect blocks is always
 * a power of 2.  This lets us use shifts and masks instead
 * of divide and remainder and avoinds pulling in the
 * 64bit division routine into the boot code.
 */
static int ext4_shift_culc(struct ext4_file_info *fi,
								struct ext4_fs_info *fsi) {
	int32_t mult;
	int ln2;

	mult = NINDIR(fsi);
	if (0 == mult) {
		return -1;
	}

	for (ln2 = 0; mult != 1; ln2++) {
		mult >>= 1;
	}

	fi->f_nishift = ln2;

	return 0;
}

/* find and read symlink file */
static int ext4_read_symlink(struct inode *node, uint32_t parent_inumber,
		const char **cp) {
	/* XXX should handle LARGEFILE */
	int len, link_len;
	int rc;
	uint32_t inumber;
	char namebuf[MAXPATHLEN + 1];
	int nlinks;
	uint32_t disk_block;
	struct ext4_file_info *fi;

	fi = inode_priv(node);

	nlinks = 0;
	link_len = ext4_file_size(fi->f_di);
	len = strlen(*cp);

	if ((link_len + len > MAXPATHLEN) || (++nlinks > MAXSYMLINKS)) {
		return ENOENT;
	}

	memmove(&namebuf[link_len], cp, len + 1);

	if (link_len < EXT2_MAXSYMLINKLEN) {
		memcpy(namebuf, fi->f_di.i_block, link_len);
	} else {
		/* Read file for symbolic link */
		if (0 != (rc = ext4_block_map(node, (int32_t) 0, &disk_block))) {
			return rc;
		}
		if (1 != ext4_read_sector(node, fi->f_buf, 1, disk_block)) {
			return EIO;
		}
		memcpy(namebuf, fi->f_buf, link_len);
	}
	/*
	 * If relative pathname, restart at parent directory.
	 * If absolute pathname, restart at root.
	 */
	*cp = namebuf;
	if (*namebuf != '/') {
		inumber = parent_inumber;
	} else {
		inumber = (uint32_t) EXT2_ROOTINO;
	}
	rc = ext4_read_inode(node, inumber);

	return rc;
}

/* set node type by file system file type */
static mode_t ext4_type_to_mode_fmt(uint8_t e2d_type) {
	switch (e2d_type) {
	case EXT2_FT_REG_FILE: return S_IFREG;
	case EXT2_FT_DIR: return S_IFDIR;
	case EXT2_FT_SYMLINK: return S_IFLNK;
	case EXT2_FT_BLKDEV: return S_IFBLK;
	case EXT2_FT_CHRDEV: return S_IFCHR;
	case EXT2_FT_FIFO: return S_IFIFO;
	default: return 0;
	}
}

static uint8_t ext4_type_from_mode_fmt(mode_t mode) {
	switch (mode & S_IFMT) {
	case S_IFREG: return EXT2_FT_REG_FILE;
	case S_IFDIR: return EXT2_FT_DIR;
	case S_IFLNK: return EXT2_FT_SYMLINK;
	case S_IFBLK: return EXT2_FT_BLKDEV;
	case S_IFCHR: return EXT2_FT_CHRDEV;
	case S_IFIFO: return EXT2_FT_FIFO;
	default: return EXT2_FT_UNKNOWN;
	}
}

int ext4_close(struct inode *node) {
	struct ext4_file_info *fi;

	fi = inode_priv(node);

	if (NULL != fi) {
		if (NULL != fi->f_buf) {
			ext4_buff_free(node->i_sb->sb_data, fi->f_buf);
		}
	}

	return 0;
}

int ext4_open(struct inode *node) {
	int rc;
	char path[PATH_MAX];
	const char *cp, *ncp;
	uint32_t inumber, parent_inumber;

	struct ext4_file_info *fi;
	struct ext4_fs_info *fsi;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;

	/* prepare full path into this filesystem */
	vfs_get_relative_path(node, path, PATH_MAX);

	/* alloc a block sized buffer used for all transfers */
	if (NULL == (fi->f_buf = ext4_buff_alloc(fsi, fsi->s_block_size))) {
		return ENOMEM;
	}

	/* read group descriptor blocks */
	if (0 != (rc = ext4_read_gdblock(node))) {
		return rc;
	}

	if (0 != (rc = ext4_shift_culc(fi, fsi))) {
		return rc;
	}

	inumber = EXT2_ROOTINO;
	if (0 != (rc = ext4_read_inode(node, inumber))) {
		return rc;
	}

	cp = path;
	while (*cp) {
		/* Remove extra separators */
		while (*cp == '/') {
			cp++;
		}
		if (*cp == '\0') {
			break;
		}

		/* Check that current node is a directory */
		if (!S_ISDIR(fi->f_di.i_mode)) {
			rc = ENOTDIR;
			goto out;
		}

		/* Get next component of path name */
		ncp = cp;
		while ((*cp != '\0') && (*cp != '/')) {
			cp++;
		}
		/*
		 * Look up component in current directory. Save directory inumber
		 * in case we find a symbolic link.
		 */
		parent_inumber = inumber;
		if (0 != (rc = ext4_search_directory(node, ncp, cp - ncp, &inumber))) {
			goto out;
		}

		/* Open next component */
		if (0 != (rc = ext4_read_inode(node, inumber))) {
			goto out;
		}

		/* Check for symbolic link */
		if (S_ISLNK(fi->f_di.i_mode)) {
			if (0 != (rc = ext4_read_symlink(node, parent_inumber, &cp))) {
				goto out;
			}
		}
	}

	fi->f_num = inumber;

	return 0;

	out: ext4_close(node);
	return rc;
}

/*
 * file_operation
 */
static struct idesc *ext4fs_open(struct inode *node, struct idesc *idesc, int __oflag) {
	int rc;
	struct ext4_file_info *fi;
	struct ext4_fs_info *fsi;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;
	fi->f_pointer = file_get_pos(file_desc_from_idesc(idesc)); /* reset seek pointer */

	if (NULL == (fi->f_buf = ext4_buff_alloc(fsi, fsi->s_block_size))) {
		return err_ptr(ENOMEM);
	}

	if (0 != (rc = ext4_read_inode(node, fi->f_num))) {
		ext4_close(node);
		return err_ptr(rc);
	}
	else {
		file_set_size(file_desc_from_idesc(idesc), ext4_file_size(fi->f_di));
	}

	return idesc;
}

static int ext4fs_close(struct file_desc *desc) {
	if (NULL == desc) {
		return 0;
	}

	return ext4_close(desc->f_inode);
}

static size_t ext4fs_read(struct file_desc *desc, void *buff, size_t size) {
	int rc;
	size_t csize;
	char *buf;
	size_t buf_size;
	char *addr = buff;
	struct ext4_file_info *fi;

	fi = inode_priv(desc->f_inode);
	fi->f_pointer = file_get_pos(desc);

	while (size != 0) {
		/* XXX should handle LARGEFILE */
		if (fi->f_pointer >= ext4_file_size(fi->f_di)) {
			break;
		}

		if (0 != (rc = ext4_buf_read_file(desc->f_inode, &buf, &buf_size))) {
			SET_ERRNO(rc);
			return 0;
		}

		csize = size;
		if (csize > buf_size) {
			csize = buf_size;
		}

		memcpy(addr, buf, csize);

		fi->f_pointer += csize;
		addr += csize;
		size -= csize;
	}

	return (addr - (char *) buff);
}

static size_t ext4fs_write(struct file_desc *desc, void *buff, size_t size) {
	uint32_t bytecount;
	struct ext4_file_info *fi;

	fi = inode_priv(desc->f_inode);
	fi->f_pointer = file_get_pos(desc);

	bytecount = ext4_write_file(desc->f_inode, buff, size);

	file_set_size(desc, ext4_file_size(fi->f_di));

	return bytecount;
}

static int ext4_create(struct inode *i_new, struct inode *i_dir);
static int ext4_mkdir(struct inode *i_new, struct inode *i_dir);
static int ext4_unlink(struct inode *dir_node, struct inode *node);
static void ext4_free_fs(struct super_block *sb);
static int ext4fs_umount_entry(struct inode *node);

static int ext4fs_format(struct block_dev *dev, void *priv);
static int ext4fs_fill_sb(struct super_block *sb, const char *source);
static int ext4fs_clean_sb(struct super_block *sb);
static int ext4fs_mount(struct super_block *sb, struct inode *dest);

static struct fsop_desc ext4_fsop = {
	.mount	      = ext4fs_mount,
	//.create_node  = ext4fs_create,
	//.delete_node  = ext4fs_delete,

	.getxattr     = ext2fs_getxattr,
	.setxattr     = ext2fs_setxattr,
	.listxattr    = ext2fs_listxattr,

	//.truncate     = ext4fs_truncate,
	.umount_entry = ext4fs_umount_entry,
};

static struct fs_driver ext4fs_driver = {
	.name     = EXT4_NAME,
	.format       = ext4fs_format,
	.fill_sb  = ext4fs_fill_sb,
	.clean_sb = ext4fs_clean_sb,
//	.file_op  = &ext4_fop,
	.fsop     = &ext4_fsop,
};

static ext4_file_info_t *ext4_fi_alloc(struct inode *i_new, void *fs) {
	ext4_file_info_t *fi;

	fi = pool_alloc(&ext4_file_pool);
	if (fi) {
		fi->f_pointer = 0;
		inode_size_set(i_new, 0);
		inode_priv_set(i_new, fi);
	}

	return fi;
}

static int ext4fs_create(struct inode *node, struct inode *parent_node, int mode) {
	int rc;

	if (node_is_directory(node)) {
		if (0 != (rc = ext4_mkdir(node, parent_node))) {
			return -rc;
		}
		if (0 != (rc = ext4_mount_entry(node))) {
			return -rc;
		}
	} else {
		if (0 != (rc = ext4_create(node, parent_node))) {
			return -rc;
		}
	}
	return 0;
}

extern int main_mke2fs(int argc, char **argv);

static int ext4fs_format(struct block_dev *dev, void *priv) {
	int argc = 6;
	char *argv[6];
	char dev_path[64];

	strcpy(dev_path, "/dev/");
	strcat(dev_path, block_dev_name(dev));

	argv[0] = "mke2fs";
	argv[1] = "-b";
	argv[2] = "1024";
	argv[3] = "-t";
	argv[4] = "ext4";
	argv[5] = dev_path;

	getopt_init();
	return main_mke2fs(argc, argv);
}

static int ext4fs_delete(struct inode *node) {
	int rc;
	struct inode *parent;

	if (NULL == (parent = vfs_subtree_get_parent(node))) {
		rc = ENOENT;
		return -rc;
	}

	if (0 != (rc = ext4_unlink(parent, node))) {
		return -rc;
	}

	return 0;
}

static int ext4fs_fill_sb(struct super_block *sb, const char *source) {
	struct ext4_fs_info *fsi;
	struct block_dev *bdev = bdev_by_path(source);
	if (NULL == bdev) {
		return -ENODEV;
	}
	sb->bdev = bdev;

	if (NULL == (fsi = pool_alloc(&ext4_fs_pool))) {
		return -ENOMEM;
	}
	memset(fsi, 0, sizeof(struct ext4_fs_info));
	sb->sb_data = fsi;
	sb->sb_ops = &ext4fs_sbops;
	sb->sb_iops = &ext4_iops;
	sb->sb_fops = &ext4_fop;

	return 0;
}

static int ext4fs_mount(struct super_block *sb, struct inode *dest) {
	int rc;
	struct ext4_file_info *fi;
	struct ext4_fs_info *fsi;

	if (NULL == (fi = pool_alloc(&ext4_file_pool))) {
		rc = ENOMEM;
		goto error;
	}
	memset(fi, 0, sizeof(struct ext4_file_info));
	fi->f_pointer = 0;
	inode_priv_set(dest, fi);

	/* presetting that we think */
	fsi = sb->sb_data;
	fsi->s_block_size = SBSIZE;
	fsi->s_sectors_in_block = fsi->s_block_size / 512;
	if (0 != (rc = ext4_read_sblock(dest))) {
		goto error;
	}
	if (NULL == (fsi->e4fs_gd = ext4_buff_alloc(fsi,
			sizeof(struct ext4_group_desc) * fsi->s_ncg))) {
		rc = ENOMEM;
		goto error;
	}
	if (0 != (rc = ext4_read_gdblock(dest))) {
		goto error;
	}
#if 0
	if (0 != (rc = ext4_mount_entry(dest))) {
		goto error;
	}
#endif
	return 0;

error:
	ext4_free_fs(sb);

	return -rc;
}

static int ext4fs_truncate (struct inode *node, off_t length) {

	inode_size_set(node, length);

	return 0;
}

static int ext4fs_clean_sb(struct super_block *sb) {
	ext4_free_fs(sb);
	return 0;
}

static void ext4_free_fs(struct super_block *sb) {
	struct ext4_fs_info *fsi = sb->sb_data;

	if (NULL != fsi) {
		if(NULL != fsi->e4fs_gd) {
			ext4_buff_free(fsi, (char *) fsi->e4fs_gd);
		}
		pool_free(&ext4_fs_pool, fsi);
	}
}

static int ext4fs_umount_entry(struct inode *node) {
	pool_free(&ext4_file_pool, inode_priv(node));

	return 0;
}

/*
 * Read a new inode into a file structure.
 */
static int ext4_read_inode(struct inode *node, uint32_t inumber) {
	char *buf;
	size_t rsize;
	int64_t inode_sector;
	struct ext4_inode *dip;
	struct ext4_file_info *fi;
	struct ext4_fs_info *fsi;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;

	rsize = EXT4_DINODE_SIZE(fsi);

	inode_sector = ext4_ino_to_fsba(fsi, inumber);

	/* Read inode and save it */
	buf = fi->f_buf;
	rsize = ext4_read_sector(node, buf, 1, inode_sector);
	if (rsize * fsi->s_block_size != fsi->s_block_size) {
		return EIO;
	}

	/* set pointer to inode struct in read buffer */
	dip = (struct ext4_inode *) (buf
			+ EXT4_DINODE_SIZE(fsi) * ext4_ino_to_fsbo(fsi, inumber));
	/* load inode struct to file info */
	e4fs_iload(dip, &fi->f_di);

	/* Clear out the old buffers */
	fi->f_ind_cache_block = ~0;
	fi->f_buf_blkno = -1;
	return 0;
}

/*
 * Given an offset in a file, find the disk block number that
 * contains that block.
 */
static int ext4_block_map(struct inode *node, int32_t file_block,
		uint32_t *disk_block_p) {
	uint32_t len;
	struct ext4_file_info *fi = inode_priv(node);

	*disk_block_p = ext4_extent_get_pblock(node, fi->f_di.i_block, file_block, &len);

	return 0;
}

/*
 * Read a portion of a file into an internal buffer.
 * Return the location in the buffer and the amount in the buffer.
 */
static int ext4_buf_read_file(struct inode *node, char **buf_p, size_t *size_p) {
	int rc;
	long off;
	int32_t file_block;
	uint32_t disk_block;
	size_t block_size;
	struct ext4_file_info *fi;
	struct ext4_fs_info *fsi;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;

	off = blkoff(fsi, fi->f_pointer);
	file_block = lblkno(fsi, fi->f_pointer);
	block_size = fsi->s_block_size; /* no fragment */


	if (file_block != fi->f_buf_blkno) {
		if (0 != (rc = ext4_block_map(node, file_block, &disk_block))) {
			return rc;
		}

		if (disk_block == 0) {
			memset(fi->f_buf, 0, block_size);
			fi->f_buf_size = block_size;
		}
		else {
			if (1 != ext4_read_sector(node, fi->f_buf, 1, disk_block)) {
				return EIO;
			}
		}

		fi->f_buf_blkno = file_block;
	}

	/*
	 * Return address of byte in buffer corresponding to
	 * offset, and size of remainder of buffer after that
	 * byte.
	 */
	*buf_p = fi->f_buf + off;
	*size_p = block_size - off;

	/* But truncate buffer at end of file */
	/* XXX should handle LARGEFILE */
	if (*size_p > ext4_file_size(fi->f_di) - fi->f_pointer) {
		*size_p = ext4_file_size(fi->f_di) - fi->f_pointer;
	}

	return 0;
}
/*
 * Write a portion to a file from an internal buffer.
 */
static size_t ext4_write_file(struct inode *node, char *buf, size_t size) {
	long inblock_off;
	int32_t file_block;
	uint32_t disk_block;
	char *buff;
	size_t block_size, len, cnt;
	size_t bytecount, end_pointer;
	struct ext4_inode fdi;
	struct ext4_file_info *fi;
	struct ext4_fs_info *fsi;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;

	block_size = fsi->s_block_size; /* no fragment */
	bytecount = 0;
	len = size;
	buff = buf;
	end_pointer = fi->f_pointer + len;

	if (0 != ext4_new_block(node, end_pointer - 1)) {
		return 0;
	}

	while (1) {
		file_block = lblkno(fsi, fi->f_pointer);

		if (0 != ext4_block_map(node, file_block, &disk_block)) {
			return 0;
		}

		/* if need alloc new block for a file */
		if (0 == disk_block) {
			return bytecount;
		}

		fi->f_buf_blkno = file_block;

		/* calculate f_pointer in scratch buffer */
		inblock_off = blkoff(fsi, fi->f_pointer);

		/* set the counter how many bytes written in block */
		/* more than block */
		if (end_pointer - fi->f_pointer > block_size) {
			if (0 != inblock_off) { /* write a part of block */
				cnt = block_size - inblock_off;
			}
			else { /* write the whole block */
				cnt = block_size;
			}
		}
		else {
			cnt = end_pointer - fi->f_pointer;
			/* over the block ? */
			if ((inblock_off + cnt) > block_size) {
				cnt -= blkoff(fsi, (inblock_off + cnt));
			}
		}

		/* one 4096-bytes block read operation */
		if (1 != ext4_read_sector(node, fi->f_buf, 1, disk_block)) {
			bytecount = 0;
			break;
		}
		/* set new data in block */
		memcpy(fi->f_buf + inblock_off, buff, cnt);

		/* write one block to device */
		if (1 != ext4_write_sector(node, fi->f_buf, 1, disk_block)) {
			bytecount = 0;
			break;
		}
		bytecount += cnt;
		buff += cnt;
		/* shift the f_pointer */
		fi->f_pointer += cnt;
		if (end_pointer <= fi->f_pointer) {
			break;
		}
	}
	/* if we write over the last EOF, set new filelen */
	if (ext4_file_size(fi->f_di) < fi->f_pointer) {
		fi->f_di.i_size_lo = fi->f_pointer;
		fi->f_di.i_size_hi = 0;
	}
	memcpy(&fdi, &fi->f_di, sizeof(struct ext4_inode));
	ext4_rw_inode(node, &fdi, EXT4_W_INODE);

	return bytecount;
}

/*
 * Search a directory for a name and return its
 * inode number.
 */
static int ext4_search_directory(struct inode *node, const char *name, int length,
		uint32_t *inumber_p) {
	int rc;
	struct ext4_dir *dp;
	struct ext4_dir *edp;
	char *buf;
	size_t buf_size;
	int namlen;
	struct ext4_file_info *fi;

	fi = inode_priv(node);
	fi->f_pointer = 0;
	/* XXX should handle LARGEFILE */
	while (fi->f_pointer < (long) ext4_file_size(fi->f_di)) {
		if (0 != (rc = ext4_buf_read_file(node, &buf, &buf_size))) {
			return rc;
		}

		dp = (struct ext4_dir *) buf;
		edp = (struct ext4_dir *) (buf + buf_size);
		for (; dp < edp;
				dp = (struct ext4_dir *) ((char *) dp
						+ fs2h16(dp->rec_len))) {
			if (fs2h16(dp->rec_len) <= 0) {
				break;
			}
			if (fs2h32(dp->inode) == (uint32_t) 0) {
				continue;
			}
			namlen = dp->name_len;
			if (namlen == length && !memcmp(name, dp->name, length)) {
				/* found entry */
				*inumber_p = fs2h32(dp->inode);
				return 0;
			}
		}
		fi->f_pointer += buf_size;
	}

	return ENOENT;
}

int ext4_write_sblock(struct inode *node) {
	struct ext4_fs_info *fsi;

	fsi = node->i_sb->sb_data;

	if (1 != ext4_write_sector(node, (char *) &fsi->e4sb, 1,
					dbtofsb(fsi, SBOFF / SECTOR_SIZE))) {
		return EIO;
	}
	return 0;
}

static int ext4_read_sblock(struct inode *node) {
	void *sbbuf = NULL;
	struct ext4_fs_info *fsi;
	struct ext4_super_block *ext4sb;
	int ret = 0;

	fsi = node->i_sb->sb_data;
	ext4sb = &fsi->e4sb;

	if (!(sbbuf = ext4_buff_alloc(fsi, fsi->s_block_size))) {
		return ENOMEM;
	}

	if (1 != ext4_read_sector(node, (char *) sbbuf, 1,
					dbtofsb(fsi, SBOFF / SECTOR_SIZE))) {
		ret = EIO;
		goto out;
	}

	e2fs_sbload(sbbuf, ext4sb);
	ext4_buff_free(fsi, sbbuf);

	if (ext4sb->s_magic != E2FS_MAGIC) {
		ret = EINVAL;
		goto out;
	}
//	if (ext2sb->s_rev_level > E2FS_REV1
//		|| (ext2sb->s_rev_level == E2FS_REV1
//		&& (ext2sb->s_first_ino != EXT4_FIRSTINO
//		|| (ext2sb->s_inode_size != 128
//		&& ext2sb->s_inode_size != 256)
//		|| ext2sb->s_feature_incompat & ~EXT2F_INCOMPAT_SUPP))) {
//		ret = ENODEV;
//		goto out;
//
//	}

	/* compute in-memory ext4_fs_info values */
	fsi->s_ncg =
			howmany(fsi->e4sb.s_blocks_count - fsi->e4sb.s_first_data_block,
					fsi->e4sb.s_blocks_per_group);
	/* XXX assume hw bsize = 512 */
	fsi->s_fsbtodb = fsi->e4sb.s_log_block_size + 1;
	fsi->s_block_size = MINBSIZE << fsi->e4sb.s_log_block_size;
	fsi->s_bshift = LOG_MINBSIZE + fsi->e4sb.s_log_block_size;
	fsi->s_qbmask = fsi->s_block_size - 1;
	fsi->s_bmask = ~fsi->s_qbmask;
	fsi->s_gdb_count =
			howmany(fsi->s_ncg, fsi->s_block_size / sizeof(struct ext4_group_desc));
	fsi->s_inodes_per_block = fsi->s_block_size / ext4sb->s_inode_size;
	fsi->s_itb_per_group = fsi->e4sb.s_inodes_per_group
			/ fsi->s_inodes_per_block;

	fsi->s_groups_count = ((fsi->e4sb.s_blocks_count
			- fsi->e4sb.s_first_data_block - 1) / fsi->e4sb.s_blocks_per_group)
			+ 1;
	fsi->s_bsearch = fsi->e4sb.s_first_data_block + 1 + fsi->s_gdb_count + 2
			+ fsi->s_itb_per_group;

	fsi->s_blocksize_bits = fsi->e4sb.s_log_block_size + 10;

	fsi->s_desc_per_block = fsi->s_block_size / sizeof(struct ext4_group_desc);

out:
	ext4_buff_free(fsi, sbbuf);
	return ret;
}

static uint16_t ext4_checksum(struct ext4_fs_info *fsi, uint32_t block_nr,
		struct ext4_group_desc *gdp) {

	int offset;
	uint16_t crc = 0;

	offset = offsetof(struct ext4_group_desc, checksum);

	crc = crc16(~0, fsi->e4sb.s_uuid, sizeof(fsi->e4sb.s_uuid));
	crc = crc16(crc, (__u8 *)&block_nr, sizeof(block_nr));
	crc = crc16(crc, (__u8 *)gdp, offset);
	offset += sizeof(gdp->checksum);

	crc = crc16(crc, (__u8 *) gdp + offset,
			sizeof(struct ext4_group_desc) - offset);

	return crc;
}


int ext4_write_gdblock(struct inode *node) {
	uint gdpb;
	int i;
	char *buff;
	struct ext4_fs_info *fsi;

	fsi = node->i_sb->sb_data;

	gdpb = fsi->s_block_size / sizeof(struct ext4_group_desc);

	for (i = 0; i < fsi->s_gdb_count; i += gdpb) {
		buff = (char *) &fsi->e4fs_gd[i * gdpb];

		fsi->e4fs_gd[i * gdpb].checksum = ext4_checksum(fsi, i / gdpb, &fsi->e4fs_gd[i * gdpb]);

		if (1 != ext4_write_sector(node, buff, 1,
						fsi->e4sb.s_first_data_block + 1 + i / gdpb)) {
			return EIO;
		}
	}

	return 0;
}

static int ext4_read_gdblock(struct inode *node) {
	size_t rsize;
	uint gdpb;
	int i;
	struct ext4_fs_info *fsi;
	void *gdbuf = NULL;
	int ret = 0;

	fsi = node->i_sb->sb_data;

	gdpb = fsi->s_block_size / sizeof(struct ext4_group_desc);

	if (!(gdbuf = ext4_buff_alloc(fsi, fsi->s_block_size))) {
		return ENOMEM;
	}

	for (i = 0; i < fsi->s_gdb_count; i++) {
		rsize = ext4_read_sector(node, gdbuf, 1,
				fsi->e4sb.s_first_data_block + 1 + i);
		if (rsize * fsi->s_block_size != fsi->s_block_size) {
			ret = EIO;
			goto out;

		}
		e2fs_cgload((struct ext4_group_desc *)gdbuf, &fsi->e4fs_gd[i * gdpb],
				(i == (fsi->s_gdb_count - 1)) ?
				(fsi->s_ncg - gdpb * i) * sizeof(struct ext4_group_desc)
				: fsi->s_block_size);
	}
out:
	ext4_buff_free(fsi, gdbuf);
	return ret;
}

struct inode *ext4_lookup(char const *name, struct inode const *dir) {
	return NULL;
}

static int ext4_iterate(struct inode *next, char *next_name, struct inode *parent, struct dir_ctx *dir_ctx) {
//	mode_t mode;
	char name_buff[NAME_MAX];
	struct ext4_fs_info *fsi;
	struct ext4_file_info *dir_fi;
	struct ext4_file_info *fi;
	struct ext4_dir *dp, *edp;
	size_t buf_size;
	char *buf;
	int rc;
	char *name;
	int idx = 0;

	fsi = parent->i_sb->sb_data;

	if (0 != ext4_open(parent)) {
		return -1;
	}

	dir_fi = inode_priv(parent);

	dir_fi->f_pointer = 0;
	while (dir_fi->f_pointer < ext4_file_size(dir_fi->f_di)) {
		if (0 != (rc = ext4_buf_read_file(parent, &buf, &buf_size))) {
			goto out;
		}
		if (buf_size != fsi->s_block_size || buf_size == 0) {
			rc = EIO;
			goto out;
		}

		dp = (struct ext4_dir *) buf;
		edp = (struct ext4_dir *) (buf + buf_size);
		for (; dp < edp; dp = (void *)((char *)dp + fs2h16(dp->rec_len))) {
			if (fs2h16(dp->rec_len) <= 0) {
				goto out;
			}
			if (fs2h32(dp->inode) == 0) {
				continue;
			}

			/* set null determine name */
			name = (char *) &dp->name;

			memcpy(name_buff, name, fs2h16(dp->name_len));
			name_buff[fs2h16(dp->name_len)] = '\0';

			if(0 != path_is_dotname(name_buff, dp->name_len)) {
				/* dont need create dot or dotdot node */
				continue;
			}

			if (idx++ < (int)(uintptr_t)dir_ctx->fs_ctx) {
				continue;
			}

			//mode = ext4_type_to_mode_fmt(dp->e2d_type);
			fi = ext4_fi_alloc(next, parent->i_sb);
			if (!fi) {
				goto out;
			}
			memset(fi, 0, sizeof(struct ext4_file_info));
			fi->f_num = fs2h32(dp->inode);

			next->i_sb = parent->i_sb;

			/* Load permisiions and credentials. */
			if (NULL == (fi->f_buf = ext4_buff_alloc(fsi, fsi->s_block_size))) {
				rc = ENOSPC;
				goto out;
			}

			ext4_read_inode(next, fs2h32(dp->inode));
			ext4_buff_free(fsi, fi->f_buf);

			next->i_mode = fi->f_di.i_mode;

			next->i_owner_id = fi->f_di.i_uid;
			next->i_group_id = fi->f_di.i_gid;
			inode_size_set(next, ext4_file_size(fi->f_di));
			strncpy(next_name, name_buff, NAME_MAX - 1);
			next_name[NAME_MAX - 1] = '\0';

			dir_ctx->fs_ctx = (void *)(uintptr_t)idx;
			ext4_close(parent);
			return 0;
		}
		dir_fi->f_pointer += buf_size;
	}

out:
	ext4_close(parent);

	return -1;
}

static int ext4_mount_entry(struct inode *dir_node) {
	int rc;
	char *buf;
	size_t buf_size;
	struct ext4_dir *dp, *edp;
	struct ext4_file_info *dir_fi, *fi;
	struct ext4_fs_info *fsi;
	char *name, *name_buff;
	struct inode *node;
	mode_t mode;

	rc = 0;

	if (NULL == (name_buff = ext4_buff_alloc(dir_node->i_sb->sb_data, NAME_MAX))) {
		rc = ENOMEM;
		return rc;
	}

	fsi = dir_node->i_sb->sb_data;
	
	if (0 != ext4_open(dir_node)) {
		goto out;
	}

	dir_fi = inode_priv(dir_node);

	dir_node->i_mode = dir_fi->f_di.i_mode;
	dir_node->i_owner_id = dir_fi->f_di.i_uid;
	dir_node->i_group_id = dir_fi->f_di.i_gid;

	dir_fi->f_pointer = 0;
	while (dir_fi->f_pointer < ext4_file_size(dir_fi->f_di)) {
		if (0 != (rc = ext4_buf_read_file(dir_node, &buf, &buf_size))) {
			goto out;
		}
		if (buf_size != fsi->s_block_size || buf_size == 0) {
			rc = EIO;
			goto out;
		}

		dp = (struct ext4_dir *) buf;
		edp = (struct ext4_dir *) (buf + buf_size);

		for (; dp < edp; dp = (void *) ((char *) dp +
								fs2h16(dp->rec_len))) {
			if (fs2h16(dp->rec_len) <= 0) {
				goto out;
			}
			if (fs2h32(dp->inode) == 0) {
				continue;
			}

			/* set null determine name */
			name = (char *) &dp->name;

			memcpy(name_buff, name, fs2h16(dp->name_len));
			name_buff[fs2h16(dp->name_len)] = '\0';

			if(0 != path_is_dotname(name_buff, dp->name_len)) {
				/* dont need create dot or dotdot node */
				continue;
			}

			mode = ext4_type_to_mode_fmt(dp->type);

			node = vfs_subtree_create(dir_node, name_buff, mode);
			if (!node) {
				rc = ENOMEM;
				goto out;
			}

			fi = ext4_fi_alloc(node, dir_node->i_sb);
			if (!fi) {
				vfs_del_leaf(node);
				rc = ENOMEM;
				goto out;
			}

			if (node_is_directory(node)) {
				rc = ext4_mount_entry(node);
			} else {
				/* read inode into fi->f_di*/
				if (0 == ext4_open(node)) {
					/* Load permisiions and credentials. */
					assert((node->i_mode & S_IFMT) == (fi->f_di.i_mode & S_IFMT));
					node->i_mode = fi->f_di.i_mode;
					node->i_owner_id = fi->f_di.i_uid;
					node->i_group_id = fi->f_di.i_gid;
				}
				ext4_close(node);
			}
		}
		dir_fi->f_pointer += buf_size;
	}

	out: ext4_close(dir_node);
	ext4_buff_free(fsi, name_buff);
	return rc;
}

static int ext4_dir_operation(struct inode *node, char *string, ino_t *numb,
		int flag, mode_t mode_fmt);


static int ext4_new_block(struct inode *node, long position) {
	/* Acquire a new block and return a pointer to it.*/
	int rc;
	uint32_t goal;
	long position_diff;
	struct ext4_file_info *fi;
	struct ext4_fs_info *fsi;
	uint32_t b, lblock;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;

	lblock = lblkno(fsi, position);

	if (0 != (rc = ext4_block_map(node, lblock, &b))) {
		return rc;
	}
	/* Is another block available? */
	if (EXT4_NO_BLOCK == b) {
		/* Check if this position follows last allocated block. */
		goal = EXT4_NO_BLOCK;
		if (fi->f_last_pos_bl_alloc != 0) {
			position_diff = position - fi->f_last_pos_bl_alloc;
			if (0 == fi->f_bsearch) {
				/* Should never happen, but not critical */
				return -1;
			}
			if (position_diff <= fsi->s_block_size) {
				goal = fi->f_bsearch + 1;
			}
		}

		if (EXT4_NO_BLOCK == (b = ext4_alloc_block(node, goal))) {
			return ENOSPC;
		}

		ext4_extent_add_block(node, lblock, b);
	}
	return 0;
}

static int ext4_new_node(struct inode *i_new, struct inode *i_dir);

static int ext4_create(struct inode *i_new, struct inode *i_dir) {
	int rc;
	struct ext4_file_info *fi, *dir_fi;
	struct ext4_fs_info *fsi;

	dir_fi = inode_priv(i_dir);

	if (0 != (rc = ext4_open(i_dir))) {
		return rc;
	}

	/* Create a new file inode */
	if (0 != (rc = ext4_new_node(i_new, i_dir))) {
		return rc;
	}
	fi = inode_priv(i_new);

	dir_fi->f_di.i_links_count++;
	ext4_rw_inode(i_dir, &dir_fi->f_di, EXT4_W_INODE);

	ext4_close(i_dir);

	fsi = i_new->i_sb->sb_data;
	if (NULL != fi) {
		ext4_buff_free(fsi, fi->f_buf);
		return 0;
	}
	return ENOSPC;
}

static int ext4_mkdir(struct inode *i_new, struct inode *i_dir) {
	int rc;
	int r1, r2; /* status codes */
	ino_t dot, dotdot; /* inode numbers for . and .. */
	struct ext4_file_info *fi, *dir_fi;
	struct ext4_fs_info *fsi;

	if (!node_is_directory(i_dir)) {
		rc = ENOTDIR;
		return rc;
	}

	dir_fi = inode_priv(i_dir);

	/* read the directory inode */
	if (0 != (rc = ext4_open(i_dir))) {
		return rc;
	}
	/* Create a new directory inode. */
	if (0 != (rc = ext4_new_node(i_new, i_dir))) {
		ext4_close(i_dir);
		return ENOSPC;
	}
	fi = inode_priv(i_new);
	/* Get the inode numbers for . and .. to enter in the directory. */
	dotdot = dir_fi->f_num; /* parent's inode number */
	dot = fi->f_num; /* inode number of the new dir itself */
	/* Now make dir entries for . and .. unless the disk is completely full. */
	/* Use dot1 and dot2, so the mode of the directory isn't important. */
	/* enter . in the new dir*/
	r1 = ext4_dir_operation(i_new, ".", &dot, ENTER, S_IFDIR);
	/* enter .. in the new dir */
	r2 = ext4_dir_operation(i_new, "..", &dotdot, ENTER, S_IFDIR);

	/* If both . and .. were successfully entered, increment the link counts. */
	if (r1 != 0 || r2 != 0) {
		/* It was not possible to enter . or .. probably disk was full -
		 * links counts haven't been touched.
		 */
		ext4_dir_operation(i_dir, inode_name(i_new)/*string*/,
				&dot, DELETE, 0);
		/* TODO del inode and clear the pool*/
		return (r1 | r2);
	}

	dir_fi->f_di.i_links_count++;
	ext4_rw_inode(i_dir, &dir_fi->f_di, EXT4_W_INODE);

	fsi = i_new->i_sb->sb_data;
	ext4_buff_free(fsi, fi->f_buf);
	ext4_close(i_dir);

	if (NULL == fi) {
		return ENOSPC;
	}
	return 0;
}

static void ext4_wipe_inode(struct ext4_file_info *fi,
		struct ext4_file_info *dir_fi) {
	/* Erase some fields in the ext4_file_info. This function is called from alloc_inode()
	 * when a new ext4_file_info is to be allocated, and from truncate(), when an existing
	 * ext4_file_info is to be truncated.
	 */
	struct ext4_inode *di = &fi->f_di;
	struct ext4_inode *dir_di = &dir_fi->f_di;

	di->i_size_lo = 0;
	di->i_size_hi = 0;
	di->i_blocks_lo = 0;
	di->i_flags = 0;
	di->i_obso_faddr = 0;

	for (int i = 0; i < EXT4_N_BLOCKS; i++) {
		di->i_block[i] = NO_BLOCK;
	}

	di->i_mode  = dir_di->i_mode & ~S_IFMT;
	di->i_uid   = dir_di->i_uid;
	di->i_atime = dir_di->i_atime;
	di->i_ctime = dir_di->i_ctime;
	di->i_mtime = dir_di->i_mtime;
	di->i_dtime = dir_di->i_dtime;
	di->i_gid   = dir_di->i_gid;
}

/*
 * Find first group which has free inode slot.
 */
static int ext4_find_group_any(struct ext4_fs_info *fsi) {
	int group, ngroups;
	struct ext4_group_desc *gd;

	group = 0;
	ngroups = fsi->s_groups_count;

	for (; group < ngroups; group++) {
		gd = ext4_get_group_desc(group, fsi);
		if (gd == NULL ) {
			return EIO;
		}
		if (ext4_free_inodes_count(gd)) {
			return group;
		}
	}
	return EIO;
}

static int ext4_free_inode_bit(struct inode *node, uint32_t bit_returned,
		int is_dir) {
	/* Return an inode by turning off its bitmap bit. */
	int group; /* group number of bit_returned */
	int bit; /* bit_returned number within its group */
	struct ext4_group_desc *gd;
	struct ext4_fs_info *fsi;
	struct ext4_file_info *fi;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;
	/* At first search group, to which bit_returned belongs to
	 * and figure out in what word bit is stored.
	 */
	if (bit_returned > fsi->e4sb.s_inodes_count||
	bit_returned < EXT2_FIRST_INO(&fsi->e4sb)) {
		return ENOMEM;
	}

	group = (bit_returned - 1) / fsi->e4sb.s_inodes_per_group;
	bit = (bit_returned - 1) % fsi->e4sb.s_inodes_per_group; /* index in bitmap */

	if (NULL == (gd = ext4_get_group_desc(group, fsi))) {
		return ENOMEM;
	}
	if (1 != ext4_read_sector(node, fi->f_buf, 1, ext4_inode_bitmap_len(gd))) {
		return EIO;
	}
	if (ext4_unsetbit(b_bitmap(fi->f_buf), bit)) {
		return EIO;
	}

	if (1 != ext4_write_sector(node, fi->f_buf, 1, ext4_inode_bitmap_len(gd))) {
		return EIO;
	}
	ext4_inc_lo_hi(gd->free_inodes_count_lo, gd->free_inodes_count_hi);
	fsi->e4sb.s_free_inodes_count++;
	if (is_dir) {
		ext4_dec_lo_hi(gd->used_dirs_count_lo, gd->used_dirs_count_hi);
	}

	return (ext4_write_sblock(node) | ext4_write_gdblock(node));
}

static uint32_t ext4_alloc_inode_bit(struct inode *node, int is_dir) { /* inode will be a directory if it is TRUE */
	int group;
	ino_t inumber;
	uint32_t bit;
	struct ext4_group_desc *gd;
	struct ext4_file_info *fi;
	struct ext4_fs_info *fsi;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;
	inumber = 0;

	group = ext4_find_group_any(fsi);

	/* Check if we have a group where to allocate an ext4_file_info */
	if (group == -1) {
		return 0; /* no bit could be allocated */
	}

	if (NULL == (gd = ext4_get_group_desc(group, fsi))) {
		return 0;
	}

	/* find_group_* should always return either a group with
	 * a free ext4_file_info slot or -1, which we checked earlier.
	 */
	if (1 != ext4_read_sector(node, fi->f_buf, 1, ext4_inode_bitmap_len(gd))) {
		return 0;
	}

	bit = ext4_setbit(b_bitmap(fi->f_buf), fsi->e4sb.s_inodes_per_group, 0);

	inumber = group * fsi->e4sb.s_inodes_per_group + bit + 1;

	/* Extra checks before real allocation.
	 * Only major bug can cause problems. Since setbit changed
	 * bp->b_bitmap there is no way to recover from this bug.
	 * Should never happen.
	 */
	if (inumber > fsi->e4sb.s_inodes_count) {
		return 0;
	}

	if (inumber < EXT4_FIRST_INO(&fsi->e4sb)) {
		return 0;
	}

	ext4_write_sector(node, fi->f_buf, 1, ext4_inode_bitmap_len(gd));

	ext4_dec_lo_hi(gd->free_inodes_count_lo, gd->free_inodes_count_hi);
	fsi->e4sb.s_free_inodes_count--;
	if (is_dir) {
		ext4_inc_lo_hi(gd->used_dirs_count_lo, gd->used_dirs_count_hi);
	}
	if (ext4_write_sblock(node) || ext4_write_gdblock(node)) {
		inumber = 0;
	}

	return inumber;
}

static int ext4_free_inode(struct inode *node) { /* ext4_file_info to free */
	/* Return an ext4_file_info to the pool of unallocated inodes. */
	int rc;
	uint32_t pos;
	uint32_t b;
	struct ext4_inode fdi;
	struct ext4_file_info *fi;
	struct ext4_fs_info *fsi;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;

	/* Locate the appropriate super_block. */
	if (0!= (rc = ext4_read_sblock(node))) {
		return rc;
	}

	/* free all data block of file */
	for(pos = 0; pos <= ext4_file_size(fi->f_di); pos += fsi->s_block_size) {
		if (0 != (rc = ext4_block_map(node, lblkno(fsi, pos), &b))) {
			return rc;
		}
		ext4_free_block(node, b);
	}

	/* clear inode in inode table */
	memset(&fdi, 0, sizeof(struct ext4_inode));
	ext4_rw_inode(node, &fdi, EXT4_W_INODE);

	/* free inode bitmap */
	b = fi->f_num;
	if (b <= 0 || b > fsi->e4sb.s_inodes_count) {
		return ENOSPC;
	}
	rc = ext4_free_inode_bit(node, b, node_is_directory(node));

	ext4_close(node);
	pool_free(&ext4_file_pool, fi);

	return rc;
}

static int ext4_alloc_inode(struct inode *i_new, struct inode *i_dir) {
	/* Allocate a free inode in inode table and return a pointer to it. */
	int rc;
	struct ext4_file_info *fi, *dir_fi;
	struct ext4_fs_info *fsi;
	uint32_t b;

	dir_fi = inode_priv(i_dir);
	fsi = i_dir->i_sb->sb_data;

	if (NULL == (fi = ext4_fi_alloc(i_new, i_dir->i_sb))) {
		rc = ENOSPC;
		goto out;
	}
	memset(fi, 0, sizeof(struct ext4_file_info));

	if (NULL == (fi->f_buf = ext4_buff_alloc(fsi, fsi->s_block_size))) {
		rc = ENOSPC;
		goto out;
	}

	if (0 != (rc = ext4_read_sblock(i_new))) {
		goto out;
	}

	/* Acquire an inode from the bit map. */
	if (0 == (b = ext4_alloc_inode_bit(i_new, node_is_directory(i_new)))) {
		rc = ENOSPC;
		goto out;
	}

	fi->f_num = b;
	ext4_wipe_inode(fi, dir_fi);

	return 0;

out:
	vfs_del_leaf(i_new);
	if (NULL != fi) {
		if (NULL != fi->f_buf) {
			ext4_buff_free(fsi, fi->f_buf);
		}
		pool_free(&ext4_file_pool, fi);
	}
	return rc;
}

static void ext4_rw_inode(struct inode *node, struct ext4_inode *fdi,
		int rw_flag) {
	/* An entry in the inode table is to be copied to or from the disk. */

	struct ext4_group_desc *gd;
	struct ext4_inode *dip;
	unsigned int block_group_number;
	uint32_t b, offset;
	struct ext4_file_info *fi;
	struct ext4_fs_info *fsi;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;

	/* Get the block where the inode resides. */
	ext4_read_sblock(node);

	block_group_number = (fi->f_num - 1) / fsi->e4sb.s_inodes_per_group;
	if (NULL == (gd = ext4_get_group_desc(block_group_number, fsi))) {
		return;
	}

	offset = ((fi->f_num - 1) % fsi->e4sb.s_inodes_per_group)
			* EXT4_INODE_SIZE(&fsi->e4sb);
	/* offset requires shifting, since each block contains several inodes,
	 * e.g. inode 2 is stored in bklock 0.
	 */
	b = (uint32_t) ext4_inode_table_len(gd) + (offset >> fsi->s_blocksize_bits);

	ext4_read_sector(node, fi->f_buf, 1, b);

	offset &= (fsi->s_block_size - 1);
	dip = (struct ext4_inode*) (b_data(fi->f_buf) + offset);

	/* Do the read or write. */
	if (rw_flag) {
		memcpy(dip, fdi, sizeof(struct ext4_inode));
		ext4_write_sector(node, fi->f_buf, 1, b);
		ext4_write_gdblock(node);
	}
	else {
		memcpy(fdi, dip, sizeof(struct ext4_inode));
	}
}

static int ext4_dir_operation(struct inode *node, char *string, ino_t *numb,
		int flag, mode_t mode_fmt) {
	/* This function searches the directory whose inode is pointed to :
	 * if (flag == ENTER)  enter 'string' in the directory with inode # '*numb';
	 * if (flag == DELETE) delete 'string' from the directory;
	 * if (flag == LOOK_UP) search for 'string' and return inode # in 'numb';
	 * if (flag == IS_EMPTY) return OK if only . and .. in dir else ENOTEMPTY;
	 *
	 *    if 'string' is dot1 or dot2, no access permissions are checked.
	 */
	int rc;
	struct ext4_dir *dp = NULL;
	struct ext4_dir *prev_dp = NULL;
	int i, e_hit, t, match;
	uint32_t pos;
	unsigned new_slots;
	uint32_t b = 0;
	int extended;
	int required_space;
	int string_len;
	int new_slot_size, actual_size;
	u16_t temp;
	struct ext4_inode fdi;
	struct ext4_file_info *fi;
	struct ext4_fs_info *fsi;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;
	/* If 'fi' is not a pointer to a dir inode, error. */
	if (!node_is_directory(node)) {
		return ENOTDIR;
	}

	e_hit = match = 0; /* set when a string match occurs */
	new_slots = 0;
	pos = 0;
	extended = required_space = string_len = 0;

	if (ENTER == flag) {
		string_len = strlen(string);
		required_space = MIN_DIR_ENTRY_SIZE + string_len;
		required_space +=
				(required_space & 0x03) == 0 ?
						0 : (DIR_ENTRY_ALIGN - (required_space & 0x03));
	}

	for (; pos < ext4_file_size(fi->f_di); pos += fsi->s_block_size) {
		if (0 != (rc = ext4_block_map(node, lblkno(fsi, pos), &b))) {
			return rc;
		}

		/* Since directories don't have holes, 'b' cannot be NO_BLOCK. */
		/* get a dir block */
		if (1 != ext4_read_sector(node, fi->f_buf, 1, b)) {
			return EIO;
		}
		prev_dp = NULL; /* New block - new first dentry, so no prev. */

		/* Search a directory block.
		 * Note, we set prev_dp at the end of the loop.
		 */
		for (dp = (struct ext4_dir*) &b_data(fi->f_buf) ;
				CUR_DISC_DIR_POS(dp, &b_data(fi->f_buf)) < fsi->s_block_size;
				dp = EXT4_NEXT_DISC_DIR_DESC(dp) ) {

			if (prev_dp == dp) {
				/* no dp in directory entry */
				dp->rec_len = fsi->s_block_size;
			}
			/* Match occurs if string found. */
			if (ENTER != flag && 0 != dp->inode) {
				if (IS_EMPTY == flag) {
					/* If this test succeeds, dir is not empty. */
					if(0 == path_is_dotname(dp->name, dp->name_len)) {
						match = 1;
					}
				}
				else {
					if ((dp->name_len == strlen(string)) &&
						(0 == strncmp(dp->name, string, dp->name_len))) {
						match = 1;
					}
				}
			}

			if (match) {
				/* LOOK_UP or DELETE found what it wanted. */
				rc = 0;
				if (IS_EMPTY == flag) {
					rc = ENOTEMPTY;
				}
				else if (DELETE == flag) {
					if (dp->name_len >= sizeof(ino_t)) {
						/* Save d_ino for recovery. */
						t = dp->name_len - sizeof(ino_t);
						*((ino_t *) &dp->name[t]) = dp->inode;
					}
					dp->inode = 0; /* erase entry */
					/* Now we have cleared dentry, if it's not the first one,
					 * merge it with previous one.  Since we assume, that
					 * existing dentry must be correct, there is no way to
					 * spann a data block.
					 */
					if (prev_dp) {
						temp = prev_dp->rec_len;
						temp += dp->rec_len;
						prev_dp->rec_len = temp;
					}
				}
				else { /* 'flag' is LOOK_UP */
					*numb = (ino_t) dp->inode;
				}
				if (1 != ext4_write_sector(node, fi->f_buf, 1, b)) {
					return EIO;
				}
				return rc;
			}

			/* Check for free slot for the benefit of ENTER. */
			if (ENTER ==  flag && 0 == dp->inode) {
				/* we found a free slot, check if it has enough space */
				if (required_space <= dp->rec_len) {
					e_hit = 1; /* we found a free slot */
					break;
				}
			}
			/* Can we shrink dentry? */
			if (ENTER == flag && required_space <= EXT4_DIR_ENTRY_SHRINK(dp)) {
				/* Shrink directory and create empty slot, now
				 * dp->d_rec_len = DIR_ENTRY_ACTUAL_SIZE + DIR_ENTRY_SHRINK.
				 */
				new_slot_size = dp->rec_len;
				actual_size = EXT4_DIR_ENTRY_ACTUAL_SIZE(dp);
				new_slot_size -= actual_size;
				dp->rec_len = actual_size;
				dp = EXT4_NEXT_DISC_DIR_DESC(dp);
				dp->rec_len = new_slot_size;
				/* if we fail before writing real ino */
				dp->inode = 0;
				e_hit = 1; /* we found a free slot */
				break;
			}

			prev_dp = dp;
		}

		/* The whole block has been searched or ENTER has a free slot. */
		if (e_hit) {
			break; /* e_hit set if ENTER can be performed now */
		}
		/* otherwise, continue searching dir */
		if (1 != ext4_write_sector(node, fi->f_buf, 1, b)) {
			return EIO;
		}
	}

	/* The whole directory has now been searched. */
	if (ENTER != flag) {
		return (flag == IS_EMPTY ? 0 : ENOENT);
	}

	/* This call is for ENTER.  If no free slot has been found so far, try to
	 * extend directory.
	 */
	if (0 == e_hit) { /* directory is full and no room left in last block */
		new_slots++; /* increase directory size by 1 entry */
		if (0 != (rc = ext4_new_block(node, ext4_file_size(fi->f_di)))) {
			return rc;
		}
		dp = (struct ext4_dir*) &b_data(fi->f_buf);
		dp->rec_len = fsi->s_block_size;
		dp->name_len = EXT4_DIR_ENTRY_MAX_NAME_LEN(dp); /* for failure */
		extended = 1;
	}

	/* 'bp' now points to a directory block with space. 'dp' points to slot. */
	dp->name_len = string_len;
	for (i = 0; i < PATH_MAX
			&& i < dp->name_len && string[i];	i++) {
		dp->name[i] = string[i];
	}
	dp->inode = (int) *numb;
	if (HAS_INCOMPAT_FEATURE(&fsi->e4sb, EXT2F_INCOMPAT_FILETYPE)) {
		dp->type = ext4_type_from_mode_fmt(mode_fmt);
	}

	if (1 != ext4_write_sector(node, fi->f_buf, 1, b)) {
		return EIO;
	}

	if (1 == new_slots) {
		ext4_add_lo_hi(fi->f_di.i_size_lo, fi->f_di.i_size_hi, (uint32_t) dp->rec_len);
		/* Send the change to disk if the directory is extended. */
		if (extended) {
			memcpy(&fdi, &fi->f_di, sizeof(struct ext4_inode));
			ext4_rw_inode(node, &fdi, EXT4_W_INODE);
		}
	}
	return 0;
}

static int ext4_new_node(struct inode *i_new, struct inode *i_dir) {
	/* It allocates a new inode, makes a directory entry for it in
	 * the dir_fi directory with string name, and initializes it.
	 * It returns a pointer to the ext4_file_info if it can do this;
	 * otherwise it returns NULL.
	 */
	int rc;
	struct ext4_file_info *fi;
	struct ext4_inode fdi;
	struct ext4_fs_info *fsi;

	fsi = i_dir->i_sb->sb_data;

	/* Last path component does not exist.  Make new directory entry. */
	if (0 != (rc = ext4_alloc_inode(i_new, i_dir))) {
		/* Can't creat new inode: out of inodes. */
		return rc;
	}

	fi = inode_priv(i_new);

	/* Force inode to the disk before making directory entry to make
	 * the system more robust in the face of a crash: an inode with
	 * no directory entry is much better than the opposite.
	 */
	if (node_is_directory(i_new)) {
		fi->f_di.i_size_lo = fsi->s_block_size;
		if (0 != ext4_new_block(i_new, fsi->s_block_size - 1)) {
			return ENOSPC;
		}
		fi->f_di.i_links_count++; /* directory have 2 links */
	}
	fi->f_di.i_mode = i_new->i_mode;
	fi->f_di.i_links_count++;

	memcpy(&fdi, &fi->f_di, sizeof(struct ext4_inode));
	ext4_rw_inode(i_new, &fdi, EXT4_W_INODE);/* force inode to disk now */

	/* New inode acquired.  Try to make directory entry. */
	if (0 != (rc = ext4_dir_operation(i_dir, inode_name(i_new),
			&fi->f_num, ENTER, i_new->i_mode))) {
		return rc;
	}
	/* The caller has to return the directory ext4_file_info (*dir_fi).  */
	return 0;
}


/* Unlink 'file_name'; rip must be the inode of 'file_name' or NULL. */
static int ext4_unlink_file(struct inode *dir_node, struct inode *node) {
	int rc;


	if ((0 != (rc = ext4_open(node))) || (0 != (rc = ext4_free_inode(node)))) {
		return rc;
	}
	return ext4_dir_operation(dir_node,
			inode_name(node), NULL, DELETE, 0);
}

static int ext4_remove_dir(struct inode *dir_node, struct inode *node) {
	/* A directory file has to be removed. Five conditions have to met:
	* 	- The file must be a directory
	*	- The directory must be empty (except for . and ..)
	*	- The final component of the path must not be . or ..
	*	- The directory must not be the root of a mounted file system (VFS)
	*	- The directory must not be anybody's root/working directory (VFS)
	*/
	int rc;
	char *dir_name;
	struct ext4_file_info *fi;

	fi = inode_priv(node);
	dir_name = inode_name( node);

	/* search_dir checks that rip is a directory too. */
	if (0 != (rc = ext4_dir_operation(node, "", NULL, IS_EMPTY, 0))) {
		return -1;
	}

	if(path_is_dotname(dir_name, strlen(dir_name))) {
		return EINVAL;
	}

	if (fi->f_num == ROOT_INODE) {
		return EBUSY; /* can't remove 'root' */
	}

	/* Unlink . and .. from the dir. */
	if (0 != (rc = (ext4_dir_operation(node, ".", NULL, DELETE, 0) |
				ext4_dir_operation(node, "..", NULL, DELETE, 0)))) {
		return rc;
	}

	/* Actually try to unlink the file; fails if parent is mode 0 etc. */
	if (0 != (rc = ext4_unlink_file(dir_node, node))) {
		return rc;
	}

	return 0;
}

static int ext4_unlink(struct inode *dir_node, struct inode *node) {
	int rc;
	struct ext4_file_info *dir_fi;

	dir_fi = inode_priv(dir_node);

	/* Temporarily open the dir. */
	if (0 != (rc = ext4_open(dir_node))) {
		return rc;
	}

	if (node_is_directory(node)) {
		rc = ext4_remove_dir(dir_node, node); /* call is RMDIR */
	}
	else {
		rc = ext4_unlink_file(dir_node, node);
	}

	if(0 == rc) {
		dir_fi->f_di.i_links_count--;
		ext4_rw_inode(dir_node, &dir_fi->f_di, EXT4_W_INODE);
	}

	ext4_close(dir_node);

	return rc;
}

DECLARE_FILE_SYSTEM_DRIVER(ext4fs_driver);
