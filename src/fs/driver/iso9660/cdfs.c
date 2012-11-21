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
#include <types.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/iso9660.h>
#include <fs/mount.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <embox/block_dev.h>
#include <embox/buff.h>
#include <mem/misc/pool.h>
#include <net/in.h>
#include <kernel/time/ktime.h>
#include <kernel/time/clock_source.h>
#include <fcntl.h>

/* cdfs filesystem description pool */
POOL_DEF(cdfs_fs_pool, struct cdfs_fs_description, OPTION_GET(NUMBER,cdfs_descriptor_quantity));

/* cdfs file description pool */
POOL_DEF(cdfs_file_pool, struct cdfs_file_description, OPTION_GET(NUMBER,inode_quantity));

static int cdfs_open(cdfs_file_description_t *filp, char *name);
static int create_dir_entry (node_t *node);
static int get_full_path(cdfs_t *cdfs, int numrec, char *path, char *root);

static int isonum_711(unsigned char *p) {
  return p[0];
}

static int isonum_731(unsigned char *p) {
  return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

static int isonum_733(unsigned char *p) {
  return isonum_731(p);
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
	buf_t *buf;
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
	ptlen = isonum_733(vd->path_table_size);
	ptblk = isonum_731(vd->type_l_path_table);
	cdfs->path_table_buffer = malloc(ptlen);
	if (!cdfs->path_table_buffer) {
		return -ENOMEM;
	}

	/* Read L path table into buffer */
	ptpos = 0;
	while (ptpos < ptlen) {
		buf = get_buffer(cdfs->devno, ptblk++);
		if (!buf) {
			return -EIO;
		}

		if (ptlen - ptpos > CDFS_BLOCKSIZE) {
			memcpy(cdfs->path_table_buffer + ptpos, buf->data, CDFS_BLOCKSIZE);
			ptpos += CDFS_BLOCKSIZE;
		}
		else {
			memcpy(cdfs->path_table_buffer + ptpos, buf->data, ptlen - ptpos);
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
							malloc(cdfs->path_table_records *
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

static int cdfs_find_in_dir(cdfs_t *cdfs, int dir, char *name, int len, buf_t **dirbuf, iso_directory_record_t **dirrec) {
	buf_t *buf;
	char *p;
	iso_directory_record_t *rec;
	int blk;
	int left;
	int reclen;
	int namelen;

	/* The first two directory records are . (current) and .. (parent) */
	blk = cdfs->path_table[dir]->extent;
	buf = get_buffer(cdfs->devno, blk++);
	if (!buf) {
		return -EIO;
	}

	/* Get length of directory from the first record */
	p = buf->data;
	rec = (iso_directory_record_t *) p;
	left = isonum_733(rec->size);

	/* Find named entry in directory */
	while (left > 0) {
		/*
		 * Read next block if all records in current block has been read
		 * Directory records never cross block boundaries
		 */
		if (p >= buf->data + CDFS_BLOCKSIZE) {
			if (p > buf->data + CDFS_BLOCKSIZE) {
				return -EIO;
			}
			buf = get_buffer(cdfs->devno, blk++);
			if (!buf) {
				return -EIO;
			}
			p = buf->data;
		}

		/* Check for match */
		rec = (iso_directory_record_t *) p;
		reclen = isonum_711(rec->length);
		namelen = isonum_711(rec->name_len);

		if (reclen > 0) {
			if (cdfs_fnmatch(cdfs, name, len, (char *) rec->name, namelen)) {
				*dirrec = rec;
				*dirbuf = buf;
				return 0;
			}

			/* Skip to next record */
			p += reclen;
			left -= reclen;
		}
		else {
			/* Skip to next block */
			left -= (buf->data + CDFS_BLOCKSIZE) - p;
			p = buf->data + CDFS_BLOCKSIZE;
		}
	}

	return -ENOENT;
}

static int cdfs_find_file(cdfs_t *cdfs, char *name, int len,
				buf_t **buf, iso_directory_record_t **rec) {
	int dir;
	int split;
	int n;
	iso_volume_descriptor_t *vd;

	/* If root get directory record from volume descriptor */
	if (len == 0) {
		*buf = get_buffer(cdfs->devno, cdfs->vdblk);
		if (!*buf) {
			return -EIO;
		}
		vd = (iso_volume_descriptor_t *) (*buf)->data;
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
		return cdfs_find_in_dir(cdfs, 1, name, len, buf, rec);
	}

	/* Locate directory */
	dir = cdfs_find_dir(cdfs, name, split + 1);
	if (dir < 0) {
		return dir;
	}

	/* Find filename in directory */
	return cdfs_find_in_dir(cdfs, dir, name + split + 1,
								  len - split - 1, buf, rec);
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

//int cdfs_mount(cdfs_fs_description_t *fs)
int cdfs_mount(node_t *root_node)
{
	cdfs_t *cdfs;
	dev_t devno;
	int rc;
	int blk;
	buf_t *buf;
	iso_volume_descriptor_t *vd;
	int type;
	unsigned char *esc;
	cdfs_fs_description_t *fs;

	fs = ((cdfs_file_description_t *) root_node->fd)->fs;

	devno = fs->devnum;

	/* Check device */
	dev_open(fs->mntfrom);
	if (devno == NODEV) {
		return -NODEV;
	}
	if (device(devno)->driver->type != DEV_TYPE_BLOCK) {
		return -ENOTBLK;
	}

	/* Check block size */
	if (dev_ioctl(devno, IOCTL_GETBLKSIZE, NULL, 0) != CDFS_BLOCKSIZE) {
		return -ENXIO;
	}

	/* Allocate file system */
	cdfs = (cdfs_t *) malloc(sizeof(cdfs_t));
	memset(cdfs, 0, sizeof(cdfs_t));
	cdfs->devno = devno;
	cdfs->blks = dev_ioctl(devno, IOCTL_GETDEVSIZE, NULL, 0);
	if (cdfs->blks < 0) {
		return cdfs->blks;
	}

	/* Allocate cache */
	if (NULL == init_buffer_pool(devno, CDFS_POOLDEPTH)) {
		return -ENOMEM;
	}

	/* Read volume descriptors */
	cdfs->vdblk = 0;
	blk = 16;
	while (1) {
		buf  = get_buffer(devno, blk);
		if (!buf) {
			return -EIO;
		}
		vd = (iso_volume_descriptor_t *) buf->data;

		type = isonum_711(vd->type);
		esc = vd->escape_sequences;

		if (memcmp(vd->id, "CD001", 5) != 0) {
			/*free_buffer_pool(cdfs->cache); */
			dev_close(cdfs->devno);
			free(cdfs);
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
	buf  = get_buffer(cdfs->devno, cdfs->vdblk);
	if (!buf) {
		return -EIO;
	}
	vd = (iso_volume_descriptor_t *) buf->data;

	cdfs->volblks = isonum_733(vd->volume_space_size);

	rc = cdfs_read_path_table(cdfs, vd);
	if (rc < 0) {
		return rc;
	}

	/* Device mounted successfully */
	fs->data = cdfs;
	create_dir_entry (root_node);

	return 0;
}


int cdfs_umount(cdfs_fs_description_t *fs) {
	cdfs_t *cdfs = (cdfs_t *) fs->data;

	/* Free cache */
	/*if (cdfs->cache) free_buffer_pool(cdfs->cache); */

	/* Close device */
	dev_close(cdfs->devno);

	/* Deallocate file system */
	if (cdfs->path_table_buffer) {
		free(cdfs->path_table_buffer);
	}
	if (cdfs->path_table) {
		free(cdfs->path_table);
	}
	free(cdfs);

	return 0;
}

int cdfs_statfs(cdfs_fs_description_t *fs, statfs_t *buf) {
	cdfs_t *cdfs = (cdfs_t *) fs->data;

	buf->bsize = CDFS_BLOCKSIZE;
	buf->iosize = CDFS_BLOCKSIZE;
	buf->blocks = cdfs->volblks;
	buf->bfree = 0;
	buf->files = -1;
	buf->ffree = 0;
	/*buf->cachesize = cdfs->cache->poolsize * CDFS_BLOCKSIZE; */

	return 0;
}

static int cdfs_open(cdfs_file_description_t *filp, char *name) {
	cdfs_t *cdfs = (cdfs_t *) filp->fs->data;
	iso_directory_record_t *rec;
	cdfs_file_t *cdfile;
	buf_t *buf;
	time_t date;
	int size;
	int extent;
	int flags;
	int rc;

	/* Check open mode */
	if (filp->flags & (O_CREAT | O_TRUNC | O_APPEND)) {
		return -EROFS;
	}

	/* Locate file in file system */
	rc = cdfs_find_file(cdfs, name, strlen(name), &buf, &rec);
	if (rc < 0) {
		return rc;
	}

	flags = isonum_711(rec->flags);
	extent = isonum_733(rec->extent);
	date = cdfs_isodate(rec->date);
	size = isonum_733(rec->size);

	/* Allocate and initialize file block */
	cdfile = (cdfs_file_t *) malloc(sizeof(cdfs_file_t));
	if (!cdfile) {
		return -ENOMEM;
	}
	cdfile->extent = extent;
	cdfile->date = date;
	cdfile->size = size;
	if (flags & 2) {
		filp->flags |= F_DIR;
	}

	filp->data = cdfile;
	filp->mode = S_IFREG | S_IRUSR | S_IXUSR |
	   S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;;
	return 0;
}

static int cdfs_close(cdfs_file_description_t *filp) {
	cdfs_file_t *cdfile = (cdfs_file_t *) filp->data;

	if (cdfile) {
		free(cdfile);
	}
	filp->pos = 0;
	return 0;
}

static int cdfs_fsync(cdfs_file_description_t *filp) {
	return 0;
}

static int cdfs_read(cdfs_file_description_t *filp, void *data, size_t size, off64_t pos) {
	cdfs_file_t *cdfile = (cdfs_file_t *) filp->data;
	cdfs_t *cdfs = (cdfs_t *) filp->fs->data;
	size_t read;
	size_t count;
	size_t left;
	char *p;
	int iblock;
	int start;
	int blk;
	buf_t *buf;

	read = 0;
	p = (char *) data;
	while (pos < cdfile->size && size > 0) {
		iblock = (int) pos / CDFS_BLOCKSIZE;
		start = (int) pos % CDFS_BLOCKSIZE;

		count = CDFS_BLOCKSIZE - start;
		if (count > size) {
			count = size;
		}

		left = cdfile->size - (int) pos;
		if (count > left) {
			count = left;
		}
		if (count <= 0) {
			break;
		}

		blk = cdfile->extent + iblock;

		if (filp->flags & O_DIRECT) {
			if (start != 0 || count != CDFS_BLOCKSIZE) {
				return read;
			}
			if (dev_read(cdfs->devno, p, count, blk) != (int) count) {
				return read;
			}
		}
		else {
			buf = get_buffer(cdfs->devno, blk);
			if (!buf) {
				return -EIO;
			}
			memcpy(p, buf->data + start, count);
		}

		pos += count;
		p += count;
		read += count;
		size -= count;
	}

	return read;
}

static off64_t cdfs_tell(cdfs_file_description_t *filp) {
	return filp->pos;
}

static off64_t cdfs_lseek(cdfs_file_description_t *filp, off64_t offset, int origin) {
	cdfs_file_t *cdfile = (cdfs_file_t *) filp->data;

	switch (origin) {
	case SEEK_END:
		offset += cdfile->size;
		break;

	case SEEK_CUR:
		offset += filp->pos;
		break;
	}

	if (offset < 0) {
		return -EINVAL;
	}

	filp->pos = offset;
	return 0;
}

static int cdfs_fstat(cdfs_file_description_t *filp, stat_t *buffer) {
	cdfs_file_t *cdfile = (cdfs_file_t *) filp->data;
	cdfs_t *cdfs = (cdfs_t *) filp->fs->data;

	if (buffer) {
		memset(buffer, 0, sizeof(stat_t));

		if (filp->flags & F_DIR) {
			buffer->st_mode = S_IFDIR | S_IRUSR | S_IXUSR |
							  S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
		}
		else {
			buffer->st_mode = S_IFREG | S_IRUSR | S_IXUSR |
					  	  	  S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
		}

		buffer->st_ino = cdfile->extent;
		buffer->st_nlink = 1;
		buffer->st_dev = cdfs->devno;
		buffer->st_atime = buffer->st_mtime = buffer->st_ctime = cdfile->date;
		buffer->st_size = cdfile->size;
		buffer->st_blksize = CDFS_BLOCKSIZE;
		buffer->st_blocks = cdfile->extent;
		buffer->st_dev = cdfs->devno;
	}

	return cdfile->size;
}

static int cdfs_stat(cdfs_fs_description_t *fs, char *name, stat_t *buffer) {
	cdfs_t *cdfs = (cdfs_t *) fs->data;
	iso_directory_record_t *rec;
	buf_t *buf;
	int rc;
	int size;

	rc = cdfs_find_file(cdfs, name, strlen(name), &buf, &rec);
	if (rc < 0) {
		return rc;
	}

	size = isonum_733(rec->size);

	if (buffer) {
		memset(buffer, 0, sizeof(stat_t));

		if (rec->flags[0] & 2) {
			buffer->st_mode = S_IFDIR | S_IRUSR | S_IXUSR |
							  S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
		}
		else {
			buffer->st_mode = S_IFREG | S_IRUSR | S_IXUSR |
							  S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
		}

		buffer->st_ino = isonum_733(rec->extent);
		buffer->st_nlink = 1;
		buffer->st_dev = cdfs->devno;
		buffer->st_atime = buffer->st_mtime = buffer->st_ctime =
				cdfs_isodate(rec->date);
		buffer->st_size = cdfs->volblks * CDFS_BLOCKSIZE;
		buffer->st_blksize = CDFS_BLOCKSIZE;
		buffer->st_blocks = cdfs->volblks;
		buffer->st_dev = cdfs->devno;
	};

	return size;
}

static int cdfs_opendir(cdfs_file_description_t *filp, char *name) {
	cdfs_t *cdfs = (cdfs_t *) filp->fs->data;
	iso_directory_record_t *rec;
	cdfs_file_t *cdfile;
	buf_t *buf;
	time_t date;
	int size;
	int extent;
	int flags;
	int rc;

	/* Locate directory */
	rc = cdfs_find_file(cdfs, name, strlen(name), &buf, &rec);
	if (rc < 0) {
		return rc;
	}

	flags = isonum_711(rec->flags);
	extent = isonum_733(rec->extent);
	date = cdfs_isodate(rec->date);
	size = isonum_733(rec->size);

	if (!(flags & 2)) {
		return -ENOTDIR;
	}

	/* Allocate and initialize file block */
	cdfile = (cdfs_file_t *) malloc(sizeof(cdfs_file_t));
	if (!cdfile) {
		return -ENOMEM;
	}
	cdfile->extent = extent;
	cdfile->date = date;
	cdfile->size = size;

	filp->data = cdfile;
	filp->mode = S_IFDIR | S_IRUSR | S_IXUSR | S_IRGRP |
				 S_IXGRP | S_IROTH | S_IXOTH;
	return 0;
}

static int cdfs_readdir(cdfs_file_description_t *filp, direntry_t *dirp, int count) {
	cdfs_file_t *cdfile = (cdfs_file_t *) filp->data;
	cdfs_t *cdfs = (cdfs_t *) filp->fs->data;
	iso_directory_record_t *rec;
	buf_t *buf;
	int namelen;
	int reclen;
	int blkleft;
	char *name;
	wchar_t *wname;

  blkagain:
	if (count != 1) {
		return -EINVAL;
	}
	if (filp->pos >= cdfile->size) {
		return 0;
	}

	/* Get directory block */
	buf = get_buffer(cdfs->devno, cdfile->extent +
			(int) filp->pos / CDFS_BLOCKSIZE);
	if (!buf) {
		return -EIO;
	}

	/* Locate directory record */
  recagain:
	rec = (iso_directory_record_t *) (buf->data +
			(int) filp->pos % CDFS_BLOCKSIZE);
	reclen = isonum_711(rec->length);
	namelen = isonum_711(rec->name_len);

	/* Check for no more records in block */
	if (reclen == 0) {
		blkleft = CDFS_BLOCKSIZE - ((int) filp->pos % CDFS_BLOCKSIZE);
		filp->pos += blkleft;
		goto blkagain;
	}

	 /* Check for . and .. entries */
	if (namelen == 1 && (rec->name[0] == 0 || rec->name[0] == 1)) {
		filp->pos += reclen;
		goto recagain;
	}

	/* Get info from directory record */
	dirp->ino = isonum_733(rec->extent);
	dirp->reclen = sizeof(direntry_t) - MAX_LENGTH_PATH_NAME + namelen + 1;
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

	filp->pos += reclen;
	return 1;
}

#define FSOP_MKFS       0x00000001
#define FSOP_MOUNT      0x00000002
#define FSOP_UMOUNT     0x00000004
#define FSOP_STATFS     0x00000008
#define FSOP_OPEN       0x00000010
#define FSOP_CLOSE      0x00000020
#define FSOP_FSYNC      0x00000040
#define FSOP_READ       0x00000080
#define FSOP_WRITE      0x00000100
#define FSOP_IOCTL      0x00000200
#define FSOP_TELL       0x00000400
#define FSOP_LSEEK      0x00000800
#define FSOP_FTRUNCATE  0x00001000
#define FSOP_FUTIME     0x00002000
#define FSOP_UTIME      0x00004000
#define FSOP_FSTAT      0x00008000
#define FSOP_STAT       0x00010000
#define FSOP_ACCESS     0x00020000
#define FSOP_FCHMOD     0x00040000
#define FSOP_CHMOD      0x00080000
#define FSOP_FCHOWN     0x00100000
#define FSOP_CHOWN      0x00200000
#define FSOP_MKDIR      0x00400000
#define FSOP_RMDIR      0x00800000
#define FSOP_RENAME     0x01000000
#define FSOP_LINK       0x02000000
#define FSOP_UNLINK     0x04000000
#define FSOP_OPENDIR    0x08000000
#define FSOP_READDIR    0x10000000

struct fsops cdfsops = {
	FSOP_OPEN | FSOP_CLOSE | FSOP_FSYNC | FSOP_READ |
	FSOP_TELL | FSOP_LSEEK | FSOP_STAT | FSOP_FSTAT |
	FSOP_OPENDIR | FSOP_READDIR,

	NULL,
	NULL,

	NULL,
	cdfs_mount,
	cdfs_umount,

	cdfs_statfs,

	cdfs_open,
	cdfs_close,
	NULL,
	cdfs_fsync,

	cdfs_read,
	NULL,
	NULL,

	cdfs_tell,
	cdfs_lseek,
	NULL,

	/*NULL, */
	/*NULL, */

	cdfs_fstat,
	cdfs_stat,

	NULL,

	NULL,
	NULL,
	NULL,
	NULL,

	NULL,
	NULL,

	NULL,
	NULL,
	NULL,

	cdfs_opendir,
	cdfs_readdir
};


void init_cdfs(void) {
	/*register_filesystem("cdfs", &cdfsops); */
	return;
}


/* File operations */
static void *cdfsfs_fopen(struct file_desc *desc,  const char *mode);
static int cdfsfs_fclose(struct file_desc *desc);
static size_t cdfsfs_fread(void *buf, size_t size, size_t count, void *file);
static int cdfsfs_fseek(void *file, long offset, int whence);
static int cdfsfs_ioctl(void *file, int request, va_list args);
static int cdfsfs_fstat(void *file, void *buff);


static file_operations_t cdfsfs_fop = { cdfsfs_fopen, cdfsfs_fclose, cdfsfs_fread,
		NULL, cdfsfs_fseek, cdfsfs_ioctl, cdfsfs_fstat };



static void *cdfsfs_fopen(struct file_desc *desc, const char *mode) {
	node_t *nod;
	uint8_t _mode;
	char path [MAX_LENGTH_PATH_NAME];
	cdfs_file_description_t *fd;

	nod = desc->node;
	fd = (cdfs_file_description_t *)nod->fd;

	if ('r' == *mode) {
		_mode = O_RDONLY;
	}
	else if ('w' == *mode) {
		_mode = O_WRONLY;
	}
	else {
		_mode = O_RDONLY;
	}
	fd->mode = _mode;

	set_path (path, nod);
	cut_mount_dir(path, (char *) fd->fs->mntto);

	if(0 == cdfs_open(fd, path)) {
		return desc;
	}
	return NULL;
}

static int cdfsfs_fseek(void *file, long offset, int whence) {
	struct file_desc *desc;
	cdfs_file_description_t *fd;

	desc = (struct file_desc *) file;
	fd = (cdfs_file_description_t *)desc->node->fd;

	cdfs_lseek(fd, (off64_t) offset, whence);
	return 0;
}

static int cdfsfs_fstat(void *file, void *buff) {
	struct file_desc *desc;
	cdfs_file_description_t *fd;

	desc = (struct file_desc *) file;
	fd = (cdfs_file_description_t *)desc->node->fd;

	cdfs_fstat(fd, buff);
	return 0;
}

static int cdfsfs_fclose(struct file_desc *desc) {

	return cdfs_close((cdfs_file_description_t *)desc->node->fd);
}

static size_t cdfsfs_fread(void *buf, size_t size, size_t count, void *file) {
	size_t size_to_read;
	struct file_desc *desc;
	int rezult;
	cdfs_file_description_t *fd;

	size_to_read = size * count;
	desc = (struct file_desc *) file;
	fd = (cdfs_file_description_t *)desc->node->fd;

	//int cdfs_read(cdfs_file_description_t *filp, void *data, size_t size, off64_t pos);
	rezult = cdfs_read(fd, (void *) buf, size_to_read, fd->pos);
	fd->pos += rezult;

	return rezult;
}

static int cdfsfs_ioctl(void *file, int request, va_list args) {
	return 0;
}

/* File system operations*/

static int cdfsfs_init(void * par);
static int cdfsfs_mount(void * par);

static fsop_desc_t cdfsfs_fsop = { cdfsfs_init, NULL, cdfsfs_mount,
		NULL, NULL };

static fs_drv_t cdfsfs_drv = { "iso9660", &cdfsfs_fop, &cdfsfs_fsop };

DECLARE_FILE_SYSTEM_DRIVER(cdfsfs_drv);

static int cdfsfs_init(void * par) {
	//init_cdfs_fsinfo_pool();
	//init_cdfs_fileinfo_pool();

	return 0;
}

static int cdfsfs_mount(void *par) {
	mount_params_t *params;
	node_t *dir_node, *dev_node;
	cdfs_file_description_t *fd, *dev_fd;

	params = (mount_params_t *) par;
	dev_node = params->dev_node;
	if (NULL == (dir_node = vfs_find_node(params->dir, NULL))) {
		/*FIXME: usually mount doesn't create a directory*/
		if (NULL == (dir_node = vfs_add_path (params->dir, NULL))) {
			return -ENODEV;/*device not found*/
		}
		dir_node->properties = DIRECTORY_NODE_TYPE;
	}

	/* If dev_node created, but not attached to the filesystem driver */
	if (NULL == (dev_fd = (cdfs_file_description_t *) dev_node->fd)) {
		if((NULL == (dev_fd = pool_alloc(&cdfs_file_pool))) ||
				(NULL == (dev_fd->fs = pool_alloc(&cdfs_fs_pool)))) {
			return -ENOMEM;
		}
		dev_node->fd = dev_fd;
		dev_fd->fs->devnum = *((dev_t *)dev_node->dev_attr);
		dev_node->file_info = (void *) &cdfsfs_fop;
	}

	strncpy(dev_fd->fs->mntto, params->dir, strlen(params->dir) + 1);
	strncpy(dev_fd->fs->mntfrom, params->dev, strlen(params->dev) + 1);

	if(NULL == (fd = pool_alloc(&cdfs_file_pool))) {
		return -ENOMEM;
	}

	fd->fs = dev_fd->fs;
	//fd->p_fs_dsc->devnum = dev_node->dev_attr;
	dir_node->fs_type = &cdfsfs_drv;
	dir_node->dev_type = dev_node->dev_type;
	dir_node->fd = (void *) fd;

	return cdfs_mount(dir_node);
}

static int create_file_node (node_t *dir_node, cdfs_t *cdfs, char *dirpath, int dir) {
	buf_t *buf;
	char *p;
	iso_directory_record_t *rec;
	int blk;
	int left;
	int reclen;
	int namelen;
	int flags;
	cdfs_file_description_t *fd, *dir_fd;
	node_t *node;
	char name[MAX_LENGTH_PATH_NAME];
	char full_name[MAX_LENGTH_PATH_NAME];

	dir_fd = (cdfs_file_description_t *) dir_node->fd;

	/* The first two directory records are . (current) and .. (parent) */
	blk = cdfs->path_table[dir]->extent;
	buf = get_buffer(cdfs->devno, blk++);
	if (!buf) {
		return -EIO;
	}

	/* Get length of directory from the first record */
	p = buf->data;
	rec = (iso_directory_record_t *) p;
	left = isonum_733(rec->size);

	/* Find named entry in directory */
	while (left > 0) {
		/*
		 * Read next block if all records in current block has been read
		 * Directory records never cross block boundaries
		 */
		if (p >= buf->data + CDFS_BLOCKSIZE) {
			if (p > buf->data + CDFS_BLOCKSIZE) {
				return -EIO;
			}
			buf = get_buffer(cdfs->devno, blk++);
			if (!buf) {
				return -EIO;
			}
			p = buf->data;
		}

		/* Check for match */
		rec = (iso_directory_record_t *) p;
		reclen = isonum_711(rec->length);
		namelen = isonum_711(rec->name_len);
		flags = isonum_711(rec->flags);

		if (reclen > 0) {
			/* Skip to next record */
			p += reclen;
			left -= reclen;

			/* if directory then not create node */
			if(flags & 2) {
				continue;
			}

			strcpy(full_name, dirpath);
			strcat(full_name, "/");

			memcpy(name, rec->name, namelen);
			name[namelen] = 0;

			strcat(full_name, name);
			if(NULL == (node = vfs_add_path (full_name, NULL))) {
				return -ENOMEM;
			}

			if(NULL == (fd = pool_alloc(&cdfs_file_pool))) {
				return -ENOMEM;
			}

			fd->fs = dir_fd->fs;
			node->fs_type = &cdfsfs_drv;
			node->dev_type = dir_node->dev_type;
			node->dev_attr = dir_node->dev_attr;
			node->fd = (void *)fd;
		}
		else {
			/* Skip to next block */
			left -= (buf->data + CDFS_BLOCKSIZE) - p;
			p = buf->data + CDFS_BLOCKSIZE;
		}
	}
	return 0;
}

static int create_dir_entry (node_t *parent) {
		int n;
		iso_pathtable_record_t *pathrec;
		cdfs_file_description_t *fd, *parents_fd;
		cdfs_t *cdfs;
		node_t *parent_node;
		node_t *node;
		int namelen;
		//int reclen;
		char path[MAX_LENGTH_PATH_NAME];
		char name[MAX_LENGTH_PATH_NAME];

		node = parent_node = parent;
		fd = parents_fd = (cdfs_file_description_t *) parent_node->fd;
		cdfs = parents_fd->fs->data;

		strncpy(path, parents_fd->fs->mntto, MAX_LENGTH_PATH_NAME);

		/* Setup pointers into path table buffer */
		for (n = 1; n < cdfs->path_table_records; n++) {
			pathrec = cdfs->path_table[n];
			namelen = pathrec->length;
			//reclen = sizeof(iso_pathtable_record_t) + namelen + (namelen & 1);

			memcpy(name, pathrec->name, namelen);
			name[namelen] = 0;
			/* root dir name empty */
			if(20 >= name[0]) {
				name[0] = 0;
				get_full_path(cdfs, n, name, path);
			}
			else {

				get_full_path(cdfs, n, name, path);

				if(NULL == (node = vfs_add_path (name, NULL))) {
					return -ENOMEM;
				}

				if(NULL == (fd = pool_alloc(&cdfs_file_pool))) {
					return -ENOMEM;
				}
				fd->fs = parents_fd->fs;
				node->fs_type = &cdfsfs_drv;
				node->dev_type = parent_node->dev_type;
				node->dev_attr = parent_node->dev_attr;
				node->fd = (void *)fd;
			}

			create_file_node (node, cdfs, name, n);
		}

	return 0;
}

static int get_full_path(cdfs_t *cdfs, int numrec, char *path, char *root) {
	char full_path[MAX_LENGTH_PATH_NAME];
	iso_pathtable_record_t *pathrec;

	pathrec = cdfs->path_table[numrec];

	/* go up to the root folder */
	while(1 != pathrec->parent) {
		strcpy(full_path, path);
		pathrec = cdfs->path_table[pathrec->parent];
		memcpy(path, pathrec->name, pathrec->length);
		path[pathrec->length] = 0;
		strcat(path, "/");
		strcat(path, full_path);
	}
	strcpy(full_path, root);
	strcat(full_path, "/");
	strcat(full_path, path);
	strcpy(path, full_path);

	return 0;
}
