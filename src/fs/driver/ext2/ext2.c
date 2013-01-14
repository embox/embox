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

#include <fs/fs_drv.h>
#include <fs/vfs.h>
#include <fs/ext2.h>
#include <fs/path.h>
#include <util/array.h>
#include <embox/unit.h>
#include <embox/block_dev.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>
#include <drivers/ramdisk.h>
#include <fs/file_system.h>
#include <fs/file_desc.h>
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
static int ext2_block_map(struct nas *nas, int32_t, int32_t *);
static int ext2_buf_read_file(struct nas *nas, char **, size_t *);
static int ext2_search_directory(struct nas *nas, const char *, int, uint32_t *);
static int ext2_read_sblock(struct nas *nas);
static int ext2_read_gdblock(struct nas *nas);
static int ext2_mount_entry(struct nas *nas);

/* ext filesystem description pool */
POOL_DEF(ext2_fs_pool, struct ext2_fs_info, OPTION_GET(NUMBER,ext_descriptor_quantity));

/* ext file description pool */
POOL_DEF(ext2_file_pool, struct ext2_file_info, OPTION_GET(NUMBER,ext_inode_quantity));

#define EXT_NAME "ext2"

static int    ext2fs_open(struct node *node, struct file_desc *file_desc, int flags);
static int    ext2fs_close(struct file_desc *desc);
static size_t ext2fs_read(struct file_desc *desc, void *buf, size_t size);
static size_t ext2fs_write(struct file_desc *desc, void *buf, size_t size);
static int    ext2fs_ioctl(struct file_desc *desc, int request, va_list args);

static struct kfile_operations ext2_fop = {
		ext2fs_open,
		ext2fs_close,
		ext2fs_read,
		ext2fs_write,
		ext2fs_ioctl
};

/*
 * help function
 */

int ext2_read_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct ext2_fs_info *fsi;
	fsi = nas->fs->fsi;

	return block_dev_read(nas->fs->bdev, (char *) buffer,
			count * fsi->s_block_size, sector);
}


int ext2_write_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct ext2_fs_info *fsi;
	fsi = nas->fs->fsi;

	return block_dev_write(nas->fs->bdev, (char *) buffer,
			count * fsi->s_block_size, sector);
}

static void lmfs_markdirty(struct buf *bp) {
	bp->lmfs_dirt = 1;
};

static void lmfs_markclean(struct buf *bp) {
	bp->lmfs_dirt = 0;
};

static int lmfs_bytes(struct buf *bp)
{
	return bp->lmfs_bytes;
}

static uint32_t rd_indir(struct buf *bp, int index) {
	return  b_ind(bp)[index];
}

/*
 * Calculate indirect block levels.
 *
 * We note that the number of indirect blocks is always
 * a power of 2.  This lets us use shifts and masks instead
 * of divide and remainder and avoinds pulling in the
 * 64bit division routine into the boot code.
 */
static int ext2_shift_culc (struct ext2_file_info *fi,
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
static int ext2_read_symlink (struct nas *nas,
					uint32_t parent_inumber, const char **cp) {
	/* XXX should handle LARGEFILE */
	int len, link_len;
	int rc;
	uint32_t inumber;
	char namebuf[MAXPATHLEN + 1];
	int nlinks;
	int32_t	disk_block;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	nlinks = 0;
	link_len = fi->f_di.i_size;
	len = strlen(*cp);

	if ((link_len + len > MAXPATHLEN) ||
		(++nlinks > MAXSYMLINKS)) {
		return ENOENT;
	}

	memmove(&namebuf[link_len], cp, len + 1);

	if (link_len < EXT2_MAXSYMLINKLEN) {
		memcpy(namebuf, fi->f_di.i_block, link_len);
	}
	else {
		/* Read file for symbolic link */
		if (0 != (rc = ext2_block_map(nas, (int32_t)0, &disk_block))) {
			return rc;
		}
		if (SBSIZE != ext2_read_sector(nas, fi->f_buf, 1,
						fsbtodb(fsi, disk_block))) {
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
	}
	else {
		inumber = (uint32_t)EXT2_ROOTINO;
	}
	rc = ext2_read_inode(nas, inumber);

	return rc;
}

/* set node type by file system file type */
static void ext2_set_node_type (int *type, uint8_t e2d_type) {

	switch (e2d_type) {
	case 1:
		*type = NODE_TYPE_FILE;
		break;
	case 2:
		*type = NODE_TYPE_DIRECTORY;
		break;
	default:
		*type = NODE_TYPE_SPECIAL;
		break;
	}
};

static int ext2_close(struct nas *nas) {
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	if (NULL != fsi) {
		if (fsi->e2fs_gd) {
			free(fsi->e2fs_gd);
		}
	}

	if (NULL != fi) {
		if (NULL != fi->f_buf) {
			free(fi->f_buf);
		}
	}

	return 0;
}

static int ext2_open(struct nas *nas) {
	int rc;
	char path[MAX_LENGTH_PATH_NAME];
	const char *cp, *ncp;
	uint32_t inumber, parent_inumber;

	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	/* prepare full path into this filesystem */
	vfs_get_path_by_node(nas->node, path);
	path_cut_mount_dir(path, fsi->mntto);

	/* allocate file system specific data structure */

	/* alloc a block sized buffer used for all transfers */
	fi->f_buf = malloc(fsi->s_block_size);

	/* read group descriptor blocks */
	fsi->e2fs_gd = malloc(sizeof(struct ext2_gd) * fsi->s_ncg);
	if(0 != (rc = ext2_read_gdblock(nas))) {
		return rc;
	}

	if(0 != ext2_shift_culc(fi, fsi)) {
		return rc;
	}

	inumber = EXT2_ROOTINO;
	if ((rc = ext2_read_inode(nas, inumber)) != 0) {
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
		if ((fi->f_di.i_mode & S_IFMT) != S_IFDIR) {
			rc = ENOTDIR;
			return rc;
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
			return rc;
		}
		/* Open next component */
		if (0 != (rc = ext2_read_inode(nas, inumber))) {
			return rc;
		}

		/* Check for symbolic link */
		if ((fi->f_di.i_mode & S_IFMT) == S_IFLNK) {
			if(0 != (rc = ext2_read_symlink (nas, parent_inumber, &cp))) {
				return rc;
			}
		}
	}
	return rc;
}

/*
 * file_operation
 */
static int ext2fs_open(struct node *node, struct file_desc *desc, int flags) {

	int rc;
	struct nas *nas;
	struct ext2_file_info *fi;

	nas = node->nas;
	fi = nas->fi->privdata;

	rc = ext2_open(nas);

	if (rc) {
		ext2_close(nas);
	}
	else {
		fi->f_seekp = 0;		/* reset seek pointer */
	}

	return rc;
}

static int ext2fs_close(struct file_desc *desc) {
	struct nas *nas;

	if(NULL == desc) {
		return 0;
	}
	nas = desc->node->nas;

	return ext2_close(nas);
}

static size_t ext2fs_read(struct file_desc *desc, void *buff, size_t size) {
	size_t csize;
	char *buf;
	size_t buf_size;
	int rc = 0;
	char *addr = buff;
	struct nas *nas;
	struct ext2_file_info *fi;

	nas = desc->node->nas;
	fi = nas->fi->privdata;

	while (size != 0) {
		/* XXX should handle LARGEFILE */
		if (fi->f_seekp >= (long)fi->f_di.i_size) {
			break;
		}

		if (0 != (rc = ext2_buf_read_file(nas, &buf, &buf_size))) {
			return 0;
		}

		csize = size;
		if (csize > buf_size) {
			csize = buf_size;
		}

		memcpy(addr, buf, csize);

		fi->f_seekp += csize;
		addr += csize;
		size -= csize;
	}

	return ((char *) buff - addr);
}


static size_t ext2fs_write(struct file_desc *desc, void *buff, size_t size) {
	size_t csize;
		char *buf;
		size_t buf_size;
		int rc = 0;
		char *addr = buff;
		struct nas *nas;
		struct ext2_file_info *fi;

		nas = desc->node->nas;
		fi = nas->fi->privdata;

		while (size != 0) {
			/* XXX should handle LARGEFILE */
			if (fi->f_seekp >= (long)fi->f_di.i_size) {
				break;
			}

			if (0 != (rc = ext2_buf_read_file(nas, &buf, &buf_size))) {
				return 0;
			}

			csize = size;
			if (csize > buf_size) {
				csize = buf_size;
			}

			memcpy(addr, buf, csize);

			fi->f_seekp += csize;
			addr += csize;
			size -= csize;
		}

		return ((char *) buff - addr);
}

static int ext2fs_ioctl(struct file_desc *desc, int request, va_list args) {
	return 0;
}

static int ext2_create(struct nas *nas, struct nas * parents_nas);
static int ext2_mkdir(struct nas *nas, struct nas * parents_nas);

static int ext2fs_init(void * par);
static int ext2fs_format(void *path);
static int ext2fs_mount(void *dev, void *dir);
static int ext2fs_create(struct node *parent_node, struct node *node);
static int ext2fs_delete(struct node *node);

static fsop_desc_t ext2_fsop = {
		ext2fs_init,
		ext2fs_format,
		ext2fs_mount,
		ext2fs_create,
		ext2fs_delete
};

static int ext2fs_init(void * par) {

	return 0;
};

static fs_drv_t ext2_drv = {
	.name = EXT_NAME,
	.file_op = &ext2_fop,
	.fsop = &ext2_fsop
};

static ext2_file_info_t *ext2_create_file(struct nas *nas, void *fs) {
	ext2_file_info_t *fi;

	if(NULL == (fi = pool_alloc(&ext2_file_pool))) {
		return NULL;
	}

	nas->fs = fs;
	nas->fi->ni.size = fi->f_seekp = 0;

	return fi;
}

static int ext2fs_create(struct node *parent_node, struct node *node) {

	struct ext2_file_info *fi;
	struct nas *nas, *parents_nas;
	int node_quantity;
	char path [MAX_LENGTH_PATH_NAME];

	nas = node->nas;
	parents_nas = parent_node->nas;

	if (node_is_directory(node)) {
		node_quantity = 3; /* need create . and .. directory */
		vfs_get_path_by_node(node, path);
	}
	else {
		node_quantity = 1;
	}

	for (int count = 0; count < node_quantity; count ++) {
		if(0 < count) {
			if(1 == count) {
				strcat(path, "/.");
			}
			else if(2 == count) {
				strcat(path, ".");
			}
			if(NULL == (node = vfs_add_path (path, NULL))) {
				return -ENOMEM;
			}
		}

		if(NULL == (fi = nas->fi->privdata =
				ext2_create_file(nas, parents_nas->fs))) {
			vfs_del_leaf(node);
			return -ENOMEM;
		}

		memset(fi, 0, sizeof(struct ext2_file_info));

		if (node_is_directory(node)) {
			return ext2_mkdir(nas, parents_nas);
		}
		else {
			return ext2_create(nas, parents_nas);
		}
	}
	return 0;
}

static int ext2fs_delete(struct node *node) {
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;
	node_t *pointnod;
	struct nas *nas;
	char path [MAX_LENGTH_PATH_NAME];

	nas = node->nas;
	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	vfs_get_path_by_node(node, path);

	/* need delete "." and ".." node for directory */
	if (node_is_directory(node)) {

		strcat(path, "/.");
		pointnod = vfs_find_node(path, NULL);
		vfs_del_leaf(pointnod);

		strcat(path, ".");
		pointnod = vfs_find_node(path, NULL);
		vfs_del_leaf(pointnod);

		path[strlen(path) - 3] = '\0';

		pool_free(&ext2_file_pool, fi);
	}

	/* root node - have fi, but haven't index*/
	if(0 == strcmp((const char *) path, (const char *) fsi->mntto)){
		pool_free(&ext2_fs_pool, fsi);
	}

	vfs_del_leaf(node);

	return 0;
}

static int ext2fs_format(void *dev) {

	return 0;
}

static int ext2fs_mount(void *dev, void *dir) {
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
		return -ENODEV;
	}

	if (NULL == (dir_nas->fs = alloc_filesystem(EXT_NAME))) {
		return -ENOMEM;
	}
	dir_nas->fs->bdev = dev_fi->privdata;

	/* allocate this fs info */
	if(NULL == (fsi = pool_alloc(&ext2_fs_pool))) {
		free_filesystem(dir_nas->fs);
		return -ENOMEM;
	}
	memset(fsi, 0, sizeof(struct ext2_fs_info));
	dir_nas->fs->fsi = fsi;
	vfs_get_path_by_node(dir_node, fsi->mntto);
	vfs_get_path_by_node(dev_node, fsi->mntfrom);

	if(NULL == (fi = pool_alloc(&ext2_file_pool))) {
		return -ENOMEM;
	}
	memset(fi, 0, sizeof(struct ext2_file_info));
	/*fi->index = fi->mode = 0;*/
	fi->f_seekp = 0;
	dir_nas->fi->privdata = (void *) fi;

	/* presetting that we think */
	fsi->s_block_size = SBSIZE;
	if(0 != ext2_read_sblock(dir_nas)) {
		return -1;
	}

	ext2_mount_entry(dir_nas);

	return 0;
}

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

	inode_sector = fsbtodb(fsi, ino_to_fsba(fsi, inumber));

	/* Read inode and save it */
	buf = fi->f_buf;
	rsize = ext2_read_sector(nas, buf, 1, inode_sector);
	if (rsize != fsi->s_block_size) {
		return EIO;
	}

	/* set pointer to inode struct in read buffer */
	dip = (struct ext2fs_dinode *)(buf +
	    EXT2_DINODE_SIZE(fsi) * ino_to_fsbo(fsi, inumber));
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
static int ext2_block_map(struct nas *nas,  int32_t file_block,
		             int32_t *disk_block_p) {
	uint level;
	int32_t ind_cache;
	int32_t ind_block_num;
	size_t rsize;
	int32_t *buf;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	buf = (void *)fi->f_buf;

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
		*disk_block_p =
		    fs2h32(fi->f_ind_cache[file_block & IND_CACHE_MASK]);
		return 0;
	}

	for (level = 0;;) {
		level += fi->f_nishift;
		if (file_block < (int32_t)1 << level)
			break;
		if (level > NIADDR * fi->f_nishift) {
			/* Block number too high */
			return EFBIG;
		}
		file_block -= (int32_t)1 << level;
	}

	ind_block_num =
	    fs2h32(fi->f_di.i_block[NDADDR + (level / fi->f_nishift - 1)]);

	for (;;) {
		level -= fi->f_nishift;
		if (ind_block_num == 0) {
			*disk_block_p = 0;	/* missing */
			return 0;
		}

		/*
		 * If we were feeling brave, we could work out the number
		 * of the disk sector and read a single disk sector instead
		 * of a filesystem block.
		 * However we don't do this very often anyway...
		 */
		rsize =
			ext2_read_sector(nas, (char *)buf, 1, fsbtodb(fsi, ind_block_num));

		if (rsize != fsi->s_block_size)
			return EIO;
		ind_block_num = fs2h32(buf[file_block >> level]);
		if (level == 0)
			break;
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
static int ext2_buf_read_file(struct nas *nas,
								char **buf_p, size_t *size_p) {
	long off;
	int32_t file_block;
	int32_t disk_block;
	size_t block_size;
	int rc;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	off = blkoff(fsi, fi->f_seekp);
	file_block = lblkno(fsi, fi->f_seekp);
	block_size = fsi->s_block_size;	/* no fragment */

	if (file_block != fi->f_buf_blkno) {
		if (0 != (rc = ext2_block_map(nas, file_block, &disk_block))) {
			return rc;
		}

		if (disk_block == 0) {
			memset(fi->f_buf, 0, block_size);
			fi->f_buf_size = block_size;
		} else {
			if(SBSIZE != ext2_read_sector(nas, fi->f_buf, 1,
									fsbtodb(fsi, disk_block))) {
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
	if (*size_p > fi->f_di.i_size - fi->f_seekp) {
		*size_p = fi->f_di.i_size - fi->f_seekp;
	}

	return 0;
}

/*
 * Search a directory for a name and return its
 * inode number.
 */
static int ext2_search_directory(struct nas *nas, const char *name, int length,
						     uint32_t *inumber_p) {
	struct ext2fs_direct *dp;
	struct ext2fs_direct *edp;
	char *buf;
	size_t buf_size;
	int namlen;
	int rc;
	struct ext2_file_info *fi;

	fi = nas->fi->privdata;
	fi->f_seekp = 0;
	/* XXX should handle LARGEFILE */
	while (fi->f_seekp < (long)fi->f_di.i_size) {
		if (0 != (rc = ext2_buf_read_file(nas, &buf, &buf_size))) {
			return rc;
		}

		dp = (struct ext2fs_direct *)buf;
		edp = (struct ext2fs_direct *)(buf + buf_size);
		for (; dp < edp;
		    dp = (struct ext2fs_direct *)((char *)dp + fs2h16(dp->e2d_reclen))) {
			if (fs2h16(dp->e2d_reclen) <= 0) {
				break;
			}
			if (fs2h32(dp->e2d_ino) == (uint32_t)0) {
				continue;
			}
			namlen = dp->e2d_namlen;
			if (namlen == length &&
			    !memcmp(name, dp->e2d_name, length)) {
				/* found entry */
				*inumber_p = fs2h32(dp->e2d_ino);
				return 0;
			}
		}
		fi->f_seekp += buf_size;
	}
	return ENOENT;
}

static int ext2_read_sblock(struct nas *nas) {
	static uint8_t sbbuf[SBSIZE];
	struct ext2_fs_info *fsi;
	struct ext2sb *ext2sb;

	fsi = nas->fs->fsi;
	ext2sb = &fsi->e2sb;

	if(SBSIZE != ext2_read_sector(nas, (char *)sbbuf, 1, SBOFF / SECTOR_SIZE)) {
		return EIO;
	}

	e2fs_sbload((void *)sbbuf, ext2sb);
	if (ext2sb->s_magic != E2FS_MAGIC)
		return EINVAL;
	if (ext2sb->s_rev_level > E2FS_REV1 ||
	    (ext2sb->s_rev_level == E2FS_REV1 &&
	     (ext2sb->s_first_ino != EXT2_FIRSTINO ||
	     (ext2sb->s_inode_size != 128 && ext2sb->s_inode_size != 256) ||
	      ext2sb->s_feature_incompat & ~EXT2F_INCOMPAT_SUPP))) {
		return ENODEV;
	}

	//e2fs_sbload((void *)sbbuf, &fsi->e2sb);
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
	fsi->s_itb_per_group = fsi->e2sb.s_inodes_per_group / fsi->s_inodes_per_block;

	fsi->s_groups_count = ((fsi->e2sb.s_blocks_count - fsi->e2sb.s_first_data_block - 1)
					/ fsi->e2sb.s_blocks_per_group) + 1;
	fsi->s_bsearch = fsi->e2sb.s_first_data_block + 1 + fsi->s_gdb_count + 2
				+ fsi->s_itb_per_group;

	fsi->s_igsearch = 0;

	return 0;
}

static int ext2_read_gdblock(struct nas *nas) {
	size_t rsize;
	uint gdpb;
	int i;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	gdpb = fsi->s_block_size / sizeof(struct ext2_gd);

	for (i = 0; i < fsi->s_gdb_count; i++) {
		rsize = ext2_read_sector(nas, fi->f_buf, 1,
				fsbtodb(fsi, fsi->e2sb.s_first_data_block + 1 + i));
		if (rsize != fsi->s_block_size) {
			return EIO;
		}

		e2fs_cgload((struct ext2_gd *)fi->f_buf,
		    &fsi->e2fs_gd[i * gdpb],
		    (i == (fsi->s_gdb_count - 1)) ?
		    (fsi->s_ncg - gdpb * i) * sizeof(struct ext2_gd):
		    fsi->s_block_size);
	}

	return 0;
}


/*
static long ext2_seek( long offset, int where) {
	struct ext2_file_info *fi = (struct ext2_file_info *)f->f_fsdata;

	switch (where) {
	case SEEK_SET:
		fi->f_seekp = offset;
		break;
	case SEEK_CUR:
		fi->f_seekp += offset;
		break;
	case SEEK_END:
		// XXX should handle LARGEFILE /
		fi->f_seekp = fi->f_di.e2di_size - offset;
		break;
	default:
		return -1;
	}
	return fi->f_seekp;
}


static int ext2_stat( struct stat *sb) {
	struct ext2_file_info *fi = (struct ext2_file_info *)f->f_fsdata;

	// only important stuff /
	memset(sb, 0, sizeof *sb);
	sb->st_mode = fi->f_di.e2di_mode;
	sb->st_uid = fi->f_di.e2di_uid;
	sb->st_gid = fi->f_di.e2di_gid;
	// XXX should handle LARGEFILE /
	sb->st_size = fi->f_di.e2di_size;
	return 0;
}
*/

static int ext2_mount_entry(struct nas *dir_nas) {
	char *buf;
	size_t buf_size;
	int rc;
	struct ext2fs_direct  *dp, *edp;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;
	char *name, *full_path;
	node_t *node;
	int type;

	fi = dir_nas->fi->privdata;
	fsi = dir_nas->fs->fsi;
	if(NULL == (full_path = malloc(MAX_LENGTH_PATH_NAME))) {
		return ENOMEM;
	}

	if(0 != (rc = ext2_open(dir_nas))) {
		goto out;
	}

	fi->f_seekp = 0;
	while (fi->f_seekp < (long)fi->f_di.i_size) {
		if (0 != (rc = ext2_buf_read_file(dir_nas, &buf, &buf_size))) {
			goto out;
		}
		if (buf_size != fsi->s_block_size || buf_size == 0) {
			goto out;
		}

		dp = (struct ext2fs_direct *)buf;
		edp = (struct ext2fs_direct *)(buf + buf_size);

		for (; dp < edp;
		     dp = (void *)((char *)dp + fs2h16(dp->e2d_reclen))) {
			if (fs2h16(dp->e2d_reclen) <= 0) {
				goto out;
			}
			if (fs2h32(dp->e2d_ino) == 0) {
				continue;
			}
			/* set node type by bogus type*/
			ext2_set_node_type (&type, dp->e2d_type);
			/* set null determine name */
			name = (char *) &dp->e2d_name;
			*(name + fs2h16(dp->e2d_namlen)) = 0;

			vfs_get_path_by_node(dir_nas->node, full_path);
			strcat(full_path, "/");
			strcat(full_path, name);
			if(NULL == (node = vfs_add_path(full_path, NULL))) {
				goto out;
			}
			if(NULL == ext2_create_file(node->nas, dir_nas->fs)) {
				goto out;
			}
			node->type = type;

			if(node_is_directory(node) &&
			  (0 != strcmp(name, "."))	&& (0 != strcmp(name, ".."))) {
				ext2_mount_entry(node->nas);
			}
		}
		fi->f_seekp += buf_size;
	}

out:
	ext2_close(dir_nas);
	free(full_path);
	return 0;
}

static void wr_indir(struct buf *bp, int index, uint32_t block);
static int empty_indir(struct buf *, struct ext2_fs_info *);
static void zero_block(struct buf *bp);

int write_map(struct nas *nas, long position, uint32_t new_block, int op) {
	/* Write a new block into an inode.
	*
	* If op includes WMAP_FREE, free the block corresponding to that position
	* in the inode ('new_block' is ignored then). Also free the indirect block
	* if that was the last entry in the indirect block.
	* Also free the double/triple indirect block if that was the last entry in
	* the double/triple indirect block.
	* It's the only function which should take care about fi->i_blocks counter.
	*/
	int index1, index2, index3; /* indexes in single..triple indirect blocks */
	long excess, block_pos;
	char new_ind = 0, new_dbl = 0, new_triple = 0;
	int single = 0, triple = 0;
	uint32_t old_block = NO_BLOCK, b1 = NO_BLOCK, b2 = NO_BLOCK, b3 = NO_BLOCK;
	struct buf *bp = NULL,
			 *bp_dindir = NULL,
			 *bp_tindir = NULL;
	static char first_time = 1;
	static long addr_in_block;
	static long addr_in_block2;
	static long doub_ind_s;
	static long triple_ind_s;
	static long out_range_s;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	if (first_time) {
		addr_in_block = fsi->s_block_size / BLOCK_ADDRESS_BYTES;
		addr_in_block2 = addr_in_block * addr_in_block;
		doub_ind_s = EXT2_NDIR_BLOCKS + addr_in_block;
		triple_ind_s = doub_ind_s + addr_in_block2;
		out_range_s = triple_ind_s + addr_in_block2 * addr_in_block;
		first_time = 0;
	}

	block_pos = position / fsi->s_block_size; /* relative blk # in file */
	fi->i_dirt = IN_DIRTY;		/* inode will be changed */

	/* Is 'position' to be found in the inode itself? */
	if (block_pos < EXT2_NDIR_BLOCKS) {
		if (fi->f_di.i_block[block_pos] != NO_BLOCK && (op & WMAP_FREE)) {
			free_block(nas, fsi, fi->f_di.i_block[block_pos]);
			fi->f_di.i_block[block_pos] = NO_BLOCK;
			fi->f_di.i_blocks -= fsi->s_sectors_in_block;
		} else {
			fi->f_di.i_block[block_pos] = new_block;
			fi->f_di.i_blocks += fsi->s_sectors_in_block;
		}
		return 0;
	}

	/* It is not in the inode, so it must be single, double or triple indirect */
	if (block_pos < doub_ind_s) {
		b1 = fi->f_di.i_block[EXT2_NDIR_BLOCKS]; /* addr of single indirect block */
		index1 = block_pos - EXT2_NDIR_BLOCKS;
		single = 1;
	}
	else if (block_pos >= out_range_s) { /* TODO: do we need it? */
		return(EFBIG);
	}
	else {
		/* double or triple indirect block. At first if it's triple,
		 * find double indirect block.
		 */
		excess = block_pos - doub_ind_s;
		b2 = fi->f_di.i_block[EXT2_DIND_BLOCK];
		if (block_pos >= triple_ind_s) {
			b3 = fi->f_di.i_block[EXT2_TIND_BLOCK];
			if (b3 == NO_BLOCK && !(op & WMAP_FREE)) {
			/* Create triple indirect block. */
				if ( (b3 = alloc_block(nas, fi->i_bsearch) ) == NO_BLOCK) {
					//ext2_debug("failed to allocate tblock near %d\n", fi->i_block[0]);
					return ENOSPC;
				}
				fi->f_di.i_block[EXT2_TIND_BLOCK] = b3;
				fi->f_di.i_blocks += fsi->s_sectors_in_block;
				new_triple = 1;
			}
			/* 'b3' is block number for triple indirect block, either old
			 * or newly created.
			 * If there wasn't one and WMAP_FREE is set, 'b3' is NO_BLOCK.
			 */
			if (b3 == NO_BLOCK && (op & WMAP_FREE)) {
				/* WMAP_FREE and no triple indirect block - then no
				* double and single indirect blocks either.
				*/
				b1 = b2 = NO_BLOCK;
			}
			else {
				//bp_tindir = get_block(fi->i_dev, b3, (new_triple ? NO_READ : NORMAL));
				ext2_read_sector(nas, (char *) bp_tindir->data, 1, b3);
				if (new_triple) {
					zero_block(bp_tindir);
					lmfs_markdirty(bp_tindir);
				}
				excess = block_pos - triple_ind_s;
				index3 = excess / addr_in_block2;
				b2 = rd_indir(bp_tindir, index3);
				excess = excess % addr_in_block2;
			}
			triple = 1;
		}

		if (b2 == NO_BLOCK && !(op & WMAP_FREE)) {
			/* Create the double indirect block. */
			if ( (b2 = alloc_block(nas, fi->i_bsearch) ) == NO_BLOCK) {
				/* Release triple ind blk. */
				//put_block(bp_tindir, INDIRECT_BLOCK);
				return(ENOSPC);
			}
			if (triple) {
				wr_indir(bp_tindir, index3, b2);  /* update triple indir */
				lmfs_markdirty(bp_tindir);
			} else {
				fi->f_di.i_block[EXT2_DIND_BLOCK] = b2;
			}
			fi->f_di.i_blocks += fsi->s_sectors_in_block;
			new_dbl = 1; /* set flag for later */
		}

		/* 'b2' is block number for double indirect block, either old
		 * or newly created.
		 * If there wasn't one and WMAP_FREE is set, 'b2' is NO_BLOCK.
		 */
		if (b2 == NO_BLOCK && (op & WMAP_FREE)) {
		/* WMAP_FREE and no double indirect block - then no
		 * single indirect block either.
		 */
			b1 = NO_BLOCK;
		}
		else {
			//bp_dindir = get_block(fi->i_dev, b2, (new_dbl ? NO_READ : NORMAL));
			ext2_read_sector(nas, (char *) bp_dindir->data, 1, b2);
			if (new_dbl) {
				zero_block(bp_dindir);
				lmfs_markdirty(bp_dindir);
			}
			index2 = excess / addr_in_block;
			b1 = rd_indir(bp_dindir, index2);
			index1 = excess % addr_in_block;
		}
		single = 0;
	}

  /* b1 is now single indirect block or NO_BLOCK; 'index' is index.
   * We have to create the indirect block if it's NO_BLOCK. Unless
   * we're freing (WMAP_FREE).
   */
	if (b1 == NO_BLOCK && !(op & WMAP_FREE)) {
		if ( (b1 = alloc_block(nas, fi->i_bsearch) ) == NO_BLOCK) {
			/* Release dbl and triple indirect blks. */
			//put_block(bp_dindir, INDIRECT_BLOCK);
			//put_block(bp_tindir, INDIRECT_BLOCK);
			//ext2_debug("failed to allocate dblock near %d\n", fi->i_block[0]);
			return ENOSPC;
		}
		if (single) {
			fi->f_di.i_block[EXT2_NDIR_BLOCKS] = b1; /* update inode single indirect */
		}
		else {
			wr_indir(bp_dindir, index2, b1);  /* update dbl indir */
			lmfs_markdirty(bp_dindir);
		}
		fi->f_di.i_blocks += fsi->s_sectors_in_block;
		new_ind = 1;
	}

	/* b1 is indirect block's number (unless it's NO_BLOCK when we're
	* freeing).
	*/
	if (b1 != NO_BLOCK) {
		//bp = get_block(fi->i_dev, b1, (new_ind ? NO_READ : NORMAL) );
		ext2_read_sector(nas, (char *) bp->data, 1, b1);
		if (new_ind) {
			zero_block(bp);
		}
		if (op & WMAP_FREE) {
			if ((old_block = rd_indir(bp, index1)) != NO_BLOCK) {
				free_block(nas, fsi, old_block);
				fi->f_di.i_blocks -= fsi->s_sectors_in_block;
				wr_indir(bp, index1, NO_BLOCK);
			}

			/* Last reference in the indirect block gone? Then
			 * free the indirect block.
			 */
			if (empty_indir(bp, fsi)) {
				free_block(nas, fsi, b1);
				fi->f_di.i_blocks -= fsi->s_sectors_in_block;
				b1 = NO_BLOCK;
				/* Update the reference to the indirect block to
				 * NO_BLOCK - in the double indirect block if there
				 * is one, otherwise in the inode directly.
				 */
				if (single) {
					fi->f_di.i_block[EXT2_NDIR_BLOCKS] = b1;
				} else {
					wr_indir(bp_dindir, index2, b1);
					lmfs_markdirty(bp_dindir);
				}
			}
		}
		else {
			wr_indir(bp, index1, new_block);
			fi->f_di.i_blocks += fsi->s_sectors_in_block;
		}
		/* b1 equals NO_BLOCK only when we are freeing up the indirect block. */
		if(b1 == NO_BLOCK) {
			lmfs_markclean(bp);
		}
		else {
			lmfs_markdirty(bp);
		}
		//put_block(bp, INDIRECT_BLOCK);
	}

	/* If the single indirect block isn't there (or was just freed),
	* see if we have to keep the double indirect block, if any.
	* If we don't have to keep it, don't bother writing it out.
	*/
	if (b1 == NO_BLOCK && !single && b2 != NO_BLOCK &&
	 empty_indir(bp_dindir, fsi)) {
		lmfs_markclean(bp_dindir);
		free_block(nas, fsi, b2);
		fi->f_di.i_blocks -= fsi->s_sectors_in_block;
		b2 = NO_BLOCK;
		if (triple) {
			wr_indir(bp_tindir, index3, b2);  /* update triple indir */
			lmfs_markdirty(bp_tindir);
		}
		else {
			fi->f_di.i_block[EXT2_DIND_BLOCK] = b2;
		}
	}
	/* If the double indirect block isn't there (or was just freed),
	* see if we have to keep the triple indirect block, if any.
	* If we don't have to keep it, don't bother writing it out.
	*/
	if (b2 == NO_BLOCK && triple && b3 != NO_BLOCK &&
	 empty_indir(bp_tindir, fsi)) {
		lmfs_markclean(bp_tindir);
		free_block(nas, fsi, b3);
		fi->f_di.i_blocks -= fsi->s_sectors_in_block;
		fi->f_di.i_block[EXT2_TIND_BLOCK] = NO_BLOCK;
	}

	//put_block(bp_dindir, INDIRECT_BLOCK);	/* release double indirect blk */
	//put_block(bp_tindir, INDIRECT_BLOCK);	/* release triple indirect blk */

	return 0;
}

static void wr_indir(struct buf *bp, int index, uint32_t block) {
	/* Given a pointer to an indirect block, write one entry. */

	/* write a block into an indirect block */
	b_ind(bp)[index] = block; //conv4(le_CPU, block);
}

static int empty_indir(struct buf *bp, struct ext2_fs_info *sb) {
	/* Return nonzero if the indirect block pointed to by bp contains
	* only NO_BLOCK entries.
	*/
	long addr_in_block;
	int i;

	addr_in_block = sb->s_block_size / 4; /* 4 bytes per addr */
	for(i = 0; i < addr_in_block; i++) {
		if(b_ind(bp)[i] != NO_BLOCK) {
			return 0;
		}
	}
	return 1;
}

uint32_t read_map(struct nas *nas, uint32_t position) {  /* position in file whose blk wanted */
	/* Given an inode and a position within the corresponding file, locate the
	 * block number in which that position is to be found and return it.
	 */
	struct buf buff;
	struct buf *bp;
	int index;
	uint32_t b;
	unsigned long excess, block_pos;
	static char first_time = 1;
	static long addr_in_block;
	static long addr_in_block2;
	static long doub_ind_s;
	static long triple_ind_s;
	static long out_range_s;

	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	bp = &buff;

	if (first_time) {
		addr_in_block = fsi->s_block_size / BLOCK_ADDRESS_BYTES;
		addr_in_block2 = addr_in_block * addr_in_block;
		doub_ind_s = EXT2_NDIR_BLOCKS + addr_in_block;
		triple_ind_s = doub_ind_s + addr_in_block2;
		out_range_s = triple_ind_s + addr_in_block2 * addr_in_block;
		first_time = 0;
	}

	block_pos = position / fsi->s_block_size; /* relative blk # in file */

	/* Is 'position' to be found in the inode itself? */
	if (block_pos < EXT2_NDIR_BLOCKS) {
		return(fi->f_di.i_block[block_pos]);
	}

	/* It is not in the inode, so it must be single, double or triple indirect */
	if (block_pos < doub_ind_s) {
		b = fi->f_di.i_block[EXT2_NDIR_BLOCKS]; /* address of single indirect block */
		index = block_pos - EXT2_NDIR_BLOCKS;
	}
	else if (block_pos >= out_range_s) { /* TODO: do we need it? */
		return(NO_BLOCK);
	}
	else {
		/* double or triple indirect block. At first if it's triple,
		 * find double indirect block.
		 */
		excess = block_pos - doub_ind_s;
		b = fi->f_di.i_block[EXT2_DIND_BLOCK];
		if (block_pos >= triple_ind_s) {
			b = fi->f_di.i_block[EXT2_TIND_BLOCK];
			if (b == NO_BLOCK) return(NO_BLOCK);
			ext2_read_sector(nas, (char *) fi->f_buf, 1, b);
			excess = block_pos - triple_ind_s;
			index = excess / addr_in_block2;
			b = rd_indir(bp, index);	/* num of double ind block */
			//put_block(bp, INDIRECT_BLOCK);	/* release triple ind block */
			excess = excess % addr_in_block2;
		}
		if (b == NO_BLOCK) {
			return(NO_BLOCK);
		}
		//bp = get_block(fi->i_dev, b, NORMAL);	/* get double indirect block */
		ext2_read_sector(nas, (char *) fi->f_buf, 1, b);
		index = excess / addr_in_block;
		b = rd_indir(bp, index);	/* num of single ind block */
		//put_block(bp, INDIRECT_BLOCK);	/* release double ind block */
		index = excess % addr_in_block;	/* index into single ind blk */
  }
  if (b == NO_BLOCK) {
	  return(NO_BLOCK);
  }
  //bp = get_block(fi->i_dev, b, NORMAL);
  ext2_read_sector(nas, (char *) fi->f_buf, 1, b);
  b = rd_indir(bp, index);
  //put_block(bp, INDIRECT_BLOCK);	/* release single ind block */

  return(b);
}


struct buf *new_block(struct nas *nas, long position) {
	// Acquire a new block and return a pointer to it.
	struct buf *bp;
	int r;
	uint32_t b;
	uint32_t goal;
	long position_diff;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	// Is another block available? //
	if ((b = read_map(nas, position)) == NO_BLOCK) {
		/* Check if this position follows last allocated
		 * block.
		 */
		goal = NO_BLOCK;
		if (fi->i_last_pos_bl_alloc != 0) {
			position_diff = position - fi->i_last_pos_bl_alloc;
			if (fi->i_bsearch == 0) {
				// Should never happen, but not critical //
				//ext2_debug("warning, i_bsearch is 0, while i_last_pos_bl_alloc is not!");
			}
			if (position_diff <= fsi->s_block_size) {
				goal = fi->i_bsearch + 1;
			} else {
				/* Non-sequential write operation,
				 * disable preallocation
				 * for this inode.
				 */
				fi->i_preallocation = 0;
				discard_preallocated_blocks(nas);
			}
		}

		if ( (b = alloc_block(nas, goal) ) == NO_BLOCK) {
			errno = ENOSPC;
			return(NULL);
		}
		if ( (r = write_map(nas, position, b, 0)) != 0) {
			free_block(nas, fsi, b);
			errno = r;
			//ext2_debug("write_map failed\n");
			return(NULL);
		}
		fi->i_last_pos_bl_alloc = position;
		if (0 == position) {
			/* fi->i_last_pos_bl_alloc points to the block position,
			 * and zero indicates first usage, thus just increment.
			 */
			fi->i_last_pos_bl_alloc++;
		}
	}

	//bp = get_block(fi->i_dev, b, NO_READ);
	ext2_read_sector(nas, fi->f_buf, 1, fsbtodb(fsi,b));
	/*TODO *///zero_block(bp);
	bp = NULL;
	return bp;
}


static void zero_block(struct buf *bp) {
	/* Zero a block. */
	memset(b_data(bp), 0, (size_t) lmfs_bytes(bp));
	lmfs_markdirty(bp);
}

static struct ext2_file_info *new_node(struct nas *nas, struct nas * parents_nas);

static int ext2_create(struct nas *nas, struct nas * parents_nas) {
	struct ext2_file_info *fi;//, *dir_fi;
	struct ext2_fs_info *fsi;
	char dirpath[MAX_LENGTH_PATH_NAME];
	//int len;
	//dir_fi = parents_nas->fi->privdata;
	fi = nas->fi->privdata;

	fsi = parents_nas->fs->fsi;
	vfs_get_path_by_node(parents_nas->node, dirpath);
	path_cut_mount_dir(dirpath, fsi->mntto);

	ext2_open(parents_nas);

	/* Copy the last component (i.e., file name) */
	//len = fs_m_in.REQ_PATH_LEN; /* including trailing '\0' */
	//if (len > NAME_MAX + 1 || len > EXT2_NAME_MAX + 1)
	//return(ENAMETOOLONG);
	/*
	err_code = sys_safecopyfrom(VFS_PROC_NR, (cp_grant_id_t) fs_m_in.REQ_GRANT,
				  (vir_bytes) 0, (vir_bytes) lastc, (size_t) len);
	if (err_code != OK) return err_code;
	NUL(lastc, len, sizeof(lastc));
	*/
	/* Get last directory inode (i.e., directory that will hold the new inode)
	if ((dir_fi = get_inode(fs_dev, (ino_t) fs_m_in.REQ_INODE_NR)) == NULL)
	  return(ENOENT);
	*/

	/* alloc a block sized buffer used for all transfers */
	fi->f_buf = malloc(fsi->s_block_size);


	/* Create a new inode by calling new_node(). */
	//omode = 0;
	fi = new_node(nas, parents_nas);
	//r = err_code;

	/* If an error occurred, release inode. */
	if (fi != NULL) {
		//put_inode(dir_fi);
		//put_inode(fi);
		ext2_close(parents_nas);
		free(fi->f_buf);
		return 0;
	}

	/* Reply message
	fs_m_out.RES_INODE_NR = fi->i_num;
	fs_m_out.RES_MODE = fi->i_mode;
	fs_m_out.RES_FILE_SIZE_LO = fi->i_size;
	*/

	/* This values are needed for the execution
	fs_m_out.RES_UID = fi->i_uid;
	fs_m_out.RES_GID = fi->i_gid;
	*/

	/* Drop parent dir
	put_inode(dir_fi);
	*/
	free(fi->f_buf);
	return -1;
}

int fs_mknod(struct nas *nas, struct nas * parents_nas) {
  struct ext2_file_info *ip;//, *dir_fi;
  //char lastc[1000 + 1];
  //int len;

  /* Copy the last component and set up caller's user and group id
  len = fs_m_in.REQ_PATH_LEN; // including trailing '\0'
  if (len > NAME_MAX + 1 || len > EXT2_NAME_MAX + 1)
	return(ENAMETOOLONG);

  err_code = sys_safecopyfrom(VFS_PROC_NR, (cp_grant_id_t) fs_m_in.REQ_GRANT,
                             (vir_bytes) 0, (vir_bytes) lastc, (size_t) len);
  if (err_code != OK) return err_code;
  NUL(lastc, len, sizeof(lastc));
  */

  /* Get last directory inode
  if((dir_fi = get_inode(fs_dev, (ino_t) fs_m_in.REQ_INODE_NR)) == NULL)
	  return(ENOENT);
  */
  /* Try to create the new node */
  ip = new_node(nas, parents_nas);

  //put_inode(ip);
  //put_inode(dir_fi);
  if(NULL == ip) {
	  return -1;
  }
  return 0; //(err_code);
}

static int ext2_mkdir(struct nas *nas, struct nas *parents_nas) {
  int r1, r2;			/* status codes */
  ino_t dot, dotdot;		/* inode numbers for . and .. */
  struct ext2_file_info *fi, *dir_fi;
  //struct ext2_fs_info *fsi;
  //char lastc[1000 + 1];         /* last component */
  //int len;
  dir_fi = parents_nas->fi->privdata;
  fi = nas->fi->privdata;

  //fsi = parents_nas->fs->fsi;

  /* Copy the last component and set up caller's user and group id //
  len = fs_m_in.REQ_PATH_LEN; // including trailing '\0' //
  if (len > NAME_MAX + 1 || len > EXT2_NAME_MAX + 1)
	return(ENAMETOOLONG);
  err_code = sys_safecopyfrom(VFS_PROC_NR, (cp_grant_id_t) fs_m_in.REQ_GRANT,
			      (vir_bytes) 0, (vir_bytes) lastc, (phys_bytes) len);
  if(err_code != OK) return(err_code);
  NUL(lastc, len, sizeof(lastc));
*/

  /* Get last directory inode
  if((dir_fi = get_inode(fs_dev, (ino_t) fs_m_in.REQ_INODE_NR)) == NULL)
      return(ENOENT);
      */

  /* Next make the inode. If that fails, return error code. */
  dir_fi = NULL;
  fi = new_node(nas, parents_nas);

  if(fi == NULL) {
	  //put_inode(fi);		/* can't make dir: it already exists */
	  //put_inode(dir_fi);
	  return -1;
  }

  /* Get the inode numbers for . and .. to enter in the directory. */
  dotdot = dir_fi->i_num;	/* parent's inode number */
  dot = fi->i_num;		/* inode number of the new dir itself */

  /* Now make dir entries for . and .. unless the disk is completely full. */
  /* Use dot1 and dot2, so the mode of the directory isn't important. */
  //fi->i_mode = (mode_t) fs_m_in.REQ_MODE;	/* set mode */
  /* enter . in the new dir*/
  //r1 = search_dir(fi, dot1, &dot, ENTER, IGN_PERM, I_DIRECTORY);
  /* enter .. in the new dir */
  //r2 = search_dir(fi, dot2, &dotdot, ENTER, IGN_PERM, I_DIRECTORY);

  /* If both . and .. were successfully entered, increment the link counts. */
  r1 = r2 = 0;
  if (r1 == 0 && r2 == 0) {
	  /* Normal case.  It was possible to enter . and .. in the new dir. */
	  dir_fi->i_dirt = IN_DIRTY;	/* mark parent's inode as dirty */
  } else {
	  /* It was not possible to enter . or .. probably disk was full -
	   * links counts haven't been touched.
	  if (search_dir(dir_fi, lastc, NULL, DELETE, IGN_PERM, 0) != OK)
		  panic("Dir disappeared ", fi->i_num);
	  */	/* undo the increment done in new_node() */
	  if(dot == dotdot) {
		  ;
	  }
  }
  if(NULL == fi) {
	  return -1;
  }
  fi->i_dirt = IN_DIRTY;		/* either way, i_links_count has changed */

  //put_inode(dir_fi);		/* return the inode of the parent dir */
  //put_inode(fi);		/* return the inode of the newly made dir */
  return 0;		/* new_node() always sets 'err_code' */
}
/*
int fs_slink() {
  phys_bytes len;
  struct ext2_file_info *sip;            inode containing symbolic link
  struct ext2_file_info *dir_fi;          directory containing link
  register int r;               error code
  char string[NAME_MAX];        last component of the new dir's path name
  char* link_target_buf = NULL;        either sip->i_block or bp->b_data
  struct buf *bp = NULL;     disk buffer for link

  caller_uid = (uid_t) fs_m_in.REQ_UID;
  caller_gid = (gid_t) fs_m_in.REQ_GID;

   Copy the link name's last component
  len = fs_m_in.REQ_PATH_LEN;
  if (len > NAME_MAX || len > EXT2_NAME_MAX)
	return(ENAMETOOLONG);

  r = sys_safecopyfrom(VFS_PROC_NR, (cp_grant_id_t) fs_m_in.REQ_GRANT,
		       (vir_bytes) 0, (vir_bytes) string, (size_t) len);
  if (r != OK) return(r);
 NUL(string, len, sizeof(string));

  Temporarily open the dir.
  if( (dir_fi = get_inode(fs_dev, (ino_t) fs_m_in.REQ_INODE_NR)) == NULL)
	  return(EINVAL);

   Create the inode for the symlink.
  sip = new_node(dir_fi, string, (mode_t) (I_SYMBOLIC_LINK | RWX_MODES),
		   (block_t) 0);

   If we can then create fast symlink (store it in inode),
   * Otherwise allocate a disk block for the contents of the symlink and
   * copy contents of symlink (the name pointed to) into first disk block.
  if( (r = err_code) == OK) {
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
			lmfs_markdirty(bp);
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

	if(r != OK) {
		sip->i_links_count = NO_LINK;
		if (search_dir(dir_fi, string, NULL, DELETE, IGN_PERM, 0) != OK)
			panic("Symbolic link vanished");
	}
  }

   put_inode() accepts NULL as a noop, so the below are safe.
  put_inode(sip);
  put_inode(dir_fi);

  return(r);
}*/

static void wipe_inode(register struct ext2_file_info *fi) {
	/* Erase some fields in the ext2_file_info. This function is called from alloc_inode()
	* when a new ext2_file_info is to be allocated, and from truncate(), when an existing
	* ext2_file_info is to be truncated.
	*/
	register int i;

	fi->f_di.i_size = 0;
	fi->f_di.i_blocks = 0;
	fi->f_di.i_flags = 0;
	fi->f_di.i_faddr = 0;

	for (i = 0; i < EXT2_N_BLOCKS; i++) {
		fi->f_di.i_block[i] = NO_BLOCK;
	}

	fi->i_dirt = IN_DIRTY;
}

/*
 * Find first group which has free inode slot.
 */
static int find_group_any(struct ext2_fs_info *fsi) {
	int group, ngroups;
	struct ext2_gd *gd;

	group = fsi->s_igsearch;
	ngroups = fsi->s_groups_count;

	for (; group < ngroups; group++) {
		gd = get_group_desc(group, fsi);
		if (gd == NULL) {
			return -1;
		}
		if (gd->free_inodes_count) {
			fsi->s_igsearch = group;
			return group;
		}
	}
	return -1;
}

static void free_inode_bit(struct nas *nas, uint32_t bit_returned, int is_dir) {
	/* Return an inode by turning off its bitmap bit. */
	int group;		/* group number of bit_returned */
	int bit;		/* bit_returned number within its group */
	struct buf *bp, buff;
	struct ext2_gd *gd;
	struct ext2_fs_info *fsi;
	struct ext2_file_info *fi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	bp = &buff;
	bp->data = fi->f_buf;/* At first search group, to which bit_returned belongs to
	* and figure out in what word bit is stored.
	*/
	if (bit_returned > fsi->e2sb.s_inodes_count ||
		bit_returned < EXT2_FIRST_INO(&fsi->e2sb)) {
		return;
	}

	group = (bit_returned - 1) / fsi->e2sb.s_inodes_per_group;
	bit = (bit_returned - 1) % fsi->e2sb.s_inodes_per_group; /* index in bitmap */

	if(NULL == (gd = get_group_desc(group, fsi))) {
		return;
	}

	ext2_read_sector(nas, fi->f_buf, 1, gd->inode_bitmap);
	//bp = get_block(sp->s_dev, gd->inode_bitmap, NORMAL);

	if (unsetbit(b_bitmap(bp), bit)) {
		return;
	}

	lmfs_markdirty(bp);
	//put_block(bp, MAP_BLOCK);

	gd->free_inodes_count++;
	fsi->e2sb.s_free_inodes_count++;

	if (is_dir) {
		gd->used_dirs_count--;
	}

	group_descriptors_dirty = 1;

	if (group < fsi->s_igsearch) {
		fsi->s_igsearch = group;
	}
}

static uint32_t alloc_inode_bit(struct nas *nas, int is_dir) {	/* inode will be a directory if it is TRUE */
	int group;
	ino_t inumber = 0;
	uint32_t bit;
	struct buf *bp;
	struct buf buf;
	struct ext2_gd *gd;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	bp = &buf;
	bp->data = fi->f_buf;

	group = find_group_any(fsi);

	/* Check if we have a group where to allocate an ext2_file_info */
	if (group == -1) {
		return 0;	/* no bit could be allocated */
	}

	gd = get_group_desc(group, fsi);

	/* find_group_* should always return either a group with
	* a free ext2_file_info slot or -1, which we checked earlier.
	*/
	ext2_read_sector(nas, fi->f_buf, 1, fsbtodb(fsi, gd->inode_bitmap));
	bp->data = fi->f_buf;
	bit = setbit(b_bitmap(bp), fsi->e2sb.s_inodes_per_group, 0);

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

	lmfs_markdirty(bp);
	//put_block(bp, MAP_BLOCK);

	gd->free_inodes_count--;
	fsi->e2sb.s_free_inodes_count--;
	if (is_dir) {
		gd->used_dirs_count++;
	}

	group_descriptors_dirty = 1;

	return inumber;
}

void free_inode(struct nas *nas) {  /* ext2_file_info to free */
	/* Return an ext2_file_info to the pool of unallocated inodes. */
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;
	uint32_t b;
	//u16_t mode;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	b = fi->i_num;
	//mode = fi->f_di.i_mode;

	/* Locate the appropriate super_block. */
	ext2_read_sblock(nas);

	if (b <= 0 || b > fsi->e2sb.s_inodes_count) {
		return;
	}
	free_inode_bit(nas, b, node_is_directory(nas->node));
}

struct ext2_file_info *alloc_inode(struct nas *nas, struct nas *parents_nas) {
/* Allocate a free ext2_file_info on parent's dev, and return a pointer to it. */

	struct ext2_file_info *fi; //, *dir_fi;
	//struct ext2_fs_info *fsi;
	//int inumb;
	uint32_t b;
	fi = nas->fi->privdata;
	//fsi = nas->fs->fsi;
	/*bp = &buff;
	bp->data = fi->f_buf;*/

	//sp = get_super(parent->i_dev);    /* get pointer to super_block */
	ext2_read_sblock(nas);

	/* Acquire an inode from the bit map. */
	b = alloc_inode_bit(nas, node_is_directory(nas->node));
	if (0 == b) {
		return NULL;
	}

	//inumb = (int) b;        /* be careful not to pass unshort as param */

	/* Try to acquire a slot in the inode table. */
	//if ((rip = get_inode(NO_DEV, inumb)) == NULL) {
		/* No inode table slots available.  Free the inode just allocated. */
		//free_inode_bit(nas, b, node_is_directory(nas->node));
	//}
	//else {
		/* An ext2_file_info slot is available. Put the ext2_file_info just allocated into it. */
		/*fi->f_di.i_mode = bits;          set up RWX bits */
		/*fi->f_di.i_links_count = NO_LINK;  initial no links */

		/* Fields not cleared already are cleared in wipe_inode(). They have
		 * been put there because truncate() needs to clear the same fields if
		 * the file happens to be open while being truncated. It saves space
		 * not to repeat the code twice.
		 */
		wipe_inode(fi);
	//}

	return fi;
}



/* it's implemented very close to the linux' find_group_dir()
static int find_group_dir(struct ext2sb *sp)
{
  int avefreei = sp->s_free_inodes_count / sp->s_groups_count;
  struct ext2_gd *gd, *best_gd = NULL;
  int group, best_group = -1;

  for (group = 0; group < sp->s_groups_count; ++group) {
	gd = get_group_desc(group);
	if (gd == NULL)
		panic("can't get group_desc to alloc ext2_file_info");
	if (gd->free_inodes_count == 0)
		continue;
	if (gd->free_inodes_count < avefreei)
	continue;
	if (!best_gd ||
	     gd->free_blocks_count > best_gd->free_blocks_count) {
		best_gd = gd;
		best_group = group;
	}
  }

  return best_group; *//* group or -1 */
//}


void rw_inode(struct nas *nas, int rw_flag/* READING or WRITING */) {
	/* An entry in the inode table is to be copied to or from the disk. */

	struct buf *bp, buff;
	struct ext2_gd *gd;
	struct ext2fs_dinode *dip;
	unsigned int block_group_number;
	uint32_t b, offset;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;



	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	bp = &buff;
	bp->data = fi->f_buf;

	/* Get the block where the inode resides. */
	//sp = get_super(rip->i_dev);     /* get pointer to super block */
	ext2_read_sblock(nas);

	block_group_number = (fi->i_num - 1) / fsi->e2sb.s_inodes_per_group;

	if (NULL == (gd = get_group_desc(block_group_number, fsi))) {
		return;
	}

	offset = ((fi->i_num - 1) % fsi->e2sb.s_inodes_per_group) * EXT2_INODE_SIZE(&fsi->e2sb);
	/* offset requires shifting, since each block contains several inodes,
	* e.g. inode 2 is stored in bklock 0.
	*/
	b = (uint32_t) gd->inode_table + (offset >> fsi->s_blocksize_bits);
	//bp = get_block(rip->i_dev, b, NORMAL);
	ext2_read_sector(nas, (char *) bp->data, 1, b);

	offset &= (fsi->s_block_size - 1);
	dip = (struct ext2fs_dinode*) (b_data(bp) + offset);

	/* Do the read or write. */
	if (rw_flag) {
		lmfs_markdirty(bp);
	}

	memcpy(dip, &fi->f_di, sizeof(struct ext2fs_dinode));

	//put_block(bp, INODE_BLOCK);
	fi->i_dirt = IN_CLEAN;
}

int search_dir(struct nas *nas, char *string, ino_t *numb, int flag, int  ftype)
	/*fi;  ptr to inode for dir to search */
	/* string[NAME_MAX + 1];	  component to search for */
	/*ino_t *numb;			  pointer to inode number */
	/*int flag;			  LOOK_UP, ENTER, DELETE or IS_EMPTY */
	/*int ftype;			  used when ENTER and
				  * INCOMPAT_FILETYPE */
	{
	/* This function searches the directory whose inode is pointed to by 'ldip':
	* if (flag == ENTER)  enter 'string' in the directory with inode # '*numb';
	* if (flag == DELETE) delete 'string' from the directory;
	* if (flag == LOOK_UP) search for 'string' and return inode # in 'numb';
	* if (flag == IS_EMPTY) return OK if only . and .. in dir else ENOTEMPTY;
	*
	*    if 'string' is dot1 or dot2, no access permissions are checked.
	*/
	struct ext2fs_direct  *dp = NULL;
	struct ext2fs_direct  *prev_dp = NULL;
	struct buf *bp = NULL;
	int i, r, e_hit, t, match;
	//mode_t bits;
	uint32_t pos;
	unsigned new_slots;
	uint32_t b;
	int extended = 0;
	int required_space = 0;
	int string_len = 0;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	/* If 'fi' is not a pointer to a dir inode, error. */
	if(!node_is_directory(nas->node)) {
		return ENOTDIR;
	}
//	if ( (fi->f_di.i_mode & I_TYPE) != I_DIRECTORY)  {
//	return(ENOTDIR);
//	}

	r = 0;

//	if (flag != 0) {
//		bits = (flag == LOOK_UP ? X_BIT : W_BIT | X_BIT);
//
//		if (string == dot1 || string == dot2) {
//			if (flag != LOOK_UP) {
//				r = read_only(fi);
//			}
//			 /* only a writable device is required. */
//		}
//		else if(check_permissions) {
//			r = forbidden(fi, bits); /* check access permissions */
//		}
//	}
//	if (r != 0) return(r);

	new_slots = 0;
	e_hit = 0;
	match = 0;    	/* set when a string match occurs */
	pos = 0;

	if (flag == ENTER) {
		string_len = strlen(string);
		required_space = MIN_DIR_ENTRY_SIZE + string_len;
		required_space += (required_space & 0x03) == 0 ? 0 :
					 (DIR_ENTRY_ALIGN - (required_space & 0x03) );

		if (fi->i_last_dpos < fi->f_di.i_size &&
			fi->i_last_dentry_size <= required_space) {
			pos = fi->i_last_dpos;
		}
	}

	for (; pos < fi->f_di.i_size; pos += fsi->s_block_size) {
		b = read_map(nas, pos);	/* get block number */

		/* Since directories don't have holes, 'b' cannot be NO_BLOCK. */
		//bp = get_block(fi->i_dev, b, NORMAL);	/* get a dir block */
		ext2_read_sector(nas, fi->f_buf, 1, b);
		prev_dp = NULL; /* New block - new first dentry, so no prev. */

		if (bp == NO_BLOCK) {
			return -1;//panic("get_block returned NO_BLOCK");
		}

		assert(bp != NULL);

		/* Search a directory block.
		 * Note, we set prev_dp at the end of the loop.
		 */
		for (dp = (struct ext2fs_direct*) &b_data(bp);
			 CUR_DISC_DIR_POS(dp, &b_data(bp)) < fsi->s_block_size;
			 dp = NEXT_DISC_DIR_DESC(dp) ) {
			/* Match occurs if string found. */
			if (flag != ENTER && dp->e2d_ino != 0) {
				if (flag == IS_EMPTY) {
					/* If this test succeeds, dir is not empty. */
					if (strncmp(dp->e2d_name, ".", dp->e2d_namlen) != 0 &&
						strncmp(dp->e2d_name, "..", dp->e2d_namlen) != 0) {
						match = 1;
					}
				} else {
					if (strncmp(dp->e2d_name, string, dp->e2d_namlen) == 0) {
						match = 1;
					}
				}
			}

			if (match) {
				/* LOOK_UP or DELETE found what it wanted. */
				r = 0;
				if (flag == IS_EMPTY) {
					r = ENOTEMPTY;
				}
				else if (flag == DELETE) {
					if (dp->e2d_namlen >= sizeof(ino_t)) {
						/* Save d_ino for recovery. */
						t = dp->e2d_namlen - sizeof(ino_t);
						*((ino_t *) &dp->e2d_name[t]) = dp->e2d_ino;
					}
					dp->e2d_ino = 0;	/* erase entry */
					lmfs_markdirty(bp);

					/* If we don't support HTree (directory index),
					 * which is fully compatible ext2 feature,
					 * we should reset EXT2_INDEX_FL, when modify
				 * linked directory structure.
					 *
					 * @TODO: actually we could just reset it for
					 * each directory, but I added if() to not
					 * forget about it later, when add HTree
					 * support.
					 */
//					if (!HAS_COMPAT_FEATURE(&fsi->e2sb,
//								COMPAT_DIR_INDEX))
//						fi->i_flags &= ~EXT2_INDEX_FL;
					if (pos < fi->i_last_dpos) {
						fi->i_last_dpos = pos;
						fi->i_last_dentry_size = dp->e2d_reclen;
					}
					fi->i_dirt = IN_DIRTY;
					/* Now we have cleared dentry, if it's not
					 * the first one, merge it with previous one.
					 * Since we assume, that existing dentry must be
					 * correct, there is no way to spann a data block.
					 */
					if (prev_dp) {
						u16_t temp = prev_dp->e2d_reclen;
						temp += dp->e2d_reclen;
						prev_dp->e2d_reclen = temp;
					}
				}
				else {
					/* 'flag' is LOOK_UP */
					*numb = (ino_t) dp->e2d_ino;
				}
				//put_block(bp, DIRECTORY_BLOCK);
				return(r);
			}

			/* Check for free slot for the benefit of ENTER. */
		if (flag == ENTER && dp->e2d_ino == 0) {
				/* we found a free slot, check if it has enough space */
				if (required_space <= dp->e2d_reclen) {
					e_hit = 1;	/* we found a free slot */
					break;
				}
			}
			/* Can we shrink dentry? */
			if (flag == ENTER && required_space <= DIR_ENTRY_SHRINK(dp)) {
				/* Shrink directory and create empty slot, now
				 * dp->d_rec_len = DIR_ENTRY_ACTUAL_SIZE + DIR_ENTRY_SHRINK.
				 */
				int new_slot_size = dp->e2d_reclen;
				int actual_size = DIR_ENTRY_ACTUAL_SIZE(dp);
				new_slot_size -= actual_size;
				dp->e2d_reclen = actual_size;
				dp = NEXT_DISC_DIR_DESC(dp);
				dp->e2d_reclen = new_slot_size;
				/* if we fail before writing real ino */
				dp->e2d_ino = 0;
				lmfs_markdirty(bp);
				e_hit = 1;	/* we found a free slot */
				break;
			}

			prev_dp = dp;
		}

		/* The whole block has been searched or ENTER has a free slot. */
		if (e_hit) break;	/* e_hit set if ENTER can be performed now */
		//put_block(bp, DIRECTORY_BLOCK); /* otherwise, continue searching dir */
	}

	/* The whole directory has now been searched. */
	if (flag != ENTER) {
		return(flag == IS_EMPTY ? 0 : ENOENT);
	}

	/* When ENTER next time, start searching for free slot from
	* i_last_dpos. It gives solid performance improvement.
	*/
	fi->i_last_dpos = pos;
	fi->i_last_dentry_size = required_space;

	/* This call is for ENTER.  If no free slot has been found so far, try to
	* extend directory.
	*/
	if (e_hit == 0) { /* directory is full and no room left in last block */
		new_slots++;		/* increase directory size by 1 entry */
		if ( (bp = new_block(nas, fi->f_di.i_size)) == NULL) {
			return -1;
		}
		dp = (struct ext2fs_direct*) &b_data(bp);
		dp->e2d_reclen = fsi->s_block_size;
		dp->e2d_namlen = DIR_ENTRY_MAX_NAME_LEN(dp); /* for failure */
		extended = 1;
	}

	/* 'bp' now points to a directory block with space. 'dp' points to slot. */
	dp->e2d_namlen = string_len;
	for (i = 0; i < MAX_LENGTH_PATH_NAME && i < dp->e2d_namlen && string[i]; i++) {
		dp->e2d_name[i] = string[i];
	}
	dp->e2d_ino = (int)  *numb;
	if (HAS_INCOMPAT_FEATURE(&fsi->e2sb, EXT2F_INCOMPAT_FILETYPE)) {
		/* Convert ftype (from inode.i_mode) to dp->d_file_type */
		if (ftype == S_IFREG){
			dp->e2d_type = EXT2_FT_REG_FILE;
		}
		else if (ftype == S_IFDIR) {
			dp->e2d_type = EXT2_FT_DIR;
		}
		else if (ftype == S_IFLNK) {
			dp->e2d_type = EXT2_FT_SYMLINK;
		}
		else if (ftype == S_IFBLK) {
			dp->e2d_type = EXT2_FT_BLKDEV;
		}
		else if (ftype == S_IFCHR) {
			dp->e2d_type = EXT2_FT_CHRDEV;
		}
		else if (ftype == S_IFIFO) {
			dp->e2d_type = EXT2_FT_FIFO;
		}
		else {
			dp->e2d_type = EXT2_FT_UNKNOWN;
		}
	}
	lmfs_markdirty(bp);
	//put_block(bp, DIRECTORY_BLOCK);
	fi->i_dirt = IN_DIRTY;

	if (new_slots == 1) {
		fi->f_di.i_size += (uint32_t) dp->e2d_reclen;
		/* Send the change to disk if the directory is extended. */
		if (extended) {
			rw_inode(nas, 1);
		}
	}
	return 0;
}

static struct ext2_file_info *new_node(struct nas *nas, struct nas * parents_nas) {
	/* New_node() is called by fs_open(), fs_mknod(), and fs_mkdir().
	* In all cases it allocates a new inode, makes a directory entry for it in
	* the dir_fi directory with string name, and initializes it.
	* It returns a pointer to the ext2_file_info if it can do this;
	* otherwise it returns NULL.  It always sets 'err_code'
	* to an appropriate value (OK or an error code).
	*/
	struct ext2_file_info *fi;
	//struct ext2_file_info *dir_fi;
	//register int r;

	/* Get final component of the path. */
	/*TODO*///fi = advance(dir_fi, string, IGN_PERM);

	if (node_is_directory(nas->node)) {
		/* New entry is a directory, alas we can't give it a ".." */
		//put_inode(fi);
		//err_code = EMLINK;
		return NULL;
	}

	/* Last path component does not exist.  Make new directory entry. */
	if (NULL == (fi = alloc_inode(nas, parents_nas))) {
		/* Can't creat new inode: out of inodes. */
		return NULL;
	}

	/* Force inode to the disk before making directory entry to make
	* the system more robust in the face of a crash: an inode with
	* no directory entry is much better than the opposite.
	*/
	rw_inode(nas, 1);		/* force inode to disk now */

	/* New inode acquired.  Try to make directory entry. */
	if (0 != search_dir(parents_nas, NULL/*string*/, &fi->i_num, ENTER,
		fi->f_di.i_mode & S_IFMT)) {
		fi->i_dirt = IN_DIRTY;	 /*dirty inodes are written out*/
		//put_inode(fi);	 /*this call frees the inode*/
		return NULL;
	}
	/* The caller has to return the directory ext2_file_info (*dir_fi).  */
	return fi;
}

DECLARE_FILE_SYSTEM_DRIVER(ext2_drv);
