/**
 * @file
 * @brief
 *
 * @date 20.08.2012
 * @author Andrey Gazukin
 */

#include <time.h>
#include <ctype.h>
#include <string.h>

#include <util/err.h>

#include <drivers/block_dev.h>

#include <fs/inode.h>
#include <fs/iso9660.h>
#include <fs/file_desc.h>

extern int cdfs_isonum_711(unsigned char *p);
extern int cdfs_isonum_733(unsigned char *p);
extern time_t cdfs_isodate(unsigned char *date);

extern int cdfs_find_file(struct cdfs_fs_info *cdfs, char *name, int len,
							iso_directory_record_t **rec);

static int cdfs_open(struct inode *node, char *name) {
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

	/* Check open mode */
	if (fi->flags & (O_CREAT | O_TRUNC | O_APPEND)) {
		return -EROFS;
	}

	for(int i = 0; name[i]; i++){
		name[i] = toupper(name[i]);
	}

	/* Locate file in file system */
	rc = cdfs_find_file(fsi, name, strlen(name), &rec);
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

static int cdfs_read(struct inode *node, void *data, size_t size, int64_t pos) {
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

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;

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
			if (block_dev_read(fsi->bdev, p, count, blk) != (int) count) {
				return read;
			}
		}
		else {
			cache = block_dev_cached_read(fsi->bdev, blk);
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
	struct cdfs *cdfs;
	iso_directory_record_t *rec;
	cdfs_file_t *cdfile;
	time_t date;
	int size;
	int extent;
	int flags;
	int rc;
	struct cdfs_file_info *fi;

	fi = inode_priv(dir->node);
	cdfs = (struct cdfs *) dir_node->i_sb->data;

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
	struct cdfs *cdfs;
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
	cdfs = (struct cdfs *) node->i_sb->data;

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

*/

/* File operations */
static struct idesc *cdfsfs_open(struct inode *node, struct idesc *idesc, int __oflag);
static int    cdfsfs_close(struct file_desc *desc);
static size_t cdfsfs_read(struct file_desc *desc, void *buf, size_t size);

extern int vfs_get_relative_path(struct inode *node, char *path, size_t path_len);

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

struct file_operations cdfsfs_fop = {
	.open = cdfsfs_open,
	.close = cdfsfs_close,
	.read = cdfsfs_read,
};

