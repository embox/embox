/**
 * @file
 * @brief ext file system
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#include <fs/journal.h>
#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/ext2.h>
#include <fs/hlpr_path.h>
#include <fs/mount.h>
#include <fs/file_system.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>

#include <util/array.h>
#include <embox/unit.h>
#include <drivers/block_dev.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>


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

static int ext2_read_inode(struct nas *nas, uint32_t);
static int ext2_block_map(struct nas *nas, int32_t, uint32_t *);
static int ext2_buf_read_file(struct nas *nas, char **, size_t *);
static size_t ext2_write_file(struct nas *nas, char *buf_p, size_t size);
static int ext2_new_block(struct nas *nas, long position);
static int ext2_search_directory(struct nas *nas, const char *, int, uint32_t *);
static int ext2_read_sblock(struct nas *nas);
static int ext2_read_gdblock(struct nas *nas);
static int ext2_mount_entry(struct nas *nas);

/* ext filesystem description pool */
POOL_DEF(ext2_fs_pool, struct ext2_fs_info,
		OPTION_GET(NUMBER,ext2_descriptor_quantity));

/* ext file description pool */
POOL_DEF(ext2_file_pool, struct ext2_file_info,
		OPTION_GET(NUMBER,inode_quantity));

#define FS_NAME "ext2"

/* TODO link counter */

static int ext2fs_open(struct node *node, struct file_desc *file_desc,
		int flags);
static int ext2fs_close(struct file_desc *desc);
static size_t ext2fs_read(struct file_desc *desc, void *buf, size_t size);
static size_t ext2fs_write(struct file_desc *desc, void *buf, size_t size);
static int ext2fs_ioctl(struct file_desc *desc, int request, ...);

static struct kfile_operations ext2_fop = {
	.open = ext2fs_open,
	.close = ext2fs_close,
	.read = ext2fs_read,
	.write = ext2fs_write,
	.ioctl = ext2fs_ioctl,
};

/*
 * help function
 */

void *ext2_buff_alloc(struct nas *nas,
 size_t size) {
	struct ext2_fs_info *fsi;

	fsi = nas->fs->fsi;
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

int ext2_buff_free(struct nas *nas, char *buff) {
	struct ext2_fs_info *fsi;

	fsi = nas->fs->fsi;

	if ((0 != fsi->s_page_count) && (NULL != buff)) {
		phymem_free(buff, fsi->s_page_count);
	}
	return 0;
}

int ext2_read_sector(struct nas *nas, char *buffer, uint32_t count,
		uint32_t sector) {
	struct ext2_fs_info *fsi;
	fsi = nas->fs->fsi;

	if (0 > block_dev_read(nas->fs->bdev, (char *) buffer,
					count * fsi->s_block_size, fsbtodb(fsi, sector))) {
		return -1;
	}
	else {
		return count;
	}
}

int ext2_write_sector(struct nas *nas, char *buffer, uint32_t count,
		uint32_t sector) {
	struct ext2_fs_info *fsi;

	fsi = nas->fs->fsi;

	if (!strcmp(nas->fs->drv->name, "ext3")) {
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
		if (0 > block_dev_write(nas->fs->bdev, (char *) buffer,
						count * fsi->s_block_size, fsbtodb(fsi, sector))) {
			return -1;
		}
	}

	return count;
}

static uint32_t ext2_rd_indir(char *buf, int index) {
	return b_ind(buf) [index];
}

/*
 * Calculate indirect block levels.
 *
 * We note that the number of indirect blocks is always
 * a power of 2.  This lets us use shifts and masks instead
 * of divide and remainder and avoinds pulling in the
 * 64bit division routine into the boot code.
 */
static int ext2_shift_culc(struct ext2_file_info *fi,
								struct ext2_fs_info *fsi) {
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
static int ext2_read_symlink(struct nas *nas, uint32_t parent_inumber,
		const char **cp) {
	/* XXX should handle LARGEFILE */
	int len, link_len;
	int rc;
	uint32_t inumber;
	char namebuf[MAXPATHLEN + 1];
	int nlinks;
	uint32_t disk_block;
	struct ext2_file_info *fi;

	fi = nas->fi->privdata;

	nlinks = 0;
	link_len = fi->f_di.i_size;
	len = strlen(*cp);

	if ((link_len + len > MAXPATHLEN) || (++nlinks > MAXSYMLINKS)) {
		return ENOENT;
	}

	memmove(&namebuf[link_len], cp, len + 1);

	if (link_len < EXT2_MAXSYMLINKLEN) {
		memcpy(namebuf, fi->f_di.i_block, link_len);
	} else {
		/* Read file for symbolic link */
		if (0 != (rc = ext2_block_map(nas, (int32_t) 0, &disk_block))) {
			return rc;
		}
		if (1 != ext2_read_sector(nas, fi->f_buf, 1, disk_block)) {
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
	rc = ext2_read_inode(nas, inumber);

	return rc;
}

/* set node type by file system file type */
static mode_t ext2_type_to_mode_fmt(uint8_t e2d_type) {
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

static uint8_t ext2_type_from_mode_fmt(mode_t mode) {
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

int ext2_close(struct nas *nas) {
	struct ext2_file_info *fi;

	fi = nas->fi->privdata;

	if (NULL != fi) {
		if (NULL != fi->f_buf) {
			ext2_buff_free(nas, fi->f_buf);
		}
	}

	return 0;
}

int ext2_open(struct nas *nas) {
	int rc;
	char path[PATH_MAX];
	const char *cp, *ncp;
	uint32_t inumber, parent_inumber;

	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	/* prepare full path into this filesystem */
	vfs_get_relative_path(nas->node, path, PATH_MAX);

	/* alloc a block sized buffer used for all transfers */
	if (NULL == (fi->f_buf = ext2_buff_alloc(nas, fsi->s_block_size))) {
		return ENOMEM;
	}

	/* read group descriptor blocks */
	if (0 != (rc = ext2_read_gdblock(nas))) {
		return rc;
	}

	if (0 != (rc = ext2_shift_culc(fi, fsi))) {
		return rc;
	}

	inumber = EXT2_ROOTINO;
	if (0 != (rc = ext2_read_inode(nas, inumber))) {
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
		if (0 != (rc = ext2_search_directory(nas, ncp, cp - ncp, &inumber))) {
			goto out;
		}

		/* Open next component */
		if (0 != (rc = ext2_read_inode(nas, inumber))) {
			goto out;
		}

		/* Check for symbolic link */
		if (S_ISLNK(fi->f_di.i_mode)) {
			if (0 != (rc = ext2_read_symlink(nas, parent_inumber, &cp))) {
				goto out;
			}
		}
	}

	fi->f_num = inumber;

	return 0;

	out: ext2_close(nas);
	return rc;
}

/*
 * file_operation
 */
static int ext2fs_open(struct node *node, struct file_desc *desc, int flags) {
	int rc;
	struct nas *nas;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	nas = node->nas;
	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	fi->f_pointer = desc->cursor; /* reset seek pointer */

	if (NULL == (fi->f_buf = ext2_buff_alloc(nas, fsi->s_block_size))) {
		return -ENOMEM;
	}

	if (0 != (rc = ext2_read_inode(nas, fi->f_num))) {
		ext2_close(nas);
		return -rc;
	}
	else {
		nas->fi->ni.size = fi->f_di.i_size;
	}

	return 0;
}

static int ext2fs_close(struct file_desc *desc) {
	struct nas *nas;

	if (NULL == desc) {
		return 0;
	}
	nas = desc->node->nas;

	return ext2_close(nas);
}

static size_t ext2fs_read(struct file_desc *desc, void *buff, size_t size) {
	int rc;
	size_t csize;
	char *buf;
	size_t buf_size;
	char *addr = buff;
	struct nas *nas;
	struct ext2_file_info *fi;

	nas = desc->node->nas;
	fi = nas->fi->privdata;
	fi->f_pointer = desc->cursor;

	while (size != 0) {
		/* XXX should handle LARGEFILE */
		if (fi->f_pointer >= (long) fi->f_di.i_size) {
			break;
		}

		if (0 != (rc = ext2_buf_read_file(nas, &buf, &buf_size))) {
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

	desc->cursor = fi->f_pointer;

	return (addr - (char *) buff);
}

static size_t ext2fs_write(struct file_desc *desc, void *buff, size_t size) {
	uint32_t bytecount;
	struct nas *nas;
	struct ext2_file_info *fi;

	nas = desc->node->nas;
	fi = nas->fi->privdata;
	fi->f_pointer = desc->cursor;

	bytecount = ext2_write_file(nas, buff, size);

	desc->cursor = fi->f_pointer;
	nas->fi->ni.size = fi->f_di.i_size;

	return bytecount;
}

static int ext2fs_ioctl(struct file_desc *desc, int request, ...) {
	return 0;
}

static int ext2_create(struct nas *nas, struct nas * parents_nas);
static int ext2_mkdir(struct nas *nas, struct nas * parents_nas);
static int ext2_unlink(struct nas *dir_nas, struct nas *nas);
static void ext2_free_fs(struct nas *nas);
static int ext2_umount_entry(struct nas *nas);

static int ext2fs_init(void * par);
static int ext2fs_format(void *path);
static int ext2fs_mount(void *dev, void *dir);
static int ext2fs_create(struct node *parent_node, struct node *node);
static int ext2fs_delete(struct node *node);
static int ext2fs_truncate(struct node *node, off_t length);
static int ext2fs_umount(void *dir);


static struct fsop_desc ext2_fsop = {
	.init	     = ext2fs_init,
	.format	     = ext2fs_format,
	.mount	     = ext2fs_mount,
	.create_node = ext2fs_create,
	.delete_node = ext2fs_delete,

	.getxattr    = ext2fs_getxattr,
	.setxattr    = ext2fs_setxattr,
	.listxattr   = ext2fs_listxattr,

	.truncate    = ext2fs_truncate,
	.umount      = ext2fs_umount,
};

static int ext2fs_init(void * par) {
	return 0;
};

static struct fs_driver ext2fs_driver = {
	.name = FS_NAME,
	.file_op = &ext2_fop,
	.fsop = &ext2_fsop,
};

static ext2_file_info_t *ext2_fi_alloc(struct nas *nas, void *fs) {
	ext2_file_info_t *fi;

	fi = pool_alloc(&ext2_file_pool);
	if (fi) {
		nas->fi->ni.size = fi->f_pointer = 0;
		nas->fi->privdata = fi;
		nas->fs = fs;
	}

	return fi;
}

static int ext2fs_create(struct node *parent_node, struct node *node) {
	int rc;
	struct nas *nas, *parents_nas;

	nas = node->nas;
	parents_nas = parent_node->nas;

	if (node_is_directory(node)) {
		if (0 != (rc = ext2_mkdir(nas, parents_nas))) {
			return -rc;
		}
		if (0 != (rc = ext2_mount_entry(nas))) {
			return -rc;
		}
	} else {
		if (0 != (rc = ext2_create(nas, parents_nas))) {
			return -rc;
		}
	}
	return 0;
}

static int ext2fs_delete(struct node *node) {
	int rc;
	node_t *parents;

	if (NULL == (parents = vfs_subtree_get_parent(node))) {
		rc = ENOENT;
		return -rc;
	}

	if (0 != (rc = ext2_unlink(parents->nas, node->nas))) {
		return -rc;
	}

	vfs_del_leaf(node);

	return 0;
}

static void ext2_dflt_sb(struct ext2sb *sb,
							size_t dev_size, float dev_factor) {
	int i;

	sb->s_first_data_block = 1;  /* First Data Block */
	sb->s_blocks_per_group = 8192;  /* # Blocks per group */
	sb->s_frags_per_group = 8192;   /* # Fragments per group */
	sb->s_wtime = 1363591930;             /* Write time */
	sb->s_mnt_count = 2;            /* Mount count */
	sb->s_max_mnt_count = 65535;        /* Maximal mount count */
	sb->s_magic = 61267;                /* Magic signature */
	sb->s_state = 1;                /* File system state */
	sb->s_errors = 1;               /* Behaviour when detecting errors */
	sb->s_lastcheck = 1363591830;         /* time of last check */
	sb->s_rev_level = 1;         /* Revision level */
	sb->s_first_ino = 11;         /* First non-reserved inode */
	sb->s_inode_size = 128;           /* size of inode structure */
	sb->s_block_group_nr = 0;       /* block group # of this superblock */
	sb->s_feature_compat = 56;    /* compatible feature set */
	sb->s_feature_incompat = 2;  /* incompatible feature set */
	sb->s_feature_ro_compat = 1; /* readonly-compatible feature set */
	sb->s_uuid[0] = 153;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[1] = 36;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[2] = 151;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[3] = 255;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[4] = 11;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[5] = 115;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[6] = 66;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[7] = 126;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[8] = 147;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[0] = 28;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[0] = 214;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[0] = 168;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[0] = 199;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[0] = 53;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[0] = 165;             /* 128-bit uuid for volume  16*/
	sb->s_uuid[0] = 3;             /* 128-bit uuid for volume  16*/
	strcpy((char *) sb->s_volume_name, "ext2");      /* volume name  16 */

	sb->s_blocks_count = dev_size / dev_factor;
	sb->s_inodes_count = sb->s_blocks_count / 4;      /* Inodes count */
	sb->s_r_blocks_count = sb->s_blocks_count / 20;    /* Reserved blocks count */
	sb->s_inodes_per_group =  /* # Inodes per group */
		(sb->s_inodes_count > (sb->s_blocks_per_group / 4) ?
				1900 : sb->s_inodes_count);
	sb->s_free_inodes_count = sb->s_inodes_count - 10; /* Free inodes count */
	sb->s_padding1 = sb->s_inodes_count / 65;
	i = sb->s_padding1 + 5 + (sb->s_inodes_count * sb->s_inode_size) / SBSIZE;
	i += 2;/* blocks for root*/
	sb->s_free_blocks_count = sb->s_blocks_count - i; /* Free blocks count */
}

static void ext2_dflt_gd(struct ext2sb *sb, struct ext2_gd *gd) {

	gd->block_bitmap = sb->s_padding1 + 3; /* Blocks bitmap block */
	gd->inode_bitmap = gd->block_bitmap + 1;     /* Inodes bitmap block */
	gd->inode_table = gd->inode_bitmap + 1; /* Inodes table block */
	gd->free_blocks_count = sb->s_free_blocks_count;   /* Free blocks count */
	gd->free_inodes_count = sb->s_free_inodes_count;   /* Free inodes count */
	gd->used_dirs_count = 1;     /* Directories count */
	gd->pad = 0;
}

static void ext2_dflt_root_inode(struct ext2fs_dinode *di) {

	di->i_mode = 040755;
	di->i_uid = di->i_gid = 1000;
	di->i_size = 1024;
	di->i_atime = di->i_ctime = di->i_mtime = 1683851637;
	di->i_dtime = 0;
	di->i_links_count = 2;
	di->i_blocks = 2;
}

static void ext2_dflt_root_entry(char *point) {
	struct	ext2fs_direct *dir;

	dir = (struct ext2fs_direct *) point;
	dir->e2d_ino = 2;
	dir->e2d_reclen = 12;
	dir->e2d_namlen = 1;
	dir->e2d_type = 2;
	strcpy(dir->e2d_name, ".");

	dir = (struct ext2fs_direct *) (point + dir->e2d_reclen);
	dir->e2d_ino = 2;
	dir->e2d_reclen = 1012;
	dir->e2d_namlen = 2;
	dir->e2d_type = 2;
	strcpy(dir->e2d_name, "..");
}

static int ext2_mark_bitmap(void *bdev, struct ext2sb *sb,
							struct ext2_gd *gd, char *buff, float dev_factor) {
	char *point;
	int i, last, mask;
	int sector;

	i = sb->s_blocks_count - sb->s_free_blocks_count;

	sector = gd->block_bitmap * dev_factor;
	memset(buff, 0xFF, SBSIZE);
	point = buff;

	mask = i % 8;
	i /= 8;
	point += i;
	last = gd->free_blocks_count / 8 + i;
	if(mask) {
		*point++ = 0xff >> (8 - mask);
		last--;
	}
	for( ; i < last && i < SBSIZE; i++) {
		*point++ = 0;
	}
	if(i < SBSIZE) {
		mask = gd->free_blocks_count % 8;
		*point = 0xff << (8 - mask);
	}
	if (0 > block_dev_write(bdev, buff, SBSIZE, sector)) {
		return -1;
	}

	sector = gd->inode_bitmap * dev_factor;
	memset(buff, 0xFF, SBSIZE);
	/* preset special inodes */
	*(buff + 1) = 0x03;

	point = buff + 2;
	i = 0;
	last = gd->free_inodes_count / 8;
	for( ; i < last && i < SBSIZE; i++) {
		*point++ = 0;
	}
	if(i < SBSIZE) {
		/* preset special inodes contains 6 preset tables*/
		mask = (gd->free_inodes_count - 6) % 8;
		*point = 0xff << mask;
	}

	if (0 > block_dev_write(bdev, buff, SBSIZE, sector)) {
		return -1;
	}

	return 0;
}


static int ext2fs_format(void *dev) {
	int rc;
	struct node *dev_node;
	struct nas *dev_nas;
	struct node_fi *dev_fi;
	struct block_dev *bdev;
	struct ext2sb sb;
	struct ext2_gd gd;
	struct ext2fs_dinode *di;
	char buff[SBSIZE];
	size_t dev_size, dev_bsize;
	int sector;
	float dev_factor;

	dev_node = dev;
	dev_nas = dev_node->nas;

	if (NULL == (dev_fi = dev_nas->fi)) {
		rc = ENODEV;
		return -rc;
	}

	memset(&sb, 0, sizeof(struct ext2sb));
	memset(&gd, 0, sizeof(struct ext2_gd));

	bdev = (struct block_dev *) dev_fi->privdata;
	dev_size = block_dev_ioctl(bdev, IOCTL_GETDEVSIZE, NULL, 0);
	dev_bsize = block_dev_ioctl(bdev, IOCTL_GETBLKSIZE, NULL, 0);
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

static int ext2fs_mount(void *dev, void *dir) {
	int rc;
	struct node *dir_node, *dev_node;
	struct nas *dir_nas, *dev_nas;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;
	struct node_fi *dev_fi;

	dev_node = dev;
	dev_nas = dev_node->nas;
	dir_node = dir;
	dir_nas = dir_node->nas;

	if (NULL == (dev_fi = dev_nas->fi)) {
		rc = ENODEV;
		return -rc;
	}

	if (NULL == (dir_nas->fs = filesystem_create(FS_NAME))) {
		rc = ENOMEM;
		goto error;
	}

	dir_nas->fs->bdev = dev_fi->privdata;

	/* allocate this fs info */
	if (NULL == (fsi = pool_alloc(&ext2_fs_pool))) {
		dir_nas->fs->fsi = fsi;
		rc = ENOMEM;
		goto error;
	}
	memset(fsi, 0, sizeof(struct ext2_fs_info));
	dir_nas->fs->fsi = fsi;

	if (NULL == (fi = pool_alloc(&ext2_file_pool))) {
		dir_nas->fi->privdata = (void *) fi;
		rc = ENOMEM;
		goto error;
	}
	memset(fi, 0, sizeof(struct ext2_file_info));
	fi->f_pointer = 0;
	dir_nas->fi->privdata = (void *) fi;

	/* presetting that we think */
	fsi->s_block_size = SBSIZE;
	fsi->s_sectors_in_block = fsi->s_block_size / 512;
	if (0 != (rc = ext2_read_sblock(dir_nas))) {
		goto error;
	}
	if (NULL == (fsi->e2fs_gd = ext2_buff_alloc(dir_nas,
			sizeof(struct ext2_gd) * fsi->s_ncg))) {
		rc = ENOMEM;
		goto error;
	}
	if (0 != (rc = ext2_read_gdblock(dir_nas))) {
		goto error;
	}

	if (0 != (rc = ext2_mount_entry(dir_nas))) {
		goto error;
	}
	return 0;

	error:
	ext2_free_fs(dir_nas);

	return -rc;
}

static int ext2fs_truncate (struct node *node, off_t length) {
	struct nas *nas = node->nas;

	nas->fi->ni.size = length;

	return 0;
}

static int ext2fs_umount(void *dir) {
	struct node *dir_node;
	struct nas *dir_nas;

	dir_node = dir;
	dir_nas = dir_node->nas;

	/* delete all entry node */
	ext2_umount_entry(dir_nas);

	/* free ext2 file system pools and buffers*/
	ext2_free_fs(dir_nas);

	return 0;
}

static void ext2_free_fs(struct nas *nas) {
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	if(NULL != nas->fs) {
		fsi = nas->fs->fsi;

		if(NULL != fsi) {
			if(NULL != fsi->e2fs_gd) {
				ext2_buff_free(nas, (char *) fsi->e2fs_gd);
			}
			pool_free(&ext2_fs_pool, fsi);
		}
		filesystem_free(nas->fs);
	}

	if(NULL != (fi = nas->fi->privdata)) {
		pool_free(&ext2_file_pool, fi);
	}
}


static int ext2_umount_entry(struct nas *nas) {
	struct node *child;

	if (node_is_directory(nas->node)) {
		while (NULL != (child = vfs_subtree_get_child_next(nas->node, NULL))) {
			if (node_is_directory(child)) {
				ext2_umount_entry(child->nas);
			}

			pool_free(&ext2_file_pool, child->nas->fi->privdata);
			vfs_del_leaf(child);
		}
	}

	return 0;
}

extern void e2fs_i_bswap(struct ext2fs_dinode *old, struct ext2fs_dinode *new);

/*
 * Read a new inode into a file structure.
 */
static int ext2_read_inode(struct nas *nas, uint32_t inumber) {
	char *buf;
	size_t rsize;
	int64_t inode_sector;
	struct ext2fs_dinode *dip;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	inode_sector = ino_to_fsba(fsi, inumber);

	/* Read inode and save it */
	buf = fi->f_buf;
	rsize = ext2_read_sector(nas, buf, 1, inode_sector);
	if (rsize * fsi->s_block_size != fsi->s_block_size) {
		return EIO;
	}

	/* set pointer to inode struct in read buffer */
	dip = (struct ext2fs_dinode *) (buf
			+ EXT2_DINODE_SIZE(fsi) * ino_to_fsbo(fsi, inumber));
	e2fs_i_bswap(dip, dip);
	/* load inode struct to file info */
	e2fs_iload(dip, &fi->f_di);

	/* Clear out the old buffers */
	fi->f_ind_cache_block = ~0;
	fi->f_buf_blkno = -1;
	return 0;
}

/*
 * Given an offset in a file, find the disk block number that
 * contains that block.
 */
static int ext2_block_map(struct nas *nas, int32_t file_block,
		uint32_t *disk_block_p) {
	uint level;
	int32_t ind_cache;
	int32_t ind_block_num;
	size_t rsize;
	int32_t *buf;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	buf = (void *) fi->f_buf;

	/*
	 * Index structure of an inode:
	 *
	 * e2di_blocks[0..NDADDR-1]
	 *			hold block numbers for blocks
	 *			0..NDADDR-1
	 *
	 * e2di_blocks[NDADDR+0]
	 *			block NDADDR+0 is the single indirect block
	 *			holds block numbers for blocks
	 *			NDADDR .. NDADDR + NINDIR(fsi)-1
	 *
	 * e2di_blocks[NDADDR+1]
	 *			block NDADDR+1 is the double indirect block
	 *			holds block numbers for INDEX blocks for blocks
	 *			NDADDR + NINDIR(fsi) ..
	 *			NDADDR + NINDIR(fsi) + NINDIR(fsi)**2 - 1
	 *
	 * e2di_blocks[NDADDR+2]
	 *			block NDADDR+2 is the triple indirect block
	 *			holds block numbers for	double-indirect
	 *			blocks for blocks
	 *			NDADDR + NINDIR(fsi) + NINDIR(fsi)**2 ..
	 *			NDADDR + NINDIR(fsi) + NINDIR(fsi)**2
	 *				+ NINDIR(fsi)**3 - 1
	 */

	if (file_block < NDADDR) {
		/* Direct block. */
		*disk_block_p = fs2h32(fi->f_di.i_block[file_block]);
		return 0;
	}

	file_block -= NDADDR;

	ind_cache = file_block >> LN2_IND_CACHE_SZ;
	if (ind_cache == fi->f_ind_cache_block) {
		*disk_block_p = fs2h32(fi->f_ind_cache[file_block & IND_CACHE_MASK]);
		return 0;
	}

	for (level = 0;;) {
		level += fi->f_nishift;
		if (file_block < (int32_t) 1 << level)
			break;
		if (level > NIADDR * fi->f_nishift) {
			/* Block number too high */
			return EFBIG;
		}
		file_block -= (int32_t) 1 << level;
	}

	ind_block_num =
			fs2h32(fi->f_di.i_block[NDADDR + (level / fi->f_nishift - 1)]);

	for (;;) {
		level -= fi->f_nishift;
		if (ind_block_num == 0) {
			*disk_block_p = 0; /* missing */
			return 0;
		}

		/*
		 * If we were feeling brave, we could work out the number
		 * of the disk sector and read a single disk sector instead
		 * of a filesystem block.
		 * However we don't do this very often anyway...
		 */
		rsize = ext2_read_sector(nas, (char *) buf, 1, ind_block_num);

		if (rsize * fsi->s_block_size != fsi->s_block_size) {
			return EIO;
		}
		ind_block_num = fs2h32(buf[file_block >> level]);
		if (0 == level) {
			break;
		}
		file_block &= (1 << level) - 1;
	}

	/* Save the part of the block that contains this sector */
	memcpy(fi->f_ind_cache, &buf[file_block & ~IND_CACHE_MASK],
			IND_CACHE_SZ * sizeof fi->f_ind_cache[0]);
	fi->f_ind_cache_block = ind_cache;

	*disk_block_p = ind_block_num;
	return 0;
}

/*
 * Read a portion of a file into an internal buffer.
 * Return the location in the buffer and the amount in the buffer.
 */
static int ext2_buf_read_file(struct nas *nas, char **buf_p, size_t *size_p) {
	int rc;
	long off;
	int32_t file_block;
	uint32_t disk_block;
	size_t block_size;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	off = blkoff(fsi, fi->f_pointer);
	file_block = lblkno(fsi, fi->f_pointer);
	block_size = fsi->s_block_size; /* no fragment */

	if (file_block != fi->f_buf_blkno) {
		if (0 != (rc = ext2_block_map(nas, file_block, &disk_block))) {
			return rc;
		}

		if (disk_block == 0) {
			memset(fi->f_buf, 0, block_size);
			fi->f_buf_size = block_size;
		}
		else {
			if (1 != ext2_read_sector(nas, fi->f_buf, 1, disk_block)) {
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
	if (*size_p > fi->f_di.i_size - fi->f_pointer) {
		*size_p = fi->f_di.i_size - fi->f_pointer;
	}

	return 0;
}
/*
 * Write a portion to a file from an internal buffer.
 */
static size_t ext2_write_file(struct nas *nas, char *buf, size_t size) {
	long inblock_off;
	int32_t file_block;
	uint32_t disk_block;
	char *buff;
	size_t block_size, len, cnt;
	size_t bytecount, end_pointer;
	struct ext2fs_dinode fdi;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	block_size = fsi->s_block_size; /* no fragment */
	bytecount = 0;
	len = size;
	buff = buf;
	end_pointer = fi->f_pointer + len;

	if (0 != ext2_new_block(nas, end_pointer - 1)) {
		return 0;
	}

	while (1) {
		file_block = lblkno(fsi, fi->f_pointer);

		if (0 != ext2_block_map(nas, file_block, &disk_block)) {
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
		if (1 != ext2_read_sector(nas, fi->f_buf, 1, disk_block)) {
			bytecount = 0;
			break;
		}
		/* set new data in block */
		memcpy(fi->f_buf + inblock_off, buff, cnt);

		/* write one block to device */
		if (1 != ext2_write_sector(nas, fi->f_buf, 1, disk_block)) {
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
	if (fi->f_di.i_size < fi->f_pointer) {
		fi->f_di.i_size = fi->f_pointer;
	}
	memcpy(&fdi, &fi->f_di, sizeof(struct ext2fs_dinode));
	ext2_rw_inode(nas, &fdi, EXT2_W_INODE);

	return bytecount;
}

/*
 * Search a directory for a name and return its
 * inode number.
 */
static int ext2_search_directory(struct nas *nas, const char *name, int length,
		uint32_t *inumber_p) {
	int rc;
	struct ext2fs_direct *dp;
	struct ext2fs_direct *edp;
	char *buf;
	size_t buf_size;
	int namlen;
	struct ext2_file_info *fi;

	fi = nas->fi->privdata;
	fi->f_pointer = 0;
	/* XXX should handle LARGEFILE */
	while (fi->f_pointer < (long) fi->f_di.i_size) {
		if (0 != (rc = ext2_buf_read_file(nas, &buf, &buf_size))) {
			return rc;
		}

		dp = (struct ext2fs_direct *) buf;
		edp = (struct ext2fs_direct *) (buf + buf_size);
		for (; dp < edp;
				dp = (struct ext2fs_direct *) ((char *) dp
						+ fs2h16(dp->e2d_reclen))) {
			if (fs2h16(dp->e2d_reclen) <= 0) {
				break;
			}
			if (fs2h32(dp->e2d_ino) == (uint32_t) 0) {
				continue;
			}
			namlen = dp->e2d_namlen;
			if (namlen == length && !memcmp(name, dp->e2d_name, length)) {
				/* found entry */
				*inumber_p = fs2h32(dp->e2d_ino);
				return 0;
			}
		}
		fi->f_pointer += buf_size;
	}

	return ENOENT;
}

int ext2_write_sblock(struct nas *nas) {
	struct ext2_fs_info *fsi;

	fsi = nas->fs->fsi;

	if (1 != ext2_write_sector(nas, (char *) &fsi->e2sb, 1,
					dbtofsb(fsi, SBOFF / SECTOR_SIZE))) {
		return EIO;
	}
	return 0;
}

static int ext2_read_sblock(struct nas *nas) {
	void *sbbuf = NULL;
	struct ext2_fs_info *fsi;
	struct ext2sb *ext2sb;
	int ret = 0;

	fsi = nas->fs->fsi;
	ext2sb = &fsi->e2sb;

	if (!(sbbuf = ext2_buff_alloc(nas, fsi->s_block_size))) {
		return ENOMEM;
	}

	if (1 != ext2_read_sector(nas, (char *) sbbuf, 1,
					dbtofsb(fsi, SBOFF / SECTOR_SIZE))) {
		ret = EIO;
		goto out;
	}

	e2fs_sbload(sbbuf, ext2sb);
	ext2_buff_free(nas, sbbuf);

	if (ext2sb->s_magic != E2FS_MAGIC) {
		ret = EINVAL;
		goto out;
	}
	if (ext2sb->s_rev_level > E2FS_REV1
		|| (ext2sb->s_rev_level == E2FS_REV1
		&& (ext2sb->s_first_ino != EXT2_FIRSTINO
		|| (ext2sb->s_inode_size != 128
		&& ext2sb->s_inode_size != 256)
		|| ext2sb->s_feature_incompat & ~EXT2F_INCOMPAT_SUPP))) {
		ret = ENODEV;
		goto out;

	}

	/* compute in-memory ext2_fs_info values */
	fsi->s_ncg =
			howmany(fsi->e2sb.s_blocks_count - fsi->e2sb.s_first_data_block,
					fsi->e2sb.s_blocks_per_group);
	/* XXX assume hw bsize = 512 */
	fsi->s_fsbtodb = fsi->e2sb.s_log_block_size + 1;
	fsi->s_block_size = MINBSIZE << fsi->e2sb.s_log_block_size;
	fsi->s_bshift = LOG_MINBSIZE + fsi->e2sb.s_log_block_size;
	fsi->s_qbmask = fsi->s_block_size - 1;
	fsi->s_bmask = ~fsi->s_qbmask;
	fsi->s_gdb_count =
			howmany(fsi->s_ncg, fsi->s_block_size / sizeof(struct ext2_gd));
	fsi->s_inodes_per_block = fsi->s_block_size / ext2sb->s_inode_size;
	fsi->s_itb_per_group = fsi->e2sb.s_inodes_per_group
			/ fsi->s_inodes_per_block;

	fsi->s_groups_count = ((fsi->e2sb.s_blocks_count
			- fsi->e2sb.s_first_data_block - 1) / fsi->e2sb.s_blocks_per_group)
			+ 1;
	fsi->s_bsearch = fsi->e2sb.s_first_data_block + 1 + fsi->s_gdb_count + 2
			+ fsi->s_itb_per_group;

	fsi->s_blocksize_bits = fsi->e2sb.s_log_block_size + 10;

	fsi->s_desc_per_block = fsi->s_block_size / sizeof(struct ext2_gd);

out:
	ext2_buff_free(nas, sbbuf);
	return ret;
}

int ext2_write_gdblock(struct nas *nas) {
	uint gdpb;
	int i;
	char *buff;
	struct ext2_fs_info *fsi;

	fsi = nas->fs->fsi;

	gdpb = fsi->s_block_size / sizeof(struct ext2_gd);

	for (i = 0; i < fsi->s_gdb_count; i += gdpb) {
		buff = (char *) &fsi->e2fs_gd[i * gdpb];

		if (1 != ext2_write_sector(nas, buff, 1,
						fsi->e2sb.s_first_data_block + 1 + i / gdpb)) {
			return EIO;
		}
	}

	return 0;
}

static int ext2_read_gdblock(struct nas *nas) {
	size_t rsize;
	uint gdpb;
	int i;
	struct ext2_fs_info *fsi;
	void *gdbuf = NULL;
	int ret = 0;

	fsi = nas->fs->fsi;

	gdpb = fsi->s_block_size / sizeof(struct ext2_gd);

	if (!(gdbuf = ext2_buff_alloc(nas, fsi->s_block_size))) {
		return ENOMEM;
	}

	for (i = 0; i < fsi->s_gdb_count; i++) {
		rsize = ext2_read_sector(nas, gdbuf, 1,
				fsi->e2sb.s_first_data_block + 1 + i);
		if (rsize * fsi->s_block_size != fsi->s_block_size) {
			ret = EIO;
			goto out;

		}
		e2fs_cgload((struct ext2_gd *)gdbuf, &fsi->e2fs_gd[i * gdpb],
				(i == (fsi->s_gdb_count - 1)) ?
				(fsi->s_ncg - gdpb * i) * sizeof(struct ext2_gd)
				: fsi->s_block_size);
	}
out:
	ext2_buff_free(nas, gdbuf);
	return ret;
}

static int ext2_mount_entry(struct nas *dir_nas) {
	int rc;
	char *buf;
	size_t buf_size;
	struct ext2fs_direct *dp, *edp;
	struct ext2_file_info *dir_fi, *fi;
	struct ext2_fs_info *fsi;
	char *name, *name_buff;
	node_t *node;
	mode_t mode;

	rc = 0;

	if (NULL == (name_buff = ext2_buff_alloc(dir_nas, NAME_MAX))) {
		rc = ENOMEM;
		return rc;
	}

	if (0 != ext2_open(dir_nas)) {
		goto out;
	}

	dir_fi = dir_nas->fi->privdata;
	fsi = dir_nas->fs->fsi;

	dir_nas->node->mode = dir_fi->f_di.i_mode;
	dir_nas->node->uid = dir_fi->f_di.i_uid;
	dir_nas->node->gid = dir_fi->f_di.i_gid;

	dir_fi->f_pointer = 0;
	while (dir_fi->f_pointer < (long) dir_fi->f_di.i_size) {
		if (0 != (rc = ext2_buf_read_file(dir_nas, &buf, &buf_size))) {
			goto out;
		}
		if (buf_size != fsi->s_block_size || buf_size == 0) {
			rc = EIO;
			goto out;
		}

		dp = (struct ext2fs_direct *) buf;
		edp = (struct ext2fs_direct *) (buf + buf_size);

		for (; dp < edp; dp = (void *) ((char *) dp +
								fs2h16(dp->e2d_reclen))) {
			if (fs2h16(dp->e2d_reclen) <= 0) {
				goto out;
			}
			if (fs2h32(dp->e2d_ino) == 0) {
				continue;
			}

			/* set null determine name */
			name = (char *) &dp->e2d_name;

			memcpy(name_buff, name, fs2h16(dp->e2d_namlen));
			name_buff[fs2h16(dp->e2d_namlen)] = '\0';

			if(0 != path_is_dotname(name_buff, dp->e2d_namlen)) {
				/* dont need create dot or dotdot node */
				continue;
			}

			mode = ext2_type_to_mode_fmt(dp->e2d_type);

			node = vfs_subtree_create(dir_nas->node, name_buff, mode);
			if (!node) {
				rc = ENOMEM;
				goto out;
			}

			fi = ext2_fi_alloc(node->nas, dir_nas->fs);
			if (!fi) {
				vfs_del_leaf(node);
				rc = ENOMEM;
				goto out;
			}

			if (node_is_directory(node)) {
				rc = ext2_mount_entry(node->nas);
			} else {
				/* read inode into fi->f_di*/
				if (0 == ext2_open(node->nas)) {
					/* Load permisiions and credentials. */
					assert((node->mode & S_IFMT) == (fi->f_di.i_mode & S_IFMT));
					node->mode = fi->f_di.i_mode;
					node->uid = fi->f_di.i_uid;
					node->gid = fi->f_di.i_gid;
				}
				ext2_close(node->nas);
			}
		}
		dir_fi->f_pointer += buf_size;
	}

	out: ext2_close(dir_nas);
	ext2_buff_free(dir_nas, name_buff);
	return rc;
}

static int ext2_dir_operation(struct nas *nas, char *string, ino_t *numb,
		int flag, mode_t mode_fmt);
static void ext2_wr_indir(char *buf, int index, uint32_t block);
static int ext2_empty_indir(char *buf, struct ext2_fs_info *fsi);
static void ext2_zero_block(char *buf);

int ext2_write_map(struct nas *nas, long position,
							uint32_t new_block, int op) {
	/* Write a new block into an inode.
	 *
	 * If op includes WMAP_FREE, free the block corresponding to that position
	 * in the inode ('new_block' is ignored then). Also free the indirect block
	 * if that was the last entry in the indirect block.
	 * Also free the double/triple indirect block if that was the last entry in
	 * the double/triple indirect block.
	 * It's the only function which should take care about fi->i_blocks counter.
	 */
	int rc;
	int index1, index2, index3; /* indexes in single..triple indirect blocks */
	long excess, block_pos;
	char new_ind, new_dbl, new_triple;
	int single, triple;
	uint32_t old_block, b1, b2, b3;
	char *bp, *bp_dindir, *bp_tindir;
	static char first_time = 1;
	static long addr_in_block, addr_in_block2;
	static long doub_ind_s, triple_ind_s;
	static long out_range_s;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	index2 = index3 = 0;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	old_block = b1 = b2 = b3 = NO_BLOCK;
	single = triple = 0;
	new_ind = new_dbl = new_triple = 0;
	bp = bp_dindir = bp_tindir = NULL;

	if (first_time) {
		addr_in_block = fsi->s_block_size / BLOCK_ADDRESS_BYTES;
		addr_in_block2 = addr_in_block * addr_in_block;
		doub_ind_s = EXT2_NDIR_BLOCKS + addr_in_block;
		triple_ind_s = doub_ind_s + addr_in_block2;
		out_range_s = triple_ind_s + addr_in_block2 * addr_in_block;
		first_time = 0;
	}

	if (out_range_s <= (block_pos = position / fsi->s_block_size)) {
		/* relative blk # in file */
		return EFBIG;
	}
	/* Is 'position' to be found in the inode itself? */
	if (block_pos < EXT2_NDIR_BLOCKS) {
		if (NO_BLOCK != fi->f_di.i_block[block_pos] && (op & WMAP_FREE)) {
			ext2_free_block(nas, fi->f_di.i_block[block_pos]);
			fi->f_di.i_block[block_pos] = NO_BLOCK;
			fi->f_di.i_blocks -= fsi->s_sectors_in_block;
		}
		else {
			fi->f_di.i_block[block_pos] = new_block;
			fi->f_di.i_blocks += fsi->s_sectors_in_block;
		}
		return 0;
	}

	rc = 0;
	bp = ext2_buff_alloc(nas, 0);
	bp_dindir = ext2_buff_alloc(nas, 0);
	bp_tindir = ext2_buff_alloc(nas, 0);
	if ((NULL == bp) || (NULL == bp) || (NULL == bp)) {
		rc = ENOMEM;
		goto out;
	}

	/* It is not in the inode, so it must be single, double or triple indirect */
	if (block_pos < doub_ind_s) {
		b1 = fi->f_di.i_block[EXT2_NDIR_BLOCKS]; /* addr of single indirect block */
		index1 = block_pos - EXT2_NDIR_BLOCKS;
		single = 1;
	}
	else {
		/* double or triple indirect block. At first if it's triple,
		 * find double indirect block.
		 */
		excess = block_pos - doub_ind_s;
		b2 = fi->f_di.i_block[EXT2_DIND_BLOCK];
		if (block_pos >= triple_ind_s) {
			b3 = fi->f_di.i_block[EXT2_TIND_BLOCK];
			if (NO_BLOCK == b3 && !(op & WMAP_FREE)) {
				/* Create triple indirect block. */
				if (NO_BLOCK == (b3 = ext2_alloc_block(nas, fi->f_bsearch))) {
					rc = ENOSPC;
					goto out;
				}
				fi->f_di.i_block[EXT2_TIND_BLOCK] = b3;
				fi->f_di.i_blocks += fsi->s_sectors_in_block;
				new_triple = 1;
			}
			/* 'b3' is block number for triple indirect block, either old
			 * or newly created.
			 * If there wasn't one and WMAP_FREE is set, 'b3' is NO_BLOCK.
			 */
			if (NO_BLOCK == b3 && (op & WMAP_FREE)) {
				/* WMAP_FREE and no triple indirect block - then no
				 * double and single indirect blocks either.
				 */
				b1 = b2 = NO_BLOCK;
			}
			else {
				if (1 != ext2_read_sector(nas, (char *) bp_tindir, 1, b3)) {
					rc = EIO;
					goto out;
				}
				if (new_triple) {
					ext2_zero_block(bp_tindir);
				}
				excess = block_pos - triple_ind_s;
				index3 = excess / addr_in_block2;
				b2 = ext2_rd_indir(bp_tindir, index3);
				excess = excess % addr_in_block2;
			}
			triple = 1;
		}

		if (NO_BLOCK == b2 && !(op & WMAP_FREE)) {
			/* Create the double indirect block. */
			if (NO_BLOCK == (b2 = ext2_alloc_block(nas, fi->f_bsearch))) {
				rc = ENOSPC;
				goto out;
			}
			if (triple) {
				ext2_wr_indir(bp_tindir, index3, b2); /* update triple indir */
				if (1 != ext2_write_sector(nas, (char *) bp_dindir, 1, b3)) {
					rc = EIO;
					goto out;
				}
			}
			else {
				fi->f_di.i_block[EXT2_DIND_BLOCK] = b2;
			}
			fi->f_di.i_blocks += fsi->s_sectors_in_block;
			new_dbl = 1; /* set flag for later */
		}

		/* 'b2' is block number for double indirect block, either old
		 * or newly created.
		 * If there wasn't one and WMAP_FREE is set, 'b2' is NO_BLOCK.
		 */
		if (NO_BLOCK == b2 && (op & WMAP_FREE)) {
			/* WMAP_FREE and no double indirect block - then no
			 * single indirect block either.
			 */
			b1 = NO_BLOCK;
		}
		else {
			if (1 != ext2_read_sector(nas, (char *) bp_dindir, 1, b2)) {
				rc = EIO;
				goto out;
			}
			if (new_dbl) {
				ext2_zero_block(bp_dindir);
			}
			index2 = excess / addr_in_block;
			b1 = ext2_rd_indir(bp_dindir, index2);
			index1 = excess % addr_in_block;
		}
		single = 0;
	}

	/* b1 is now single indirect block or NO_BLOCK; 'index' is index.
	 * We have to create the indirect block if it's NO_BLOCK. Unless
	 * we're freing (WMAP_FREE).
	 */
	if (NO_BLOCK == b1 && !(op & WMAP_FREE)) {
		if (NO_BLOCK == (b1 = ext2_alloc_block(nas, fi->f_bsearch))) {
			/*failed to allocate dblock*/
			rc = ENOSPC;
			goto out;
		}
		if (single) {
			fi->f_di.i_block[EXT2_NDIR_BLOCKS] = b1; /* update inode single indirect */
		}
		else {
			ext2_wr_indir(bp_dindir, index2, b1); /* update dbl indir */
			if (1 != ext2_write_sector(nas, (char *) bp_dindir, 1, b2)) {
				rc = EIO;
				goto out;
			}
		}
		fi->f_di.i_blocks += fsi->s_sectors_in_block;
		new_ind = 1;
	}

	/* b1 is indirect block's number (unless it's NO_BLOCK when we're
	 * freeing).
	 */
	if (NO_BLOCK != b1) {
		if (1 != ext2_read_sector(nas, (char *) bp, 1, b1)) {
			rc = EIO;
			goto out;
		}
		if (new_ind) {
			ext2_zero_block(bp);
		}
		if (op & WMAP_FREE) {
			if (NO_BLOCK != (old_block = ext2_rd_indir(bp, index1))) {
				ext2_free_block(nas, old_block);
				fi->f_di.i_blocks -= fsi->s_sectors_in_block;
				ext2_wr_indir(bp, index1, NO_BLOCK );
			}

			/* Last reference in the indirect block gone? Then
			 * free the indirect block.
			 */
			if (ext2_empty_indir(bp, fsi)) {
				ext2_free_block(nas, b1);
				fi->f_di.i_blocks -= fsi->s_sectors_in_block;
				b1 = NO_BLOCK;
				/* Update the reference to the indirect block to
				 * NO_BLOCK - in the double indirect block if there
				 * is one, otherwise in the inode directly.
				 */
				if (single) {
					fi->f_di.i_block[EXT2_NDIR_BLOCKS] = b1;
				}
				else {
					ext2_wr_indir(bp_dindir, index2, b1);
				}
			}
		}
		else {
			ext2_wr_indir(bp, index1, new_block);
			fi->f_di.i_blocks += fsi->s_sectors_in_block;
		}

		ext2_write_sector(nas, (char *) bp, 1, b1);
	}

	/* If the single indirect block isn't there (or was just freed),
	 * see if we have to keep the double indirect block, if any.
	 * If we don't have to keep it, don't bother writing it out.
	 */
	if (NO_BLOCK == b1 && !single && NO_BLOCK != b2
			&& ext2_empty_indir(bp_dindir, fsi)) {
		ext2_free_block(nas, b2);
		fi->f_di.i_blocks -= fsi->s_sectors_in_block;
		b2 = NO_BLOCK;
		if (triple) {
			ext2_wr_indir(bp_tindir, index3, b2); /* update triple indir */
			if (1 != ext2_write_sector(nas, (char *) bp_tindir, 1, b3)) {
				rc = EIO;
				goto out;
			}
		} else {
			fi->f_di.i_block[EXT2_DIND_BLOCK] = b2;
		}
	}
	/* If the double indirect block isn't there (or was just freed),
	 * see if we have to keep the triple indirect block, if any.
	 * If we don't have to keep it, don't bother writing it out.
	 */
	if (NO_BLOCK == b2 && triple && NO_BLOCK != b3
			&& ext2_empty_indir(bp_tindir, fsi)) {
		ext2_free_block(nas, b3);
		fi->f_di.i_blocks -= fsi->s_sectors_in_block;
		fi->f_di.i_block[EXT2_TIND_BLOCK] = NO_BLOCK;
	}

	if (new_dbl && NO_BLOCK != (b2 = fi->f_di.i_block[EXT2_DIND_BLOCK])) {
		ext2_write_sector(nas, bp_dindir, 1, b2);/* release double indirect blk */
	}
	if (new_triple && NO_BLOCK != (b3 = fi->f_di.i_block[EXT2_TIND_BLOCK])) {
		ext2_write_sector(nas, bp_tindir, 1, b3);/* release triple indirect blk */
	}

	out:
	if (NULL != bp) {
		ext2_buff_free(nas, (char *) bp);
	}
	if (NULL != bp_dindir) {
		ext2_buff_free(nas, (char *) bp_dindir);
	}
	if (NULL != bp_tindir) {
		ext2_buff_free(nas, (char *) bp_tindir);
	}
	return rc;
}

static void ext2_wr_indir(char *buf, int index, uint32_t block) {
	/* write a block into an indirect block */
	b_ind(buf) [index] = block;
}

static int ext2_empty_indir(char *buf, struct ext2_fs_info *sb) {
	/* Return nonzero if the indirect block pointed to by bp contains
	 * only NO_BLOCK entries.
	 */
	long addr_in_block;
	int i;

	addr_in_block = sb->s_block_size / 4; /* 4 bytes per addr */
	for (i = 0; i < addr_in_block; i++) {
		if (b_ind(buf) [i] != NO_BLOCK ) {
			return 0;
		}
	}
	return 1;
}

static int ext2_new_block(struct nas *nas, long position) {
	/* Acquire a new block and return a pointer to it.*/
	int rc;
	uint32_t b;
	uint32_t goal;
	long position_diff;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	if (0 != (rc = ext2_block_map(nas, lblkno(fsi, position), &b))) {
		return rc;
	}
	/* Is another block available? */
	if (NO_BLOCK == b) {
		/* Check if this position follows last allocated block. */
		goal = NO_BLOCK;
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

		if (NO_BLOCK == (b = ext2_alloc_block(nas, goal))) {
			return ENOSPC;
		}
		/* clear new sector */
		memset(fi->f_buf, 0, fsi->s_block_size);
		ext2_write_sector(nas, fi->f_buf, 1, b);

		if (0 != (rc = ext2_write_map(nas, position, b, 0))) {
			ext2_free_block(nas, b);
			return rc;
		}
		fi->f_last_pos_bl_alloc = position;
		if (0 == position) {
			/* fi->f_last_pos_bl_alloc points to the block position,
			 * and zero indicates first usage, thus just increment.
			 */
			fi->f_last_pos_bl_alloc++;
		}
	}
	return 0;
}

static void ext2_zero_block(char *buf) {
	/* Zero a block. */
	memset(b_data(buf), 0, (size_t) 1024);
}

static int ext2_new_node(struct nas *nas,
		struct nas * parents_nas);

static int ext2_create(struct nas *nas, struct nas *parents_nas) {
	int rc;
	struct ext2_file_info *fi, *dir_fi;

	dir_fi = parents_nas->fi->privdata;

	if (0 != (rc = ext2_open(parents_nas))) {
		return rc;
	}

	/* Create a new file inode */
	if (0 != (rc = ext2_new_node(nas, parents_nas))) {
		return rc;
	}
	fi = nas->fi->privdata;

	dir_fi->f_di.i_links_count++;
	ext2_rw_inode(parents_nas, &dir_fi->f_di, EXT2_W_INODE);

	ext2_close(parents_nas);

	if (NULL != fi) {
		ext2_buff_free(nas, fi->f_buf);
		return 0;
	}
	return ENOSPC;
}

static int ext2_mkdir(struct nas *nas, struct nas *parents_nas) {
	int rc;
	int r1, r2; /* status codes */
	ino_t dot, dotdot; /* inode numbers for . and .. */
	struct ext2_file_info *fi, *dir_fi;

	if (!node_is_directory(parents_nas->node)) {
		rc = ENOTDIR;
		return rc;
	}

	dir_fi = parents_nas->fi->privdata;

	/* read the directory inode */
	if (0 != (rc = ext2_open(parents_nas))) {
		return rc;
	}
	/* Create a new directory inode. */
	if (0 != (rc = ext2_new_node(nas, parents_nas))) {
		ext2_close(parents_nas);
		return ENOSPC;
	}
	fi = nas->fi->privdata;
	/* Get the inode numbers for . and .. to enter in the directory. */
	dotdot = dir_fi->f_num; /* parent's inode number */
	dot = fi->f_num; /* inode number of the new dir itself */
	/* Now make dir entries for . and .. unless the disk is completely full. */
	/* Use dot1 and dot2, so the mode of the directory isn't important. */
	/* enter . in the new dir*/
	r1 = ext2_dir_operation(nas, ".", &dot, ENTER, S_IFDIR);
	/* enter .. in the new dir */
	r2 = ext2_dir_operation(nas, "..", &dotdot, ENTER, S_IFDIR);

	/* If both . and .. were successfully entered, increment the link counts. */
	if (r1 != 0 || r2 != 0) {
		/* It was not possible to enter . or .. probably disk was full -
		 * links counts haven't been touched.
		 */
		ext2_dir_operation(parents_nas, (char *) nas->node->name/*string*/,
				&dot, DELETE, 0);
		/* TODO del inode and clear the pool*/
		return (r1 | r2);
	}

	dir_fi->f_di.i_links_count++;
	ext2_rw_inode(parents_nas, &dir_fi->f_di, EXT2_W_INODE);

	ext2_buff_free(nas, fi->f_buf);
	ext2_close(parents_nas);

	if (NULL == fi) {
		return ENOSPC;
	}
	return 0;
}

static void ext2_wipe_inode(struct ext2_file_info *fi,
		struct ext2_file_info *dir_fi) {
	/* Erase some fields in the ext2_file_info. This function is called from alloc_inode()
	 * when a new ext2_file_info is to be allocated, and from truncate(), when an existing
	 * ext2_file_info is to be truncated.
	 */
	struct ext2fs_dinode *di = &fi->f_di;
	struct ext2fs_dinode *dir_di = &dir_fi->f_di;

	di->i_size = 0;
	di->i_blocks = 0;
	di->i_flags = 0;
	di->i_faddr = 0;

	for (int i = 0; i < EXT2_N_BLOCKS; i++) {
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
static int ext2_find_group_any(struct ext2_fs_info *fsi) {
	int group, ngroups;
	struct ext2_gd *gd;

	group = 0;
	ngroups = fsi->s_groups_count;

	for (; group < ngroups; group++) {
		gd = ext2_get_group_desc(group, fsi);
		if (gd == NULL ) {
			return EIO;
		}
		if (gd->free_inodes_count) {
			return group;
		}
	}
	return EIO;
}

static int ext2_free_inode_bit(struct nas *nas, uint32_t bit_returned,
		int is_dir) {
	/* Return an inode by turning off its bitmap bit. */
	int group; /* group number of bit_returned */
	int bit; /* bit_returned number within its group */
	struct ext2_gd *gd;
	struct ext2_fs_info *fsi;
	struct ext2_file_info *fi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	/* At first search group, to which bit_returned belongs to
	 * and figure out in what word bit is stored.
	 */
	if (bit_returned > fsi->e2sb.s_inodes_count||
	bit_returned < EXT2_FIRST_INO(&fsi->e2sb)) {
		return ENOMEM;
	}

	group = (bit_returned - 1) / fsi->e2sb.s_inodes_per_group;
	bit = (bit_returned - 1) % fsi->e2sb.s_inodes_per_group; /* index in bitmap */

	if (NULL == (gd = ext2_get_group_desc(group, fsi))) {
		return ENOMEM;
	}
	if (1 != ext2_read_sector(nas, fi->f_buf, 1, gd->inode_bitmap)) {
		return EIO;
	}
	if (ext2_unsetbit(b_bitmap(fi->f_buf), bit)) {
		return EIO;
	}

	if (1 != ext2_write_sector(nas, fi->f_buf, 1, gd->inode_bitmap)) {
		return EIO;
	}
	gd->free_inodes_count++;
	fsi->e2sb.s_free_inodes_count++;
	if (is_dir) {
		gd->used_dirs_count--;
	}

	return (ext2_write_sblock(nas) | ext2_write_gdblock(nas));
}

static uint32_t ext2_alloc_inode_bit(struct nas *nas, int is_dir) { /* inode will be a directory if it is TRUE */
	int group;
	ino_t inumber;
	uint32_t bit;
	struct ext2_gd *gd;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	inumber = 0;

	group = ext2_find_group_any(fsi);

	/* Check if we have a group where to allocate an ext2_file_info */
	if (group == -1) {
		return 0; /* no bit could be allocated */
	}

	if (NULL == (gd = ext2_get_group_desc(group, fsi))) {
		return 0;
	}

	/* find_group_* should always return either a group with
	 * a free ext2_file_info slot or -1, which we checked earlier.
	 */
	if (1 != ext2_read_sector(nas, fi->f_buf, 1, gd->inode_bitmap)) {
		return 0;
	}

	bit = ext2_setbit(b_bitmap(fi->f_buf), fsi->e2sb.s_inodes_per_group, 0);

	inumber = group * fsi->e2sb.s_inodes_per_group + bit + 1;

	/* Extra checks before real allocation.
	 * Only major bug can cause problems. Since setbit changed
	 * bp->b_bitmap there is no way to recover from this bug.
	 * Should never happen.
	 */
	if (inumber > fsi->e2sb.s_inodes_count) {
		return 0;
	}

	if (inumber < EXT2_FIRST_INO(&fsi->e2sb)) {
		return 0;
	}

	ext2_write_sector(nas, fi->f_buf, 1, gd->inode_bitmap);

	gd->free_inodes_count--;
	fsi->e2sb.s_free_inodes_count--;
	if (is_dir) {
		gd->used_dirs_count++;
	}
	if (ext2_write_sblock(nas) || ext2_write_gdblock(nas)) {
		inumber = 0;
	}

	return inumber;
}

static int ext2_free_inode(struct nas *nas) { /* ext2_file_info to free */
	/* Return an ext2_file_info to the pool of unallocated inodes. */
	int rc;
	uint32_t pos;
	uint32_t b;
	struct ext2fs_dinode fdi;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	/* Locate the appropriate super_block. */
	if (0!= (rc = ext2_read_sblock(nas))) {
		return rc;
	}

	/* free all data block of file */
	for(pos = 0; pos <= fi->f_di.i_size; pos += fsi->s_block_size) {
		if (0 != (rc = ext2_block_map(nas, lblkno(fsi, pos), &b))) {
			return rc;
		}
		ext2_free_block(nas, b);
	}

	/* clear inode in inode table */
	memset(&fdi, 0, sizeof(struct ext2fs_dinode));
	ext2_rw_inode(nas, &fdi, EXT2_W_INODE);

	/* free inode bitmap */
	b = fi->f_num;
	if (b <= 0 || b > fsi->e2sb.s_inodes_count) {
		return ENOSPC;
	}
	rc = ext2_free_inode_bit(nas, b, node_is_directory(nas->node));

	ext2_close(nas);
	pool_free(&ext2_file_pool, fi);

	return rc;
}

static int ext2_alloc_inode(struct nas *nas,
		struct nas *parents_nas) {
	/* Allocate a free inode in inode table and return a pointer to it. */
	int rc;
	struct ext2_file_info *fi, *dir_fi;
	struct ext2_fs_info *fsi;
	uint32_t b;

	dir_fi = parents_nas->fi->privdata;
	fsi = parents_nas->fs->fsi;

	if (NULL == (fi = ext2_fi_alloc(nas, parents_nas->fs))) {
		rc = ENOSPC;
		goto out;
	}
	memset(fi, 0, sizeof(struct ext2_file_info));

	if (NULL == (fi->f_buf = ext2_buff_alloc(nas, fsi->s_block_size))) {
		rc = ENOSPC;
		goto out;
	}

	if (0 != (rc = ext2_read_sblock(nas))) {
		goto out;
	}

	/* Acquire an inode from the bit map. */
	if (0 == (b = ext2_alloc_inode_bit(nas, node_is_directory(nas->node)))) {
		rc = ENOSPC;
		goto out;
	}

	fi->f_num = b;
	ext2_wipe_inode(fi, dir_fi);

	return 0;

	out: vfs_del_leaf(nas->node);
	if (NULL != fi) {
		if (NULL != fi->f_buf) {
			ext2_buff_free(nas, fi->f_buf);
		}
		pool_free(&ext2_file_pool, fi);
	}
	return rc;
}

void ext2_rw_inode(struct nas *nas, struct ext2fs_dinode *fdi,
		int rw_flag) {
	/* An entry in the inode table is to be copied to or from the disk. */

	struct ext2_gd *gd;
	struct ext2fs_dinode *dip;
	unsigned int block_group_number;
	uint32_t b, offset;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	/* Get the block where the inode resides. */
	ext2_read_sblock(nas);

	block_group_number = (fi->f_num - 1) / fsi->e2sb.s_inodes_per_group;
	if (NULL == (gd = ext2_get_group_desc(block_group_number, fsi))) {
		return;
	}
	offset = ((fi->f_num - 1) % fsi->e2sb.s_inodes_per_group)
			* EXT2_INODE_SIZE(&fsi->e2sb);
	/* offset requires shifting, since each block contains several inodes,
	 * e.g. inode 2 is stored in bklock 0.
	 */
	b = (uint32_t) gd->inode_table + (offset >> fsi->s_blocksize_bits);

	ext2_read_sector(nas, fi->f_buf, 1, b);

	offset &= (fsi->s_block_size - 1);
	dip = (struct ext2fs_dinode*) (b_data(fi->f_buf) + offset);

	e2fs_i_bswap(dip, dip);

	/* Do the read or write. */
	if (rw_flag) {
		memcpy(dip, fdi, sizeof(struct ext2fs_dinode));
		e2fs_i_bswap(dip, dip);
		ext2_write_sector(nas, fi->f_buf, 1, b);
	}
	else {
		memcpy(fdi, dip, sizeof(struct ext2fs_dinode));
	}
}

static int ext2_dir_operation(struct nas *nas, char *string, ino_t *numb,
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
	struct ext2fs_direct *dp = NULL;
	struct ext2fs_direct *prev_dp = NULL;
	int i, e_hit, t, match;
	uint32_t pos;
	unsigned new_slots;
	uint32_t b;
	int extended;
	int required_space;
	int string_len;
	int new_slot_size, actual_size;
	u16_t temp;
	struct ext2fs_dinode fdi;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	/* If 'fi' is not a pointer to a dir inode, error. */
	if (!node_is_directory(nas->node)) {
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

	for (; pos < fi->f_di.i_size; pos += fsi->s_block_size) {
		if (0 != (rc = ext2_block_map(nas, lblkno(fsi, pos), &b))) {
			return rc;
		}

		/* Since directories don't have holes, 'b' cannot be NO_BLOCK. */
		/* get a dir block */
		if (1 != ext2_read_sector(nas, fi->f_buf, 1, b)) {
			return EIO;
		}
		prev_dp = NULL; /* New block - new first dentry, so no prev. */

		/* Search a directory block.
		 * Note, we set prev_dp at the end of the loop.
		 */
		for (dp = (struct ext2fs_direct*) &b_data(fi->f_buf) ;
				CUR_DISC_DIR_POS(dp, &b_data(fi->f_buf)) < fsi->s_block_size;
				dp = NEXT_DISC_DIR_DESC(dp) ) {

			if (prev_dp == dp) {
				/* no dp in directory entry */
				dp->e2d_reclen = fsi->s_block_size;
			}
			/* Match occurs if string found. */
			if (ENTER != flag && 0 != dp->e2d_ino) {
				if (IS_EMPTY == flag) {
					/* If this test succeeds, dir is not empty. */
					if(0 == path_is_dotname(dp->e2d_name, dp->e2d_namlen)) {
						match = 1;
					}
				}
				else {
					if ((dp->e2d_namlen == strlen(string)) &&
						(0 == strncmp(dp->e2d_name, string, dp->e2d_namlen))) {
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
					if (dp->e2d_namlen >= sizeof(ino_t)) {
						/* Save d_ino for recovery. */
						t = dp->e2d_namlen - sizeof(ino_t);
						*((ino_t *) &dp->e2d_name[t]) = dp->e2d_ino;
					}
					dp->e2d_ino = 0; /* erase entry */
					/* Now we have cleared dentry, if it's not the first one,
					 * merge it with previous one.  Since we assume, that
					 * existing dentry must be correct, there is no way to
					 * spann a data block.
					 */
					if (prev_dp) {
						temp = prev_dp->e2d_reclen;
						temp += dp->e2d_reclen;
						prev_dp->e2d_reclen = temp;
					}
				}
				else { /* 'flag' is LOOK_UP */
					*numb = (ino_t) dp->e2d_ino;
				}
				if (1 != ext2_write_sector(nas, fi->f_buf, 1, b)) {
					return EIO;
				}
				return rc;
			}

			/* Check for free slot for the benefit of ENTER. */
			if (ENTER ==  flag && 0 == dp->e2d_ino) {
				/* we found a free slot, check if it has enough space */
				if (required_space <= dp->e2d_reclen) {
					e_hit = 1; /* we found a free slot */
					break;
				}
			}
			/* Can we shrink dentry? */
			if (ENTER == flag && required_space <= DIR_ENTRY_SHRINK(dp)) {
				/* Shrink directory and create empty slot, now
				 * dp->d_rec_len = DIR_ENTRY_ACTUAL_SIZE + DIR_ENTRY_SHRINK.
				 */
				new_slot_size = dp->e2d_reclen;
				actual_size = DIR_ENTRY_ACTUAL_SIZE(dp);
				new_slot_size -= actual_size;
				dp->e2d_reclen = actual_size;
				dp = NEXT_DISC_DIR_DESC(dp);
				dp->e2d_reclen = new_slot_size;
				/* if we fail before writing real ino */
				dp->e2d_ino = 0;
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
		if (1 != ext2_write_sector(nas, fi->f_buf, 1, b)) {
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
		if (0 != (rc = ext2_new_block(nas, fi->f_di.i_size))) {
			return rc;
		}
		dp = (struct ext2fs_direct*) &b_data(fi->f_buf);
		dp->e2d_reclen = fsi->s_block_size;
		dp->e2d_namlen = DIR_ENTRY_MAX_NAME_LEN(dp); /* for failure */
		extended = 1;
	}

	/* 'bp' now points to a directory block with space. 'dp' points to slot. */
	dp->e2d_namlen = string_len;
	for (i = 0; i < PATH_MAX
			&& i < dp->e2d_namlen && string[i];	i++) {
		dp->e2d_name[i] = string[i];
	}
	dp->e2d_ino = (int) *numb;
	if (HAS_INCOMPAT_FEATURE(&fsi->e2sb, EXT2F_INCOMPAT_FILETYPE)) {
		dp->e2d_type = ext2_type_from_mode_fmt(mode_fmt);
	}

	if (1 != ext2_write_sector(nas, fi->f_buf, 1, b)) {
		return EIO;
	}

	if (1 == new_slots) {
		fi->f_di.i_size += (uint32_t) dp->e2d_reclen;
		/* Send the change to disk if the directory is extended. */
		if (extended) {
			memcpy(&fdi, &fi->f_di, sizeof(struct ext2fs_dinode));
			ext2_rw_inode(nas, &fdi, EXT2_W_INODE);
		}
	}
	return 0;
}

static int ext2_new_node(struct nas *nas,
		struct nas * parents_nas) {
	/* It allocates a new inode, makes a directory entry for it in
	 * the dir_fi directory with string name, and initializes it.
	 * It returns a pointer to the ext2_file_info if it can do this;
	 * otherwise it returns NULL.
	 */
	int rc;
	struct ext2_file_info *fi;
	struct ext2fs_dinode fdi;
	struct ext2_fs_info *fsi;

	fsi = parents_nas->fs->fsi;

	/* Last path component does not exist.  Make new directory entry. */
	if (0 != (rc = ext2_alloc_inode(nas, parents_nas))) {
		/* Can't creat new inode: out of inodes. */
		return rc;
	}

	fi = nas->fi->privdata;

	/* Force inode to the disk before making directory entry to make
	 * the system more robust in the face of a crash: an inode with
	 * no directory entry is much better than the opposite.
	 */
	if (node_is_directory(nas->node)) {
		fi->f_di.i_size = fsi->s_block_size;
		if (0 != ext2_new_block(nas, fsi->s_block_size - 1)) {
			return ENOSPC;
		}
		fi->f_di.i_links_count++; /* directory have 2 links */
	}
	fi->f_di.i_mode = nas->node->mode;
	fi->f_di.i_links_count++;

	memcpy(&fdi, &fi->f_di, sizeof(struct ext2fs_dinode));
	ext2_rw_inode(nas, &fdi, EXT2_W_INODE);/* force inode to disk now */

	/* New inode acquired.  Try to make directory entry. */
	if (0 != (rc = ext2_dir_operation(parents_nas, (char *) nas->node->name,
			&fi->f_num, ENTER, nas->node->mode))) {
		return rc;
	}
	/* The caller has to return the directory ext2_file_info (*dir_fi).  */
	return 0;
}


/* Unlink 'file_name'; rip must be the inode of 'file_name' or NULL. */
static int ext2_unlink_file(struct nas *dir_nas, struct nas *nas) {
	int rc;


	if ((0 != (rc = ext2_open(nas))) || (0 != (rc = ext2_free_inode(nas)))) {
		return rc;
	}
	return ext2_dir_operation(dir_nas,
			(char *) nas->node->name, NULL, DELETE, 0);
}

static int ext2_remove_dir(struct nas *dir_nas, struct nas *nas) {
	/* A directory file has to be removed. Five conditions have to met:
	* 	- The file must be a directory
	*	- The directory must be empty (except for . and ..)
	*	- The final component of the path must not be . or ..
	*	- The directory must not be the root of a mounted file system (VFS)
	*	- The directory must not be anybody's root/working directory (VFS)
	*/
	int rc;
	char *dir_name;
	struct ext2_file_info *fi;

	fi = nas->fi->privdata;
	dir_name = (char *) nas->node->name;

	/* search_dir checks that rip is a directory too. */
	if (0 != (rc = ext2_dir_operation(nas, "", NULL, IS_EMPTY, 0))) {
		return -1;
	}

	if(path_is_dotname(dir_name, strlen(dir_name))) {
		return EINVAL;
	}

	if (fi->f_num == ROOT_INODE) {
		return EBUSY; /* can't remove 'root' */
	}

	/* Unlink . and .. from the dir. */
	if (0 != (rc = (ext2_dir_operation(nas, ".", NULL, DELETE, 0) |
				ext2_dir_operation(nas, "..", NULL, DELETE, 0)))) {
		return rc;
	}

	/* Actually try to unlink the file; fails if parent is mode 0 etc. */
	if (0 != (rc = ext2_unlink_file(dir_nas, nas))) {
		return rc;
	}

	return 0;
}

static int ext2_unlink(struct nas *dir_nas, struct nas *nas) {
	int rc;
	struct ext2_file_info *dir_fi;

	dir_fi = dir_nas->fi->privdata;

	/* Temporarily open the dir. */
	if (0 != (rc = ext2_open(dir_nas))) {
		return rc;
	}

	if (node_is_directory(nas->node)) {
		rc = ext2_remove_dir(dir_nas, nas); /* call is RMDIR */
	}
	else {
		rc = ext2_unlink_file(dir_nas, nas);
	}

	if(0 == rc) {
		dir_fi->f_di.i_links_count--;
		ext2_rw_inode(dir_nas, &dir_fi->f_di, EXT2_W_INODE);
	}

	ext2_close(dir_nas);

	return rc;
}

#if __BYTE_ORDER == __BIG_ENDIAN
void e2fs_sb_bswap(struct ext2sb *old, struct ext2sb *new) {
	/* preserve unused fields */
	memcpy(new, old, sizeof(struct ext2sb));
	new->s_inodes_count = bswap32(old->s_inodes_count);
	new->s_blocks_count = bswap32(old->s_blocks_count);
	new->s_r_blocks_count = bswap32(old->s_r_blocks_count);
	new->s_free_blocks_count = bswap32(old->s_free_blocks_count);
	new->s_free_inodes_count = bswap32(old->s_free_inodes_count);
	new->s_first_data_block = bswap32(old->s_first_data_block);
	new->s_log_block_size = bswap32(old->s_log_block_size);
	new->s_log_frag_size = bswap32(old->s_log_frag_size);
	new->s_blocks_per_group = bswap32(old->s_blocks_per_group);
	new->s_frags_per_group = bswap32(old->s_frags_per_group);
	new->s_inodes_per_group = bswap32(old->s_inodes_per_group);
	new->s_mtime = bswap32(old->s_mtime);
	new->s_wtime = bswap32(old->s_wtime);
	new->s_mnt_count = bswap16(old->s_mnt_count);
	new->s_max_mnt_count = bswap16(old->s_max_mnt_count);
	new->s_magic = bswap16(old->s_magic);
	new->s_state = bswap16(old->s_state);
	new->s_errors = bswap16(old->s_errors);
	new->s_minor_rev_level = bswap16(old->s_minor_rev_level);
	new->s_lastcheck = bswap32(old->s_lastcheck);
	new->s_checkinterval = bswap32(old->s_checkinterval);
	new->s_creator_os = bswap32(old->s_creator_os);
	new->s_rev_level = bswap32(old->s_rev_level);
	new->s_def_resuid = bswap16(old->s_def_resuid);
	new->s_def_resgid = bswap16(old->s_def_resgid);
	new->s_first_ino = bswap32(old->s_first_ino);
	new->s_inode_size = bswap16(old->s_inode_size);
	new->s_block_group_nr = bswap16(old->s_block_group_nr);
	new->s_feature_compat = bswap32(old->s_feature_compat);
	new->s_feature_incompat = bswap32(old->s_feature_incompat);
	new->s_feature_ro_compat = bswap32(old->s_feature_ro_compat);
	new->s_algorithm_usage_bitmap = bswap32(old->s_algorithm_usage_bitmap);
	new->s_padding1 = bswap16(old->s_padding1);
}

void e2fs_cg_bswap(struct ext2_gd *old, struct ext2_gd *new, int size) {
	int i;

	for (i = 0; i < (size / sizeof(struct ext2_gd)); i++) {
		new[i].block_bitmap = bswap32(old[i].block_bitmap);
		new[i].inode_bitmap = bswap32(old[i].inode_bitmap);
		new[i].inode_table = bswap32(old[i].inode_table);
		new[i].free_blocks_count = bswap16(old[i].free_blocks_count);
		new[i].free_inodes_count = bswap16(old[i].free_inodes_count);
		new[i].used_dirs_count = bswap16(old[i].used_dirs_count);
	}
}

void e2fs_i_bswap(struct ext2fs_dinode *old, struct ext2fs_dinode *new) {

	new->i_mode = bswap16(old->i_mode);
	new->i_uid = bswap16(old->i_uid);
	new->i_gid = bswap16(old->i_gid);
	new->i_links_count = bswap16(old->i_links_count);
	new->i_size = bswap32(old->i_size);
	new->i_atime = bswap32(old->i_atime);
	new->i_ctime = bswap32(old->i_ctime);
	new->i_mtime = bswap32(old->i_mtime);
	new->i_dtime = bswap32(old->i_dtime);
	new->i_blocks = bswap32(old->i_blocks);
	new->i_flags = bswap32(old->i_flags);
	new->i_gen = bswap32(old->i_gen);
	new->i_facl = bswap32(old->i_facl);
	new->i_dacl = bswap32(old->i_dacl);
	new->i_faddr = bswap32(old->i_faddr);
	memcpy(&new->i_block[0], &old->i_block[0],
			(NDADDR + NIADDR) * sizeof(uint32_t));
}
#else
void e2fs_i_bswap(struct ext2fs_dinode *old, struct ext2fs_dinode *new) {

}
#endif

DECLARE_FILE_SYSTEM_DRIVER(ext2fs_driver);

/*
 int ext2_fs_slink() {
	phys_bytes len;
	struct ext2_file_info *sip;            inode containing symbolic link
	struct ext2_file_info *dir_fi;          directory containing link
	int r;               error code
	char string[NAME_MAX];        last component of the new dir's path name
	char* link_target_buf = NULL;        either sip->i_block or bp->b_data
	struct buf *bp = NULL;     disk buffer for link

	caller_uid = (uid_t) fs_m_in.REQ_UID;
	caller_gid = (gid_t) fs_m_in.REQ_GID;

	Copy the link name's last component
	len = fs_m_in.REQ_PATH_LEN;
	if (len > NAME_MAX || len > EXT2_NAME_MAX) {
		return(ENAMETOOLONG);
	}

	r = sys_safecopyfrom(VFS_PROC_NR, (cp_grant_id_t) fs_m_in.REQ_GRANT,
	(vir_bytes) 0, (vir_bytes) string, (size_t) len);
	if (r != OK) {
	 	 return(r);
	}
	NUL(string, len, sizeof(string));

	Temporarily open the dir.
	if ( (dir_fi = get_inode(fs_dev, (ino_t) fs_m_in.REQ_INODE_NR)) == NULL) {
	 	 return(EINVAL);
	}
	Create the inode for the symlink.
	sip = new_node(dir_fi, string, (mode_t) (I_SYMBOLIC_LINK | RWX_MODES),
	(block_t) 0);

	If we can then create fast symlink (store it in inode),
	* Otherwise allocate a disk block for the contents of the symlink and
	* copy contents of symlink (the name pointed to) into first disk block.
	if ((r = err_code) == OK) {
		 if ( (fs_m_in.REQ_MEM_SIZE + 1) > sip->i_sp->s_block_size) {
			 r = ENAMETOOLONG;
		 } else if ((fs_m_in.REQ_MEM_SIZE + 1) <= MAX_FAST_SYMLINK_LENGTH) {
			 r = sys_safecopyfrom(VFS_PROC_NR,
			 (cp_grant_id_t) fs_m_in.REQ_GRANT3,
			 (vir_bytes) 0, (vir_bytes) sip->i_block,
			 (vir_bytes) fs_m_in.REQ_MEM_SIZE);
			 sip->i_dirt = IN_DIRTY;
			 link_target_buf = (char*) sip->i_block;
		 } else {
			 if ((bp = new_block(sip, (off_t) 0)) != NULL) {
				 sys_safecopyfrom(VFS_PROC_NR,
				 (cp_grant_id_t) fs_m_in.REQ_GRANT3,
				 (vir_bytes) 0, (vir_bytes) b_data(bp),
				 (vir_bytes) fs_m_in.REQ_MEM_SIZE);
				 //lmfs_markdirty(bp);
				 link_target_buf = b_data(bp);
			 } else {
				 r = err_code;
			 }
		 }
		 if (r == OK) {
			 assert(link_target_buf);
			 link_target_buf[fs_m_in.REQ_MEM_SIZE] = '\0';
			 sip->i_size = (off_t) strlen(link_target_buf);
			 if (sip->i_size != fs_m_in.REQ_MEM_SIZE) {
				 This can happen if the user provides a buffer
				 * with a \0 in it. This can cause a lot of trouble
				 * when the symlink is used later. We could just use
				 * the strlen() value, but we want to let the user
				 * know he did something wrong. ENAMETOOLONG doesn't
				 * exactly describe the error, but there is no
				 * ENAMETOOWRONG.

				 r = ENAMETOOLONG;
			 }
		 }

		 put_block(bp, DIRECTORY_BLOCK);  put_block() accepts NULL.

		 if (r != OK) {
			 sip->i_links_count = NO_LINK;
			 if (search_dir(dir_fi, string, NULL, DELETE, IGN_PERM, 0) != OK)
			 panic("Symbolic link vanished");
		 }
	}

	put_inode() accepts NULL as a noop, so the below are safe.
	put_inode(sip);
	put_inode(dir_fi);

	return(r);
 }
 */
