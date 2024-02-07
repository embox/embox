/**
 * @file
 * @brief
 *
 * @date 20.08.2012
 * @author Andrey Gazukin
 */

/**
 * cdfs.c
 *
 * ISO-9660 CD-ROM Filesystem
 *
 * Copyright (C) 2002 Michael Ringgaard. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <ctype.h>

#include <util/err.h>
#include <util/math.h>

#include <fs/inode.h>
#include <fs/vfs.h>
#include <fs/hlpr_path.h>
#include <fs/super_block.h>
#include <fs/file_desc.h>
#include <fs/fs_driver.h>
#include <fs/inode_operation.h>
#include <fs/dir_context.h>

#include <fs/iso9660.h>

#include <drivers/block_dev.h>
#include <mem/misc/pool.h>
#include <mem/sysmalloc.h>

#include <kernel/time/ktime.h>
#include <kernel/time/clock_source.h>

#include <framework/mod/options.h>

/* cdfs filesystem description pool */
POOL_DEF(cdfs_fs_pool, struct cdfs_fs_info, OPTION_GET(NUMBER,cdfs_descriptor_quantity));

/* cdfs file description pool */
POOL_DEF(cdfs_file_pool, struct cdfs_file_info, OPTION_GET(NUMBER,inode_quantity));

static int cdfs_open(struct inode *node, char *name);
static int cdfs_iterate(struct inode *next, char *next_name, struct inode *parent, struct dir_ctx *dir_ctx);

static int cdfs_isonum_711(unsigned char *p) {
  return p[0];
}

static int cdfs_isonum_731(unsigned char *p) {
  return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

static int cdfs_isonum_733(unsigned char *p) {
  return cdfs_isonum_731(p);
}

static int cdfs_fnmatch(cdfs_t *cdfs, char *fn1, int len1, char *fn2, int len2) {
	wchar_t *wfn2;
	int wlen2;

	if (cdfs->joliet) {
		wfn2 = (wchar_t *) fn2;
		wlen2 = len2 / 2;
		if (wlen2 > 1 && ntohs(wfn2[wlen2 - 2]) == ';') {
			wlen2 -= 2;
		}
		if (wlen2 > 0 && ntohs(wfn2[wlen2 - 1]) == '.') {
			wlen2 -= 1;
		}
		if (len1 != wlen2) {
			return 0;
		}
		while (len1--) {
			if (*fn1++ != ntohs(*wfn2++)) {
				return 0;
			}
		}
		return 1;
	}
	else {
		if (len2 > 1 && fn2[len2 - 2] == ';') {
			len2 -= 2;
		}
		if (len2 > 0 && fn2[len2 - 1] == '.') {
			len2 -= 1;
		}
		if (len1 != len2) {
			return 0;
		}
		while (len1--) {
			if (*fn1++ != *fn2++) {
				return 0;
			}
		}
		return 1;
	}
}

static int cdfs_read_path_table(cdfs_t *cdfs, iso_volume_descriptor_t *vd) {
	struct block_dev_cache *cache;
	unsigned char *pt;
	int ptblk;
	int ptlen;
	int ptpos;
	int n;
	iso_pathtable_record_t *pathrec;
	int namelen;
	int reclen;
	char name[128];

	/* Determine size and location of path table and allocate buffer */
	ptlen = cdfs_isonum_733(vd->path_table_size);
	ptblk = cdfs_isonum_731(vd->type_l_path_table);
	cdfs->path_table_buffer = sysmalloc(ptlen);
	if (!cdfs->path_table_buffer) {
		return -ENOMEM;
	}

	/* Read L path table into buffer */
	ptpos = 0;
	while (ptpos < ptlen) {
		cache = block_dev_cached_read(cdfs->bdev, ptblk++);
		if (!cache) {
			return -EIO;
		}

		if (ptlen - ptpos > CDFS_BLOCKSIZE) {
			memcpy(cdfs->path_table_buffer + ptpos, cache->data, CDFS_BLOCKSIZE);
			ptpos += CDFS_BLOCKSIZE;
		}
		else {
			memcpy(cdfs->path_table_buffer + ptpos, cache->data, ptlen - ptpos);
			ptpos = ptlen;
		}
	}

	/*
	 * Determine number of records in pathtable
	 * Path table records are indexed from 1 (first entry not used)
	 */
	pt = cdfs->path_table_buffer;
	n = 1;
	while (pt < cdfs->path_table_buffer + ptlen) {
		pathrec = (iso_pathtable_record_t *) pt;
		namelen = pathrec->length;
		reclen = sizeof(iso_pathtable_record_t) + namelen + (namelen & 1);

		n++;
		pt += reclen;
		memcpy(name, (char *)(pathrec + sizeof(iso_pathtable_record_t)), namelen);
	}
	if((0 == name[0]) && (0 == name[1])) {
		name[0] = 33;
	}

	cdfs->path_table_records = n;

	/* Allocate path table */
	cdfs->path_table = (iso_pathtable_record_t **)
							sysmalloc(cdfs->path_table_records *
							sizeof(iso_pathtable_record_t **));
	if (!cdfs->path_table) {
		return -ENOMEM;
	}
	cdfs->path_table[0] = NULL;

	/* Setup pointers into path table buffer */
	pt = cdfs->path_table_buffer;
	for (n = 1; n < cdfs->path_table_records; n++) {
		pathrec = (iso_pathtable_record_t *) pt;
		namelen = pathrec->length;
		reclen = sizeof(iso_pathtable_record_t) + namelen + (namelen & 1);

		cdfs->path_table[n] = pathrec;
		pt += reclen;
	}

	return 0;
}

static int cdfs_find_dir(cdfs_t *cdfs, char *name, int len) {
	char *p;
	int l;
	int dir = 2;
	int parent = 1;
	char upper_name[NAME_MAX];


	for(int i = 0; name[i]; i++){
		upper_name[i] = toupper(name[i]);
	}

	name = upper_name;

	while (1) {
		/* Skip path separator */
		if (*name == PS1 || *name == PS2) {
			name++;
			len--;
		}
		if (len == 0) {
			return parent;
		}

		/* Find next part of name */
		p = name;
		l = 0;
		while (l < len && *p != PS1 && *p != PS2) {
			l++;
			p++;
		}

		/* Find directory for next name part */
		while (dir < cdfs->path_table_records) {
			if (cdfs->path_table[dir]->parent != parent) {
				return -ENOENT;
			}
			if (cdfs_fnmatch(cdfs, name, l,
				cdfs->path_table[dir]->name, cdfs->path_table[dir]->length)) {
				break;
			}
			dir++;
		}

		/* If we reached the end of the path table the directory does not exist */
		if (dir == cdfs->path_table_records) {
			return -ENOENT;
		}

		/* If we have parsed the whole name return the directory number */
		if (l == len) {
			return dir;
		}

		/* Go forward in path table until first entry for directory found */
		parent = dir;
		while (dir < cdfs->path_table_records) {
			if (cdfs->path_table[dir]->parent == parent) {
				break;
			}
			dir++;
		}

		/* Prepare for next name part */
		name = p;
		len -= l;
	}
	return -ENOENT;
}

static int cdfs_find_in_dir(cdfs_t *cdfs, int dir, char *name, int len, iso_directory_record_t **dirrec) {
	struct block_dev_cache *cache;
	char *p;
	iso_directory_record_t *rec;
	int blk;
	int left;
	int reclen;
	int namelen;

	/* The first two directory records are . (current) and .. (parent) */
	blk = cdfs->path_table[dir]->extent;
	cache = block_dev_cached_read(cdfs->bdev, blk++);
	if (!cache) {
		return -EIO;
	}

	/* Get length of directory from the first record */
	p = cache->data;
	rec = (iso_directory_record_t *) p;
	left = cdfs_isonum_733(rec->size);

	/* Find named entry in directory */
	while (left > 0) {
		/*
		 * Read next block if all records in current block has been read
		 * Directory records never cross block boundaries
		 */
		if (p >= cache->data + CDFS_BLOCKSIZE) {
			if (p > cache->data + CDFS_BLOCKSIZE) {
				return -EIO;
			}
			cache = block_dev_cached_read(cdfs->bdev, blk++);
			if (!cache) {
				return -EIO;
			}
			p = cache->data;
		}

		/* Check for match */
		rec = (iso_directory_record_t *) p;
		reclen = cdfs_isonum_711(rec->length);
		namelen = cdfs_isonum_711(rec->name_len);

		if (reclen > 0) {
			if (cdfs_fnmatch(cdfs, name, len, (char *) rec->name, namelen)) {
				*dirrec = rec;
				return 0;
			}

			/* Skip to next record */
			p += reclen;
			left -= reclen;
		}
		else {
			/* Skip to next block */
			left -= (cache->data + CDFS_BLOCKSIZE) - p;
			p = cache->data + CDFS_BLOCKSIZE;
		}
	}

	return -ENOENT;
}

static int cdfs_find_file(cdfs_t *cdfs, char *name, int len, iso_directory_record_t **rec) {
	int dir;
	int split;
	int n;
	struct block_dev_cache *cache;
	iso_volume_descriptor_t *vd;

	/* If root get directory record from volume descriptor */
	if (len == 0 || (0 == strcmp(name, "/"))) {
		cache = block_dev_cached_read(cdfs->bdev, cdfs->vdblk);
		if (!cache) {
			return -EIO;
		}
		vd = (iso_volume_descriptor_t *) (cache)->data;
		*rec = (iso_directory_record_t *) vd->root_directory_record;
		return 1;
	}

	/* Split path into directory part and file name part */
	split = -1;
	for (n = 0; n < len; n++) {
		if (name[n] == PS1 || name[n] == PS2) {
			split = n;
		}
	}

	/* Find directly if file located in root directory */
	if (split == -1) {
		return cdfs_find_in_dir(cdfs, 1, name, len, rec);
	}

	/* Locate directory */
	dir = cdfs_find_dir(cdfs, name, split + 1);
	if (dir < 0) {
		return dir;
	}

	/* Find filename in directory */
	return cdfs_find_in_dir(cdfs, dir, name + split + 1,
								  len - split - 1, rec);
}

static time_t cdfs_isodate(unsigned char *date)
{
	static struct tm tm;

	memset(&tm, 0, sizeof tm);
	tm.tm_year = date[0];
	tm.tm_mon = date[1] - 1;
	tm.tm_mday = date[2];
	tm.tm_hour = date[3];
	tm.tm_min = date[4];
	tm.tm_sec = date[5];
	tm.tm_min += (*(char *) &date[6]) * 15;

	return (time_t) &tm; /*mktime(&tm); */
}

int cdfs_mount(struct inode *root_node)
{
	cdfs_t *cdfs;
	int rc;
	int blk;
	struct block_dev_cache *cache;
	iso_volume_descriptor_t *vd;
	int type;
	unsigned char *esc;
	struct cdfs_fs_info *fsi;

	fsi = root_node->i_sb->sb_data;

	/* Check block size */
	if (CDFS_BLOCKSIZE != block_dev_block_size(root_node->i_sb->bdev)) {
		return -ENXIO;
	}

	/* Allocate file system */
	cdfs = (cdfs_t *) sysmalloc(sizeof(cdfs_t));
	memset(cdfs, 0, sizeof(cdfs_t));
	cdfs->bdev = root_node->i_sb->bdev;
	cdfs->blks = block_dev_size(root_node->i_sb->bdev);
	if (cdfs->blks < 0) {
		return cdfs->blks;
	}

	/* Allocate cache */
	if (NULL == block_dev_cache_init(root_node->i_sb->bdev, CDFS_POOLDEPTH)) {
		return -ENOMEM;
	}

	/* Read volume descriptors */
	cdfs->vdblk = 0;
	blk = 0x8000/CDFS_BLOCKSIZE;
	while (1) {
		cache  = block_dev_cached_read(root_node->i_sb->bdev, blk);
		if (!cache) {
			return -EIO;
		}
		vd = (iso_volume_descriptor_t *) cache->data;

		type = cdfs_isonum_711(vd->type);
		esc = vd->escape_sequences;

		if (memcmp(vd->id, "CD001", 5) != 0) {
			/*free_buffer_pool(cdfs->cache); */
			//block_dev_close(cdfs->bdev);
			sysfree(cdfs);
			return -EIO;
		}

		if (cdfs->vdblk == 0 && type == ISO_VD_PRIMARY) {
			cdfs->vdblk = blk;
		}
		else if (type == ISO_VD_SUPPLEMENTAL &&
				 esc[0] == 0x25 && esc[1] == 0x2F &&
				 (esc[2] == 0x40 || esc[2] == 0x43 || esc[2] == 0x45)) {
			cdfs->vdblk = blk;
			cdfs->joliet = 1;
		}

		if (type == ISO_VD_END) {
			break;
		}
		blk++;
	}
	if (cdfs->vdblk == 0) {
		return -EIO;
	}

	/* Initialize filesystem from selected volume descriptor and read path table */
	cache  = block_dev_cached_read(root_node->i_sb->bdev, cdfs->vdblk);
	if (!cache) {
		return -EIO;
	}
	vd = (iso_volume_descriptor_t *) cache->data;

	cdfs->volblks = cdfs_isonum_733(vd->volume_space_size);

	rc = cdfs_read_path_table(cdfs, vd);
	if (rc < 0) {
		return rc;
	}

	/* Device mounted successfully */
	fsi->data = cdfs;

	return 0;
}

static void cdfs_free_fs(struct super_block *sb);
static int cdfs_clean_sb(struct super_block *sb) {
	struct cdfs_fs_info *fsi = sb->sb_data;
	cdfs_t *cdfs = (cdfs_t *) fsi->data;

	/* Close device */
	//block_dev_close(fs->bdev);

	/* Deallocate file system */
	if (cdfs->path_table_buffer) {
		sysfree(cdfs->path_table_buffer);
	}
	if (cdfs->path_table) {
		sysfree(cdfs->path_table);
	}
	sysfree(cdfs);

	cdfs_free_fs(sb);

	pool_free(&cdfs_file_pool, inode_priv(sb->sb_root));

	return 0;
}

/* int cdfs_statfs(struct cdfs_fs_info *fsi, statfs_t *cache) { */
/* 	cdfs_t *cdfs = (cdfs_t *) fsi->data; */

/* 	cache->bsize = CDFS_BLOCKSIZE; */
/* 	cache->iosize = CDFS_BLOCKSIZE; */
/* 	cache->blocks = cdfs->volblks; */
/* 	cache->bfree = 0; */
/* 	cache->files = -1; */
/* 	cache->ffree = 0; */
/* 	/\*cache->cachesize = cdfs->cache->poolsize * CDFS_BLOCKSIZE; *\/ */

/* 	return 0; */
/* } */

static int cdfs_open(struct inode *node, char *name) {
	cdfs_t *cdfs;
	iso_directory_record_t *rec;
	time_t date;
	int size;
	int extent;
	int flags;
	int rc;
	struct cdfs_file_info *fi;
	struct cdfs_fs_info *fsi;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;;

	cdfs = (cdfs_t *) fsi->data;


	/* Check open mode */
	if (fi->flags & (O_CREAT | O_TRUNC | O_APPEND)) {
		return -EROFS;
	}

	for(int i = 0; name[i]; i++){
		name[i] = toupper(name[i]);
	}

	/* Locate file in file system */
	rc = cdfs_find_file(cdfs, name, strlen(name), &rec);
	if (rc < 0) {
		return rc;
	}

	flags = cdfs_isonum_711(rec->flags);
	extent = cdfs_isonum_733(rec->extent);
	date = cdfs_isodate(rec->date);
	size = cdfs_isonum_733(rec->size);

	fi->extent = extent;
	fi->date = date;
	fi->size = size;
	if (flags & 2) {
		//fi->flags |= F_DIR;
		fi->flags |= S_IFDIR;
	}

//	fi->mode = S_IFREG | S_IRUSR | S_IXUSR |
//	   S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
	return 0;
}

static int cdfs_read(struct inode *node, void *data, size_t size, off64_t pos) {
	size_t read;
	size_t count;
	size_t left;
	char *p;
	int iblock;
	int start;
	int blk;
	struct block_dev_cache *cache;
	struct cdfs_file_info *fi;
	struct cdfs_fs_info *fsi;
	cdfs_t *cdfs;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;
	cdfs = (cdfs_t *) fsi->data;

	read = 0;
	p = (char *) data;
	while (pos < fi->size && size > 0) {
		iblock = (int) pos / CDFS_BLOCKSIZE;
		start = (int) pos % CDFS_BLOCKSIZE;

		count = CDFS_BLOCKSIZE - start;
		if (count > size) {
			count = size;
		}

		left = fi->size - (int) pos;
		if (count > left) {
			count = left;
		}
		if (count <= 0) {
			break;
		}

		blk = fi->extent + iblock;

		if (fi->flags & O_DIRECT) {
			if (start != 0 || count != CDFS_BLOCKSIZE) {
				return read;
			}
			if (block_dev_read(cdfs->bdev, p, count, blk) != (int) count) {
				return read;
			}
		}
		else {
			cache = block_dev_cached_read(cdfs->bdev, blk);
			if (!cache) {
				return -EIO;
			}
			memcpy(p, cache->data + start, count);
		}

		pos += count;
		p += count;
		read += count;
		size -= count;
	}

	return read;
}

/*
static int cdfs_opendir(struct inode *dir_node, char *name) {
	cdfs_t *cdfs;
	iso_directory_record_t *rec;
	cdfs_file_t *cdfile;
	time_t date;
	int size;
	int extent;
	int flags;
	int rc;
	struct cdfs_file_info *fi;

	fi = inode_priv(dir->node);
	cdfs = (cdfs_t *) dir_node->i_sb->data;

	// Locate directory
	rc = cdfs_find_file(cdfs, name, strlen(name), &rec);
	if (rc < 0) {
		return rc;
	}

	flags = cdfs_isonum_711(rec->flags);
	extent = cdfs_isonum_733(rec->extent);
	date = cdfs_isodate(rec->date);
	size = cdfs_isonum_733(rec->size);

	if (!(flags & 2)) {
		return -ENOTDIR;
	}

	// Allocate and initialize file block
	cdfile = (cdfs_file_t *) sysmalloc(sizeof(cdfs_file_t));
	if (!cdfile) {
		return -ENOMEM;
	}
	cdfile->extent = extent;
	cdfile->date = date;
	cdfile->size = size;

	fi->data = cdfile;
	fi->mode = S_IFDIR | S_IRUSR | S_IXUSR | S_IRGRP |
				 S_IXGRP | S_IROTH | S_IXOTH;
	return 0;
}

static int cdfs_readdir(struct inode *node, direntry_t *dirp, int count) {
	cdfs_file_t *cdfile;
	cdfs_t *cdfs;
	iso_directory_record_t *rec;
	struct block_dev_cache *cache;
	int namelen;
	int reclen;
	int blkleft;
	char *name;
	wchar_t *wname;
	struct cdfs_file_info *fi;

	fi = inode_priv(node);
	cdfile = (cdfs_file_t *) fi->data;
	cdfs = (cdfs_t *) node->i_sb->data;

  blkagain:
	if (count != 1) {
		return -EINVAL;
	}
	if (fi->pos >= cdfile->size) {
		return 0;
	}

	// Get directory block
	cache = block_dev_cached_read(cdfs->bdev, cdfile->extent +
			(int) fi->pos / CDFS_BLOCKSIZE);
	if (!cache) {
		return -EIO;
	}

	// Locate directory record
  recagain:
	rec = (iso_directory_record_t *) (cache->data +
			(int) fi->pos % CDFS_BLOCKSIZE);
	reclen = cdfs_isonum_711(rec->length);
	namelen = cdfs_isonum_711(rec->name_len);

	// Check for no more records in block
	if (reclen == 0) {
		blkleft = CDFS_BLOCKSIZE - ((int) fi->pos % CDFS_BLOCKSIZE);
		fi->pos += blkleft;
		goto blkagain;
	}

	 // Check for . and .. entries /
	if (namelen == 1 && (rec->name[0] == 0 || rec->name[0] == 1)) {
		fi->pos += reclen;
		goto recagain;
	}

	// Get info from directory record /
	dirp->ino = cdfs_isonum_733(rec->extent);
	dirp->reclen = sizeof(direntry_t) - PATH_MAX + namelen + 1;
	if (cdfs->joliet) {
		namelen /= 2;
		wname = (wchar_t *) rec->name;
		if (namelen > 1 && ntohs(wname[namelen - 2]) == ';') {
			namelen -= 2;
		}
		if (namelen > 0 && ntohs(wname[namelen - 1]) == '.') {
			namelen -= 1;
		}

		dirp->namelen = namelen;
		for (int n = 0; n < namelen; n++) {
			dirp->name[n] = (char) ntohs(wname[n]);
		}
		dirp->name[namelen] = 0;
	}
	else {
		name = (char *) rec->name;
		if (namelen > 1 && name[namelen - 2] == ';') namelen -= 2;
		if (namelen > 0 && name[namelen - 1] == '.') namelen -= 1;

		dirp->namelen = namelen;
		memcpy(dirp->name, name, namelen);
		dirp->name[namelen] = 0;
	}

	fi->pos += reclen;
	return 1;
}

void cdfs_init(void) {

	return;
}
*/

/* File operations */
static struct idesc *cdfsfs_open(struct inode *node, struct idesc *idesc, int __oflag);
static int    cdfsfs_close(struct file_desc *desc);
static size_t cdfsfs_read(struct file_desc *desc, void *buf, size_t size);

static struct file_operations cdfsfs_fop = {
	.open = cdfsfs_open,
	.close = cdfsfs_close,
	.read = cdfsfs_read,
};

static struct idesc *cdfsfs_open(struct inode *node, struct idesc *idesc, int __oflag) {
	char path [PATH_MAX + 1];
	int res;

	vfs_get_relative_path(node, path, PATH_MAX);

	res = cdfs_open(node, path + 1);
	if (res) {
		return err_ptr(-res);
	}
	return idesc;
}

static int cdfsfs_close(struct file_desc *desc) {
	struct cdfs_file_info *fi;

	fi = inode_priv(desc->f_inode);
	fi->pos = 0;

	return 0;
}

static size_t cdfsfs_read(struct file_desc *desc, void *buf, size_t size) {
	int rezult;
	struct cdfs_file_info *fi;

	fi = inode_priv(desc->f_inode);

	rezult = cdfs_read(desc->f_inode, (void *) buf, size, fi->pos);
	fi->pos += rezult;

	return rezult;
}

/* File system operations*/
static int cdfs_fill_sb(struct super_block *sb, const char *source);
//static int cdfsfs_mount(struct super_block *sb, struct inode *dest);
static int cdfs_umount_entry(struct inode *node);

static struct fsop_desc cdfsfs_fsop = {
	//.mount = cdfsfs_mount,
	.umount_entry = cdfs_umount_entry,
};

static struct fs_driver cdfsfs_driver = {
	.name = "iso9660",
	.fill_sb = cdfs_fill_sb,
	.clean_sb = cdfs_clean_sb,
	.fsop = &cdfsfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(cdfsfs_driver);

static int cdfs_destroy_inode(struct inode *inode) {
	return 0;
}

static struct super_block_operations cdfs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = cdfs_destroy_inode,
};

struct inode_operations cdfs_iops = {
	.ino_iterate = cdfs_iterate,
};


static void cdfs_free_fs(struct super_block *sb) {
	struct cdfs_fs_info *fsi = sb->sb_data;

	if (NULL != fsi) {
		pool_free(&cdfs_fs_pool, fsi);
	}
}

static int cdfs_fill_node(struct inode* node, char *name, cdfs_t *cdfs, iso_directory_record_t *rec) {
	int flags;
	int namelen;
	struct cdfs_file_info *fi;

	namelen = cdfs_isonum_711(rec->name_len);
	flags = cdfs_isonum_711(rec->flags);

	if (cdfs->joliet) {
		wchar_t *wname;

		namelen /= 2;
		namelen = min(NAME_MAX, namelen);

		wname = (wchar_t *) rec->name;
		if (namelen > 1 && ntohs(wname[namelen - 2]) == ';') {
			namelen -= 2;
		}
		if (namelen > 0 && ntohs(wname[namelen - 1]) == '.') {
			namelen -= 1;
		}

		for (int n = 0; n < namelen; n++) {
			name[n] = (char) ntohs(wname[n]);
		}
	} else {
		namelen = min(NAME_MAX, namelen);
		if (namelen > 1 && rec->name[namelen - 2] == ';') {
			namelen -= 2;
		}
		if (namelen > 0 && rec->name[namelen - 1] == '.') {
			namelen -= 1;
		}
		memcpy(name, rec->name, namelen);
	}
	name[namelen] = 0;

	for(int i = 0; name[i]; i++){
		name[i] = tolower(name[i]);
	}

	fi = pool_alloc(&cdfs_file_pool);
	if (!fi) {
		return -ENOMEM;
	}

	/* if directory then not create node */
	if (flags & 2) {
		node->i_mode = S_IFDIR;
	} else {
		node->i_mode = S_IFREG;
	}

	fi->size = cdfs_isonum_733(rec->size);
	inode_priv_set(node, fi);
	inode_size_set(node, fi->size);
	return 0;
}

static int cdfs_iterate(struct inode *next, char *next_name, struct inode *parent, struct dir_ctx *dir_ctx) {
	int n;
	cdfs_t *cdfs;
	struct cdfs_fs_info *fsi;
	struct block_dev_cache *cache;
	int blk;
	char *p;
	iso_directory_record_t *rec;
	int left;
	int reclen;
	int idx = 0;

	fsi = parent->i_sb->sb_data;
	cdfs = fsi->data;

	if (0 == (int) (intptr_t) dir_ctx->fs_ctx) {
		dir_ctx->fs_ctx = (void*)(intptr_t)2;
	}

	n = cdfs_find_dir(cdfs, inode_name(parent), strlen(inode_name(parent)));

	/* The first two directory records are . (current) and .. (parent) */
	blk = cdfs->path_table[n]->extent;
	cache = block_dev_cached_read(cdfs->bdev, blk++);
	if (!cache) {
		return -1;
	}

	/* Get length of directory from the first record */
	p = cache->data;
	rec = (iso_directory_record_t *) p;
	left = cdfs_isonum_733(rec->size);

	/* Find named entry in directory */
	while (left > 0) {
		/*
		 * Read next block if all records in current block has been read
		 * Directory records never cross block boundaries
		 */
		if (p >= cache->data + CDFS_BLOCKSIZE) {
			if (p > cache->data + CDFS_BLOCKSIZE) {
				return -1;
			}
			cache = block_dev_cached_read(cdfs->bdev, blk++);
			if (!cache) {
				return -1;
			}
			p = cache->data;
		}

		/* Check for match */
		rec = (iso_directory_record_t *) p;
		reclen = cdfs_isonum_711(rec->length);

		if (reclen > 0) {

			if (idx++ < (int)(uintptr_t)dir_ctx->fs_ctx) {
			} else {
				cdfs_fill_node(next, next_name, cdfs, rec);
				dir_ctx->fs_ctx = (void *)(uintptr_t)idx;
				return 0;
			}
			/* Skip to next record */
			p += reclen;
			left -= reclen;
		}
		else {
			/* Skip to next block */
			left -= (cache->data + CDFS_BLOCKSIZE) - p;
			p = cache->data + CDFS_BLOCKSIZE;
		}
	}
	return -1;

}

static int cdfs_fill_sb(struct super_block *sb, const char *source) {
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
	if (NULL == (fsi = pool_alloc(&cdfs_fs_pool))) {
		return -ENOMEM;
	}
	memset(fsi, 0, sizeof(struct cdfs_fs_info));

	sb->sb_data = fsi;
	sb->sb_ops = &cdfs_sbops;
	sb->sb_iops = &cdfs_iops;
	sb->sb_fops = &cdfsfs_fop;

	dest = sb->sb_root;

	/* allocate this directory info */
	if(NULL == (fi = pool_alloc(&cdfs_file_pool))) {
		rc = -ENOMEM;
		goto error;
	}
	memset(fi, 0, sizeof(struct cdfs_file_info));

	inode_priv_set(dest, fi);

	if(0 == (rc = cdfs_mount(dest))) {
		return 0;
	}

	return 0;
error:
	cdfs_free_fs(sb);

	return rc;
}

#if 0
static int cdfsfs_mount(struct super_block *sb, struct inode *dest) {
#if 0
	struct cdfs_file_info *fi;
	int rc;

	/* allocate this directory info */
	if(NULL == (fi = pool_alloc(&cdfs_file_pool))) {
		rc = -ENOMEM;
		goto error;
	}
	memset(fi, 0, sizeof(struct cdfs_file_info));
	inode_priv_set(dest, fi);

	if(0 == (rc = cdfs_mount(dest))) {
		return 0;
	}

error:
	cdfs_free_fs(sb);

	return rc;
#endif
	return 0;
}
#endif

static int cdfs_umount_entry(struct inode *node) {
	//pool_free(&cdfs_file_pool, inode_priv(node));

	return 0;
}
