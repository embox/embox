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
static int ext2_ls(struct nas *nas, /*const char *pattern,
					void (*funcp)(char* arg),*/ char* path);

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

static int ext2_read_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct ext2_fs_info *fsi;
	fsi = nas->fs->fsi;

	return block_dev_read(nas->fs->bdev, (char *) buffer,
			count * fsi->e2fs_bsize, sector);
}

/*
static int ext2_write_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct ext2_fs_info *fsi;
	fsi = nas->fs->fsi;

	return block_dev_write(nas->fsi->bdev, (char *) buffer,
			count * fsi->e2fs_bsize, sector);
}
*/

/*
 * Calculate indirect block levels.
 *
 * We note that the number of indirect blocks is always
 * a power of 2.  This lets us use shifts and masks instead
 * of divide and remainder and avoinds pulling in the
 * 64bit division routine into the boot code.
 */
static int ext2_culc_shift (struct ext2_file_info *fi,
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
	link_len = fi->f_di.e2di_size;
	len = strlen(*cp);

	if ((link_len + len > MAXPATHLEN) ||
		(++nlinks > MAXSYMLINKS)) {
		return ENOENT;
	}

	memmove(&namebuf[link_len], cp, len + 1);

	if (link_len < EXT2_MAXSYMLINKLEN) {
		memcpy(namebuf, fi->f_di.e2di_blocks, link_len);
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

/*
 * file_operation
 */
static int ext2fs_open(struct node *node, struct file_desc *desc, int flags) {
	char path[MAX_LENGTH_PATH_NAME];
	const char *cp, *ncp;
	uint32_t inumber, parent_inumber;

	int rc;
	struct nas *nas;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	nas = node->nas;
	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	/* prepare full path into this filesystem */
	vfs_get_path_by_node(node, path);
	path_cut_mount_dir(path, fsi->mntto);

	/* allocate file system specific data structure */

	/* alloc a block sized buffer used for all transfers */
	fi->f_buf = malloc(fsi->e2fs_bsize);

	/* read group descriptor blocks */
	fsi->e2fs_gd = malloc(sizeof(struct ext2_gd) * fsi->e2fs_ncg);
	if(0 != (rc = ext2_read_gdblock(nas))) {
		goto out;
	}

	if(0 != ext2_culc_shift(fi, fsi)) {
		goto out;
	}

	inumber = EXT2_ROOTINO;
	if ((rc = ext2_read_inode(nas, inumber)) != 0) {
		goto out;
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
		if ((fi->f_di.e2di_mode & S_IFMT) != S_IFDIR) {
			rc = ENOTDIR;
			goto out;
		}

		/* Get next component of path name */
		ncp = cp;
		while ((*cp != '\0') && (*cp != '/')) {
			cp++;
		}

		/*
		 * Look up component in current directory.
		 * Save directory inumber in case we find a
		 * symbolic link.
		 */
		parent_inumber = inumber;
		rc = ext2_search_directory(nas, ncp, cp - ncp, &inumber);
		if (rc) {
			goto out;
		}

		/* Open next component */
		if (0 != (rc = ext2_read_inode(nas, inumber))) {
			goto out;
		}

		/* Check for symbolic link */
		if ((fi->f_di.e2di_mode & S_IFMT) == S_IFLNK) {
			if(ext2_read_symlink (nas, parent_inumber, &cp)) {
				goto out;
			}
		}
	}

	fi->f_seekp = 0;		/* reset seek pointer */

out:
	if (rc) {
		ext2fs_close(desc);
	}

	return rc;
}

static int ext2fs_close(struct file_desc *desc) {
	struct nas *nas;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	if(NULL == desc) {
		return 0;
	}
	nas = desc->node->nas;
	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	if (NULL != fsi) {
		if (fsi->e2fs_gd) {
			free(fsi->e2fs_gd);
		}
		//pool_free(&ext2_fs_pool, fsi);
	}

	if (NULL != fi) {
		if (NULL != fi->f_buf) {
			free(fi->f_buf);
		}
		//pool_free(&ext2_file_pool, fi);
	}

	return 0;
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
		if (fi->f_seekp >= (long)fi->f_di.e2di_size) {
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


static size_t ext2fs_write(struct file_desc *desc, void *buf, size_t size) {
	/*struct nas *nas;
	struct ext2_file_info *f;

	nas = desc->node->nas;
	f = nas->fi->privdata;
	 not impl */
	return EROFS;
}

static int ext2fs_ioctl(struct file_desc *desc, int request, va_list args) {
	return 0;
}

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

static ext2_file_info_t *ext2_create_file(struct nas *nas) {
	ext2_file_info_t *fi;

	if(NULL == (fi = pool_alloc(&ext2_file_pool))) {
		return NULL;
	}

	nas->fi->ni.size = fi->f_seekp = 0;

	return fi;
}

static int ext2fs_create(struct node *parent_node, struct node *node) {
	struct nas *nas, *parents_nas;

	parents_nas = parent_node->nas;
	nas = node->nas;
	nas->fs = parents_nas->fs;
	/* don't need create fi for directory - take root node fi */
	if(NULL == (nas->fi->privdata = ext2_create_file(nas))) {
		return -ENOMEM;
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
	}

	if (node_is_directory(node)) {
		pool_free(&ext2_file_pool, fi);
	}

	/* root node - have fi, but haven't index*/
	if(0 == strcmp((const char *) path, (const char *) fsi->mntto)){
		pool_free(&ext2_fs_pool, fsi);
		pool_free(&ext2_file_pool, fi);
	}

	vfs_del_leaf(node);

	return 0;
}

static int ext2fs_format(void *dev) {
	/*node_t *dev_node;
	struct nas *dev_nas;
	struct node_fi *dev_fi;

	if (NULL == (dev_node = dev)) {
		return -ENODEV;//device not found /
	}

	if(!node_is_block_dev(dev_node)) {
		return -ENODEV;
	}
	dev_nas = dev_node->nas;
	dev_fi = dev_nas->fi;
	*/

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
	fsi->e2fs_bsize = SBSIZE;
	if(0 != ext2_read_sblock(dir_nas)) {
		return -1;
	}

	ext2fs_open(dir_nas->node, NULL, 0);
	ext2_ls(dir_nas, "/1/11");

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
	if (rsize != fsi->e2fs_bsize) {
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
		*disk_block_p = fs2h32(fi->f_di.e2di_blocks[file_block]);
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
		if (level > NIADDR * fi->f_nishift)
			/* Block number too high */
			return EFBIG;
		file_block -= (int32_t)1 << level;
	}

	ind_block_num =
	    fs2h32(fi->f_di.e2di_blocks[NDADDR + (level / fi->f_nishift - 1)]);

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

		if (rsize != fsi->e2fs_bsize)
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
	block_size = fsi->e2fs_bsize;	/* no fragment */

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
	if (*size_p > fi->f_di.e2di_size - fi->f_seekp) {
		*size_p = fi->f_di.e2di_size - fi->f_seekp;
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
	while (fi->f_seekp < (long)fi->f_di.e2di_size) {
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
	struct ext2fs *ext2fs;

	fsi = nas->fs->fsi;
	ext2fs = &fsi->e2fs;

	if(SBSIZE != ext2_read_sector(nas, (char *)sbbuf, 1, SBOFF / SECTOR_SIZE)) {
		return EIO;
	}

	e2fs_sbload((void *)sbbuf, ext2fs);
	if (ext2fs->e2fs_magic != E2FS_MAGIC)
		return EINVAL;
	if (ext2fs->e2fs_rev > E2FS_REV1 ||
	    (ext2fs->e2fs_rev == E2FS_REV1 &&
	     (ext2fs->e2fs_first_ino != EXT2_FIRSTINO ||
	     (ext2fs->e2fs_inode_size != 128 && ext2fs->e2fs_inode_size != 256) ||
	      ext2fs->e2fs_features_incompat & ~EXT2F_INCOMPAT_SUPP))) {
		return ENODEV;
	}

	e2fs_sbload((void *)sbbuf, &fsi->e2fs);
	/* compute in-memory ext2_fs_info values */
	fsi->e2fs_ncg =
	    howmany(fsi->e2fs.e2fs_bcount - fsi->e2fs.e2fs_first_dblock,
	    fsi->e2fs.e2fs_bpg);
	/* XXX assume hw bsize = 512 */
	fsi->e2fs_fsbtodb = fsi->e2fs.e2fs_log_bsize + 1;
	fsi->e2fs_bsize = MINBSIZE << fsi->e2fs.e2fs_log_bsize;
	fsi->e2fs_bshift = LOG_MINBSIZE + fsi->e2fs.e2fs_log_bsize;
	fsi->e2fs_qbmask = fsi->e2fs_bsize - 1;
	fsi->e2fs_bmask = ~fsi->e2fs_qbmask;
	fsi->e2fs_ngdb =
	    howmany(fsi->e2fs_ncg, fsi->e2fs_bsize / sizeof(struct ext2_gd));
	fsi->e2fs_ipb = fsi->e2fs_bsize / ext2fs->e2fs_inode_size;
	fsi->e2fs_itpg = fsi->e2fs.e2fs_ipg / fsi->e2fs_ipb;

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

	gdpb = fsi->e2fs_bsize / sizeof(struct ext2_gd);

	for (i = 0; i < fsi->e2fs_ngdb; i++) {
		rsize = ext2_read_sector(nas, fi->f_buf, 1,
				fsbtodb(fsi, fsi->e2fs.e2fs_first_dblock + 1 + i));
		if (rsize != fsi->e2fs_bsize) {
			return EIO;
		}

		e2fs_cgload((struct ext2_gd *)fi->f_buf,
		    &fsi->e2fs_gd[i * gdpb],
		    (i == (fsi->e2fs_ngdb - 1)) ?
		    (fsi->e2fs_ncg - gdpb * i) * sizeof(struct ext2_gd):
		    fsi->e2fs_bsize);
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
/* ??? */

static const char *const typestr[] = {
	"unknown",
	"REG",
	"DIR",
	"CHR",
	"BLK",
	"FIFO",
	"SOCK",
	"LNK"
};

typedef struct entry_t entry_t;
struct entry_t {
	entry_t	*e_next;
	uint32_t	e_ino;
	uint8_t	e_type;
	char	e_name[1];
};

#define NELEM(x) (sizeof (x) / sizeof(*x))


/*
static int ext2_fn_match(const char *fname, const char *pattern) {
	char fc, pc;

	do {
		fc = *fname++;
		pc = *pattern++;
		if (!fc && !pc) {
			return 1;
		}
		if (pc == '?' && fc) {
			pc = fc;
		}
	} while (fc == pc);

	if (pc != '*') {
		return 0;
	}
	//
	 * Too hard (and unnecessary really) too check for "*?name" etc....
	 * "**" will look for a '*' and "*?" a '?'
	 //
	pc = *pattern++;
	if (!pc) {
		return 1;
	}
	while ((fname = strchr(fname, pc))) {
		if (ext2_fn_match(++fname, pattern)) {
			return 1;
		}
	}
	return 0;
}
*/

static int ext2_ls(struct nas *nas, char* path) {
	char *buf;
	const char *t;
	size_t buf_size;
	int rc;
	entry_t	*names = 0, *n, **np;
	entry_t *p_names;
	struct ext2fs_direct  *dp, *edp;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;
	char *point, *full_path;
	node_t *node;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	full_path = malloc(MAX_LENGTH_PATH_NAME);

	fi->f_seekp = 0;
	while (fi->f_seekp < (long)fi->f_di.e2di_size) {
		if (0 != (rc = ext2_buf_read_file(nas, &buf, &buf_size))) {
			goto out;
		}
		if (buf_size != fsi->e2fs_bsize || buf_size == 0) {
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
			/* set null determine name */
			point = (char *) &dp->e2d_name;
			*(point + fs2h16(dp->e2d_namlen)) = 0;

			if (dp->e2d_type >= NELEM(typestr) ||
			    !(t = typestr[dp->e2d_type])) {
				/*
				 * This does not handle "old" filesystems properly. On little
				 * endian machines, we get a bogus type name if the namlen
				 * matches a valid type identifier. We could check if we read
				 *  namlen "0" and handle this case specially, if there were
				 *  a pressing need...
				 */
				printf("bad dir entry\n");
				goto out;
			}
			/*if (pattern && !ext2_fn_match(dp->e2d_name, pattern)) {
				continue;
			}*/

			vfs_get_path_by_node(nas->node, full_path);
			strcat(full_path, "/");
			strcat(full_path, point);
			if(NULL == (node = vfs_add_path(full_path, NULL))) {
				goto out;
			}
			ext2fs_create(nas->node, node);
			ext2_set_node_type (&node->type, dp->e2d_type);

			if(node_is_directory(node) &&
			  (0 != strcmp(point, "."))	&& (0 != strcmp(point, ".."))) {
				ext2fs_open(node, NULL, 0);
				path_cut_mount_dir(full_path, fsi->mntto);
				ext2_ls(node->nas, full_path);
			}

			n = malloc(sizeof *n + strlen(dp->e2d_name));
			if (!n) {
				printf("%d: %s (%s)\n",
					fs2h32(dp->e2d_ino), dp->e2d_name, t);
				continue;
			}
			n->e_ino = fs2h32(dp->e2d_ino);
			n->e_type = dp->e2d_type;
			strcpy(n->e_name, dp->e2d_name);
			for (np = &names; *np; np = &(*np)->e_next) {
				if (strcmp(n->e_name, (*np)->e_name) < 0) {
					break;
				}
			}
			n->e_next = *np;
			*np = n;
		}
		fi->f_seekp += buf_size;
	}

	if (names) {
		p_names = names;
		do {
			n = p_names;
			printf("%d: %s (%s)\n",
			n->e_ino, n->e_name, typestr[n->e_type]);
			p_names = n->e_next;
		} while (p_names);
	} else {
		printf("not found\n");
	}
out:
	if (names) {
		do {
			n = names;
			names = n->e_next;
			free(n);
		} while (names);
	}
	free(full_path);
	return 0;
}


/*
 * byte swap functions for big endian machines
 * (ext2fs is always little endian)
 *
 * XXX: We should use src/sys/ufs/ext2fs/ext2fs_bswap.c
 */

/* These functions are only needed if native byte order is not big endian */
#if defined(__BIG_ENDIAN)
void 1e2fs_sb_bswap(struct ext2fs *old, struct ext2fs *new) {

	/* preserve unused fields */
	memcpy(new, old, sizeof(struct ext2fs));
	new->e2fs_icount	=	bswap32(old->e2fs_icount);
	new->e2fs_bcount	=	bswap32(old->e2fs_bcount);
	new->e2fs_rbcount	=	bswap32(old->e2fs_rbcount);
	new->e2fs_fbcount	=	bswap32(old->e2fs_fbcount);
	new->e2fs_ficount	=	bswap32(old->e2fs_ficount);
	new->e2fs_first_dblock	=	bswap32(old->e2fs_first_dblock);
	new->e2fs_log_bsize	=	bswap32(old->e2fs_log_bsize);
	new->e2fs_fsize		=	bswap32(old->e2fs_fsize);
	new->e2fs_bpg		=	bswap32(old->e2fs_bpg);
	new->e2fs_fpg		=	bswap32(old->e2fs_fpg);
	new->e2fs_ipg		=	bswap32(old->e2fs_ipg);
	new->e2fs_mtime		=	bswap32(old->e2fs_mtime);
	new->e2fs_wtime		=	bswap32(old->e2fs_wtime);
	new->e2fs_mnt_count	=	bswap16(old->e2fs_mnt_count);
	new->e2fs_max_mnt_count	=	bswap16(old->e2fs_max_mnt_count);
	new->e2fs_magic		=	bswap16(old->e2fs_magic);
	new->e2fs_state		=	bswap16(old->e2fs_state);
	new->e2fs_beh		=	bswap16(old->e2fs_beh);
	new->e2fs_minrev	=	bswap16(old->e2fs_minrev);
	new->e2fs_lastfsck	=	bswap32(old->e2fs_lastfsck);
	new->e2fs_fsckintv	=	bswap32(old->e2fs_fsckintv);
	new->e2fs_creator	=	bswap32(old->e2fs_creator);
	new->e2fs_rev		=	bswap32(old->e2fs_rev);
	new->e2fs_ruid		=	bswap16(old->e2fs_ruid);
	new->e2fs_rgid		=	bswap16(old->e2fs_rgid);
	new->e2fs_first_ino	=	bswap32(old->e2fs_first_ino);
	new->e2fs_inode_size	=	bswap16(old->e2fs_inode_size);
	new->e2fs_block_group_nr =	bswap16(old->e2fs_block_group_nr);
	new->e2fs_features_compat =	bswap32(old->e2fs_features_compat);
	new->e2fs_features_incompat =	bswap32(old->e2fs_features_incompat);
	new->e2fs_features_rocompat =	bswap32(old->e2fs_features_rocompat);
	new->e2fs_algo		=	bswap32(old->e2fs_algo);
	new->e2fs_reserved_ngdb	=	bswap16(old->e2fs_reserved_ngdb);


void e2fs_cg_bswap(struct ext2_gd *old, struct ext2_gd *new, int size)
{
	int i;

	for (i = 0; i < (size / sizeof(struct ext2_gd)); i++) {
		new[i].ext2bgd_b_bitmap	= bswap32(old[i].ext2bgd_b_bitmap);
		new[i].ext2bgd_i_bitmap	= bswap32(old[i].ext2bgd_i_bitmap);
		new[i].ext2bgd_i_tables	= bswap32(old[i].ext2bgd_i_tables);
		new[i].ext2bgd_nbfree	= bswap16(old[i].ext2bgd_nbfree);
		new[i].ext2bgd_nifree	= bswap16(old[i].ext2bgd_nifree);
		new[i].ext2bgd_ndirs	= bswap16(old[i].ext2bgd_ndirs);
	}
}

void e2fs_i_bswap(struct ext2fs_dinode *old, struct ext2fs_dinode *new)
{

	new->e2di_mode		=	bswap16(old->e2di_mode);
	new->e2di_uid		=	bswap16(old->e2di_uid);
	new->e2di_gid		=	bswap16(old->e2di_gid);
	new->e2di_nlink		=	bswap16(old->e2di_nlink);
	new->e2di_size		=	bswap32(old->e2di_size);
	new->e2di_atime		=	bswap32(old->e2di_atime);
	new->e2di_ctime		=	bswap32(old->e2di_ctime);
	new->e2di_mtime		=	bswap32(old->e2di_mtime);
	new->e2di_dtime		=	bswap32(old->e2di_dtime);
	new->e2di_nblock	=	bswap32(old->e2di_nblock);
	new->e2di_flags		=	bswap32(old->e2di_flags);
	new->e2di_gen		=	bswap32(old->e2di_gen);
	new->e2di_facl		=	bswap32(old->e2di_facl);
	new->e2di_dacl		=	bswap32(old->e2di_dacl);
	new->e2di_faddr		=	bswap32(old->e2di_faddr);
	memcpy(&new->e2di_blocks[0], &old->e2di_blocks[0],
	    (NDADDR + NIADDR) * sizeof(uint32_t));
}
#endif

#if 0
void ext2_dump_sblock(struct ext2_fs_info *fsi) {

	printf("fsi->e2fs.e2fs_bcount = %u\n", fsi->e2fs.e2fs_bcount);
	printf("fsi->e2fs.e2fs_first_dblock = %u\n", fsi->e2fs.e2fs_first_dblock);
	printf("fsi->e2fs.e2fs_log_bsize = %u\n", fsi->e2fs.e2fs_log_bsize);
	printf("fsi->e2fs.e2fs_bpg = %u\n", fsi->e2fs.e2fs_bpg);
	printf("fsi->e2fs.e2fs_ipg = %u\n", fsi->e2fs.e2fs_ipg);
	printf("fsi->e2fs.e2fs_magic = 0x%x\n", fsi->e2fs.e2fs_magic);
	printf("fsi->e2fs.e2fs_rev = %u\n", fsi->e2fs.e2fs_rev);

	if (fsi->e2fs.e2fs_rev == E2FS_REV1) {
		printf("fsi->e2fs.e2fs_first_ino = %u\n",
		    fsi->e2fs.e2fs_first_ino);
		printf("fsi->e2fs.e2fs_inode_size = %u\n",
		    fsi->e2fs.e2fs_inode_size);
		printf("fsi->e2fs.e2fs_features_compat = %u\n",
		    fsi->e2fs.e2fs_features_compat);
		printf("fsi->e2fs.e2fs_features_incompat = %u\n",
		    fsi->e2fs.e2fs_features_incompat);
		printf("fsi->e2fs.e2fs_features_rocompat = %u\n",
		    fsi->e2fs.e2fs_features_rocompat);
		printf("fsi->e2fs.e2fs_reserved_ngdb = %u\n",
		    fsi->e2fs.e2fs_reserved_ngdb);
	}

	printf("fsi->e2fs_bsize = %u\n", fsi->e2fs_bsize);
	printf("fsi->e2fs_fsbtodb = %u\n", fsi->e2fs_fsbtodb);
	printf("fsi->e2fs_ncg = %u\n", fsi->e2fs_ncg);
	printf("fsi->e2fs_ngdb = %u\n", fsi->e2fs_ngdb);
	printf("fsi->e2fs_ipb = %u\n", fsi->e2fs_ipb);
	printf("fsi->e2fs_itpg = %u\n", fsi->e2fs_itpg);
}
#endif

DECLARE_FILE_SYSTEM_DRIVER(ext2_drv);

