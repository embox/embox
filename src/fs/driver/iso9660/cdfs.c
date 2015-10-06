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

#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/hlpr_path.h>
#include <fs/file_system.h>
#include <fs/file_desc.h>
#include <fs/fs_driver.h>
#include <fs/file_operation.h>

#include <fs/iso9660.h>

#include <drivers/block_dev.h>
#include <mem/misc/pool.h>
#include <mem/sysmalloc.h>
#include <arpa/inet.h>

#include <kernel/time/ktime.h>
#include <kernel/time/clock_source.h>

#include <framework/mod/options.h>



/* cdfs filesystem description pool */
POOL_DEF(cdfs_fs_pool, struct cdfs_fs_info, OPTION_GET(NUMBER,cdfs_descriptor_quantity));

/* cdfs file description pool */
POOL_DEF(cdfs_file_pool, struct cdfs_file_info, OPTION_GET(NUMBER,inode_quantity));

static int cdfs_open(struct nas *nas, char *name);
static int cdfs_create_dir_entry (struct nas *parent_nas);
//static int cdfs_get_full_path(cdfs_t *cdfs, int numrec, char *path);

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
	block_dev_cache_t *cache;
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

static int cdfs_find_in_dir(cdfs_t *cdfs, int dir, char *name, int len, block_dev_cache_t **dirbuf, iso_directory_record_t **dirrec) {
	block_dev_cache_t *cache;
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
				*dirbuf = cache;
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

static int cdfs_find_file(cdfs_t *cdfs, char *name, int len,
				block_dev_cache_t **cache, iso_directory_record_t **rec) {
	int dir;
	int split;
	int n;
	iso_volume_descriptor_t *vd;

	/* If root get directory record from volume descriptor */
	if (len == 0) {
		*cache = block_dev_cached_read(cdfs->bdev, cdfs->vdblk);
		if (!*cache) {
			return -EIO;
		}
		vd = (iso_volume_descriptor_t *) (*cache)->data;
		*rec = (iso_directory_record_t *) vd->root_directory_record;
		return 0;
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
		return cdfs_find_in_dir(cdfs, 1, name, len, cache, rec);
	}

	/* Locate directory */
	dir = cdfs_find_dir(cdfs, name, split + 1);
	if (dir < 0) {
		return dir;
	}

	/* Find filename in directory */
	return cdfs_find_in_dir(cdfs, dir, name + split + 1,
								  len - split - 1, cache, rec);
}

struct tm tm;
static time_t cdfs_isodate(unsigned char *date)
{
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

int cdfs_mount(struct nas *root_nas)
{
	cdfs_t *cdfs;
	int rc;
	int blk;
	block_dev_cache_t *cache;
	iso_volume_descriptor_t *vd;
	int type;
	unsigned char *esc;
	struct cdfs_fs_info *fsi;

	fsi = root_nas->fs->fsi;

	/* Check block size */
	if (CDFS_BLOCKSIZE !=
			block_dev_ioctl(root_nas->fs->bdev, IOCTL_GETBLKSIZE, NULL, 0)) {
		return -ENXIO;
	}

	/* Allocate file system */
	cdfs = (cdfs_t *) sysmalloc(sizeof(cdfs_t));
	memset(cdfs, 0, sizeof(cdfs_t));
	cdfs->bdev = root_nas->fs->bdev;
	cdfs->blks =
			block_dev_ioctl(root_nas->fs->bdev, IOCTL_GETDEVSIZE, NULL, 0);
	if (cdfs->blks < 0) {
		return cdfs->blks;
	}

	/* Allocate cache */
	if (NULL == block_dev_cache_init(root_nas->fs->bdev, CDFS_POOLDEPTH)) {
		return -ENOMEM;
	}

	/* Read volume descriptors */
	cdfs->vdblk = 0;
	blk = 16;
	while (1) {
		cache  = block_dev_cached_read(root_nas->fs->bdev, blk);
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
	cache  = block_dev_cached_read(root_nas->fs->bdev, cdfs->vdblk);
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
	cdfs_create_dir_entry (root_nas);

	return 0;
}


int cdfs_umount(struct cdfs_fs_info *fsi) {
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

static int cdfs_open(struct nas *nas, char *name) {
	cdfs_t *cdfs;
	iso_directory_record_t *rec;
	block_dev_cache_t *cache;
	time_t date;
	int size;
	int extent;
	int flags;
	int rc;
	struct cdfs_file_info *fi;
	struct cdfs_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	cdfs = (cdfs_t *) fsi->data;


	/* Check open mode */
	if (fi->flags & (O_CREAT | O_TRUNC | O_APPEND)) {
		return -EROFS;
	}

	/* Locate file in file system */
	rc = cdfs_find_file(cdfs, name, strlen(name), &cache, &rec);
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

/*
static int cdfs_fsync(struct nas *nas) {
	return 0;
}
*/

static int cdfs_read(struct nas *nas, void *data, size_t size, off64_t pos) {
	size_t read;
	size_t count;
	size_t left;
	char *p;
	int iblock;
	int start;
	int blk;
	block_dev_cache_t *cache;
	struct cdfs_file_info *fi;
	struct cdfs_fs_info *fsi;
	cdfs_t *cdfs;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
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
static int cdfs_opendir(struct nas *nas, char *name) {
	cdfs_t *cdfs;
	iso_directory_record_t *rec;
	cdfs_file_t *cdfile;
	block_dev_cache_t *cache;
	time_t date;
	int size;
	int extent;
	int flags;
	int rc;
	struct cdfs_file_info *fi;

	fi = nas->fi->privdata;
	cdfs = (cdfs_t *) fi->fs->data;

	// Locate directory
	rc = cdfs_find_file(cdfs, name, strlen(name), &cache, &rec);
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

static int cdfs_readdir(struct nas *nas, direntry_t *dirp, int count) {
	cdfs_file_t *cdfile;
	cdfs_t *cdfs;
	iso_directory_record_t *rec;
	block_dev_cache_t *cache;
	int namelen;
	int reclen;
	int blkleft;
	char *name;
	wchar_t *wname;
	struct cdfs_file_info *fi;

	fi = nas->fi->privdata;
	cdfile = (cdfs_file_t *) fi->data;
	cdfs = (cdfs_t *) fi->fs->data;

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
static int    cdfsfs_open(struct node *node, struct file_desc *desc, int flags);
static int    cdfsfs_close(struct file_desc *desc);
static size_t cdfsfs_read(struct file_desc *desc, void *buf, size_t size);
static int    cdfsfs_ioctl(struct file_desc *desc, int request, ...);

static struct kfile_operations cdfsfs_fop = {
	.open = cdfsfs_open,
	.close = cdfsfs_close,
	.read = cdfsfs_read,
	.ioctl = cdfsfs_ioctl,
};

static int cdfsfs_open(struct node *node, struct file_desc *desc, int flags) {
	char path [PATH_MAX];
	struct nas *nas;
	struct cdfs_file_info *fi;

	nas = node->nas;
	fi = nas->fi->privdata;

	fi->flags = flags;

	vfs_get_relative_path(node, path, PATH_MAX);

	if(0 == cdfs_open(node->nas, path)) {
		return 0;
	}
	return -1;
}

static int cdfsfs_close(struct file_desc *desc) {
	struct cdfs_file_info *fi;
	struct nas *nas;

	nas = desc->node->nas;

	fi = nas->fi->privdata;
	fi->pos = 0;
	return 0;
}

static size_t cdfsfs_read(struct file_desc *desc, void *buf, size_t size) {
	int rezult;
	struct cdfs_file_info *fi;
	struct nas *nas;

	nas = desc->node->nas;

	fi = (struct cdfs_file_info *)nas->fi->privdata;

	rezult = cdfs_read(nas, (void *) buf, size, fi->pos);
	fi->pos += rezult;

	return rezult;
}

static int cdfsfs_ioctl(struct file_desc *desc, int request, ...) {
	return 0;
}

/* File system operations*/

static int cdfsfs_init(void * par);
static int cdfsfs_mount(void * dev, void *dir);
static int cdfsfs_umount(void *dir);

static struct fsop_desc cdfsfs_fsop = {
	.init = cdfsfs_init,
	.mount = cdfsfs_mount,
	.umount = cdfsfs_umount,
};

static struct fs_driver cdfsfs_driver = {
	.name = "iso9660",
	.file_op = &cdfsfs_fop,
	.fsop = &cdfsfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(cdfsfs_driver);

static int cdfsfs_init(void * par) {

	return 0;
}

static void cdfs_free_fs(struct nas *nas) {
	struct cdfs_file_info *fi;
	struct cdfs_fs_info *fsi;

	if(NULL != nas->fs) {
		fsi = nas->fs->fsi;

		if(NULL != fsi) {
			pool_free(&cdfs_fs_pool, fsi);
		}
		filesystem_free(nas->fs);
	}

	if(NULL != (fi = nas->fi->privdata)) {
		pool_free(&cdfs_file_pool, fi);
	}
}

static int cdfsfs_mount(void *dev, void *dir) {
	struct node *dir_node, *dev_node;
	struct nas *dir_nas, *dev_nas;
	struct cdfs_file_info *fi;
	struct cdfs_fs_info *fsi;
	struct node_fi *dev_fi;
	int rc;

	dev_node = dev;
	dev_nas = dev_node->nas;
	dir_node = dir;
	dir_nas = dir_node->nas;

	if (NULL == (dev_fi = dev_nas->fi)) {
		return -ENODEV;
	}

	if (NULL == (dir_nas->fs = filesystem_create("iso9660"))) {
		return -ENOMEM;
	}

	dir_nas->fs->bdev = dev_fi->privdata;

	/* allocate this fs info */
	if(NULL == (fsi = pool_alloc(&cdfs_fs_pool))) {
		rc = -ENOMEM;
		goto error;
	}
	memset(fsi, 0, sizeof(struct cdfs_fs_info));
	dir_nas->fs->fsi = fsi;
	//XXX vfs_get_path_by_node(dir_node, fsi->mntto);

	/* allocate this directory info */
	if(NULL == (fi = pool_alloc(&cdfs_file_pool))) {
		rc = -ENOMEM;
		goto error;
	}
	memset(fi, 0, sizeof(struct cdfs_file_info));
	dir_nas->fi->privdata = (void *) fi;

	if(0 == (rc = cdfs_mount(dir_nas))) {
		return 0;
	}

	error:
	cdfs_free_fs(dir_nas);

	return rc;

}

static int cdfs_umount_entry(struct nas *nas) {
	struct node *child;

	if (node_is_directory(nas->node)) {
		while (NULL != (child = vfs_subtree_get_child_next(nas->node, NULL))) {
			if (node_is_directory(child)) {
				cdfs_umount_entry(child->nas);
			}

			pool_free(&cdfs_file_pool, child->nas->fi->privdata);
			vfs_del_leaf(child);
		}
	}

	return 0;
}

static int cdfsfs_umount(void *dir) {
	struct node *dir_node;
	struct nas *dir_nas;
	struct cdfs_fs_info *fsi;

	dir_node = dir;
	dir_nas = dir_node->nas;
	fsi = dir_nas->fs->fsi;

	cdfs_umount(fsi);

	/* delete all entry node */
	cdfs_umount_entry(dir_nas);

	/* free cdfs file system pools and buffers*/
	cdfs_free_fs(dir_nas);

	return 0;
}


static struct node *cdfs_get_dir_node(cdfs_t *cdfs, int numrec, struct node *root) {
	char path[PATH_MAX];
	char tail[PATH_MAX];
	iso_pathtable_record_t *pathrec;
	struct node *dir;

	dir = root;
	pathrec = cdfs->path_table[numrec];

	memcpy(tail, pathrec->name, pathrec->length);
	tail[pathrec->length] = 0;

	/* go up to the root folder */
	while (1 != pathrec->parent) {
		pathrec = cdfs->path_table[pathrec->parent];
		memcpy(path, pathrec->name, pathrec->length);
		path[pathrec->length] = 0;
		strcat(path, "/");
		strcat(path, tail);
		strncpy(tail, path, PATH_MAX);
	}

	if (1 != numrec) {
		dir = vfs_subtree_lookup(root, tail);
	}

	return dir;
}

static int cdfs_create_file_node(node_t *dir_node, cdfs_t *cdfs, int dir) {
	block_dev_cache_t *cache;
	char *p;
	iso_directory_record_t *rec;
	int blk;
	int left;
	int reclen;
	int namelen;
	int flags;
	struct cdfs_file_info *fi;
	node_t *node;
	struct nas *nas, *dir_nas;
	wchar_t *wname;
	char name[PATH_MAX];

	dir_nas = dir_node->nas;

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
		flags = cdfs_isonum_711(rec->flags);

		if (reclen > 0) {
			/* Skip to next record */
			p += reclen;
			left -= reclen;

			/* if directory then not create node */
			if (flags & 2) {
				continue;
			}

			if (cdfs->joliet) {
				namelen /= 2;
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
				if (namelen > 1 && rec->name[namelen - 2] == ';') {
					namelen -= 2;
				}
				if (namelen > 0 && rec->name[namelen - 1] == '.') {
					namelen -= 1;
				}
				memcpy(name, rec->name, namelen);
			}
			name[namelen] = 0;

			node = vfs_subtree_create_child(dir_node, name, S_IFREG);
			if (!node) {
				return -ENOMEM;
			}

			fi = pool_alloc(&cdfs_file_pool);
			if (!fi) {
				vfs_del_leaf(node);
				return -ENOMEM;
			}

			nas = node->nas;

			nas->fs = dir_nas->fs;
			nas->fi->privdata = (void *)fi;

 		} else {
			/* Skip to next block */
			left -= (cache->data + CDFS_BLOCKSIZE) - p;
			p = cache->data + CDFS_BLOCKSIZE;
		}
	}
	return 0;
}

static int cdfs_create_dir_entry (struct nas *root_nas) {
	int n;
	iso_pathtable_record_t *pathrec;
	cdfs_t *cdfs;
	int namelen;
	char name[PATH_MAX];
	struct node *root_node, *node, *dir_node;
	struct nas *nas;
	struct cdfs_file_info *fi, *parent_fi;
	struct cdfs_fs_info *fsi;

	dir_node = root_node = node = root_nas->node;

	fi = parent_fi = root_nas->fi->privdata;
	fsi = root_nas->fs->fsi;
	cdfs = fsi->data;

	/* Setup pointers into path table buffer */
	for (n = 1; n < cdfs->path_table_records; n++) {
		pathrec = cdfs->path_table[n];
		namelen = pathrec->length;

		if(path_is_dotname(pathrec->name, namelen)) {
			continue;
		}

		memcpy(name, pathrec->name, namelen);
		name[20 >= name[0] ? 0 : namelen] = 0; /* root dir name empty */

		if (*name) {
			dir_node = cdfs_get_dir_node(cdfs, pathrec->parent, root_node);

			node = vfs_subtree_create_child(dir_node, name, S_IFDIR);
			if (!node) {
				return -ENOMEM;
			}

			fi = pool_alloc(&cdfs_file_pool);
			if (!fi) {
				vfs_del_leaf(node);
				return -ENOMEM;
			}

			nas = node->nas;
			nas->fs = root_nas->fs;
			nas->fi->privdata = (void *)fi;
		}

		cdfs_create_file_node(node, cdfs, n);
	}

	return 0;
}
//
//static int cdfs_get_full_path(cdfs_t *cdfs, int numrec, char *path) {
//	char full_path[PATH_MAX];
//	iso_pathtable_record_t *pathrec;
//
//	pathrec = cdfs->path_table[numrec];
//
//	/* go up to the root folder */
//	while (1 != pathrec->parent) {
//		strncpy(full_path, path, PATH_MAX);
//		pathrec = cdfs->path_table[pathrec->parent];
//		memcpy(path, pathrec->name, pathrec->length);
//		path[pathrec->length] = 0;
//		strcat(path, "/");
//		strcat(path, full_path);
//	}
//
//	return 0;
//}
