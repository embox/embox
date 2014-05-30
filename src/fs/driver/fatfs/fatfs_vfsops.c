/*
 * Copyright (c) 2005-2008, Kohsuke Ohtani
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <util/array.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>
#include <mem/sysmalloc.h>

#include <fs/fs_driver.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/path.h>
#include <fs/hlpr_path.h>
#include <fs/file_system.h>
#include <fs/file_desc.h>
#include <limits.h>

#include <embox/block_dev.h>

#include <framework/mod/options.h>

#include "fatfs.h"

/* fat filesystem description pool */
POOL_DEF(fat_fs_pool, struct fat_fs_info,
	OPTION_GET(NUMBER, fat_descriptor_quantity));

/* fat file description pool */
POOL_DEF(fat_file_pool, struct fat_file_info,
	OPTION_GET(NUMBER, inode_quantity));

#define LABEL "EMBOX_DISK\0"
#define SYSTEM "FAT12"

/*
 *  Time bits: 15-11 hours (0-23), 10-5 min, 4-0 sec /2
 *  Date bits: 15-9 year - 1980, 8-5 month, 4-0 day
 */
#define TEMP_DATE   0x3021
#define TEMP_TIME   0

#define fat_open	((vnop_open_t)vop_nullop)
#define fat_close	((vnop_close_t)vop_nullop)
static int fat_read	(vnode_t, file_t, void *, size_t, size_t *);
static int fat_write	(vnode_t, file_t, void *, size_t, size_t *);
#define fat_seek	((vnop_seek_t)vop_nullop)
#define fat_ioctl	((vnop_ioctl_t)vop_einval)
#define fat_fsync	((vnop_fsync_t)vop_nullop)
static int fat_readdir(vnode_t, file_t, struct dirent *);
static int fat_lookup	(vnode_t, char *, vnode_t);
static int fat_create	(vnode_t, char *, mode_t);
static int fat_remove	(vnode_t, vnode_t, char *);
static int fat_rename	(vnode_t, vnode_t, char *, vnode_t, vnode_t, char *);
static int fat_mkdir	(vnode_t, char *, mode_t);
static int fat_rmdir	(vnode_t, vnode_t, char *);
static int fat_getattr(vnode_t, struct vattr *);
static int fat_setattr(vnode_t, struct vattr *);
static int fat_inactive(vnode_t);
static int fat_truncate(vnode_t, off_t);

/*
 * Read one cluster to buffer.
 */
static int fat_read_cluster(struct fatfsmount *fmp, u_long cluster) {
	u_long sec;
	size_t size;

	sec = cl_to_sec(fmp, cluster);
	size = fmp->sec_per_cl * SEC_SIZE;
	return device_read(fmp->dev, fmp->io_buf, &size, sec);
}

/*
 * Write one cluster from buffer.
 */
static int fat_write_cluster(struct fatfsmount *fmp, u_long cluster) {
	u_long sec;
	size_t size;

	sec = cl_to_sec(fmp, cluster);
	size = fmp->sec_per_cl * SEC_SIZE;
	return device_write(fmp->dev, fmp->io_buf, &size, sec);
}

/*
 * Lookup vnode for the specified file/directory.
 * The vnode data will be set properly.
 */
static int fat_lookup(vnode_t dvp, char *name, vnode_t vp) {
	struct fatfsmount *fmp;
	struct fat_dirent *de;
	struct fatfs_node *np;
	int error;

	if (*name == '\0') {
		return ENOENT;
	}

	fmp = vp->v_mount->m_data;
	mutex_lock(&fmp->lock);

	DPRINTF(("fatfs_lookup: name=%s\n", name));

	np = vp->v_data;
	error = fatfs_lookup_node(dvp, name, np);
	if (error) {
		DPRINTF(("fatfs_lookup: failed!! name=%s\n", name));
		mutex_unlock(&fmp->lock);
		return error;
	}
	de = &np->dirent;
	vp->v_type = IS_DIR(de) ? VDIR : VREG;
	fat_attr_to_mode(de->attr, &vp->v_mode);
	vp->v_mode = ALLPERMS;
	vp->v_size = de->size;
	vp->v_blkno = de->cluster;

	DPRINTF(("fatfs_lookup: cl=%d\n", de->cluster));
	mutex_unlock(&fmp->lock);
	return 0;
}

static int fat_read(vnode_t vp, file_t fp, void *buf,
						size_t size, size_t *result) {
	struct fatfsmount *fmp;
	int nr_read, nr_copy, buf_pos, error;
	u_long cl, file_pos;

	DPRINTF(("fatfs_read: vp=%x\n", vp));

	*result = 0;
	fmp = vp->v_mount->m_data;

	if (vp->v_type == VDIR) {
		return EISDIR;
	}
	if (vp->v_type != VREG) {
		return EINVAL;
	}

	/* Check if current file position is already end of file. */
	file_pos = fp->f_offset;
	if (file_pos >= vp->v_size) {
		return 0;
	}

	mutex_lock(&fmp->lock);

	/* Get the actual read size. */
	if (vp->v_size - file_pos < size) {
		size = vp->v_size - file_pos;
	}

	/* Seek to the cluster for the file offset */
	error = fat_seek_cluster(fmp, vp->v_blkno, file_pos, &cl);
	if (error) {
		goto out;
	}

	/* Read and copy data */
	nr_read = 0;
	buf_pos = file_pos % fmp->cluster_size;
	do {
		if (fat_read_cluster(fmp, cl)) {
			error = EIO;
			goto out;
		}

		nr_copy = fmp->cluster_size;
		if (buf_pos > 0) {
			nr_copy -= buf_pos;
		}
		if (buf_pos + size < fmp->cluster_size) {
			nr_copy = size;
		}
		memcpy(buf, fmp->io_buf + buf_pos, nr_copy);

		file_pos += nr_copy;
		nr_read += nr_copy;
		size -= nr_copy;
		if (size <= 0) {
			break;
		}

		error = fat_next_cluster(fmp, cl, &cl);
		if (error) {
			goto out;
		}

		buf = (void *)((u_long)buf + nr_copy);
		buf_pos = 0;
	} while (!IS_EOFCL(fmp, cl));

	fp->f_offset = file_pos;
	*result = nr_read;
	error = 0;
 out:
	mutex_unlock(&fmp->lock);
	return error;
}

static int fat_write(vnode_t vp, file_t fp, void *buf,
						size_t size, size_t *result) {
	struct fatfsmount *fmp;
	struct fatfs_node *np;
	struct fat_dirent *de;
	int nr_copy, nr_write, buf_pos, i, cl_size, error;
	u_long file_pos, end_pos;
	u_long cl;

	DPRINTF(("fatfs_write: vp=%x\n", vp));

	*result = 0;
	fmp = vp->v_mount->m_data;

	if (vp->v_type == VDIR) {
		return EISDIR;
	}
	if (vp->v_type != VREG) {
		return EINVAL;
	}

	mutex_lock(&fmp->lock);

	/* Check if file position exceeds the end of file. */
	end_pos = vp->v_size;
	file_pos = (fp->f_flags & O_APPEND) ? end_pos : fp->f_offset;
	if (file_pos + size > end_pos) {
		/* Expand the file size before writing to it */
		end_pos = file_pos + size;
		error = fat_expand_file(fmp, vp->v_blkno, end_pos);
		if (error) {
			error = EIO;
			goto out;
		}

		/* Update directory entry */
		np = vp->v_data;
		de = &np->dirent;
		de->size = end_pos;
		error = fatfs_put_node(fmp, np);
		if (error) {
			goto out;
		}
		vp->v_size = end_pos;
	}

	/* Seek to the cluster for the file offset */
	error = fat_seek_cluster(fmp, vp->v_blkno, file_pos, &cl);
	if (error) {
		goto out;
	}

	buf_pos = file_pos % fmp->cluster_size;
	cl_size = size / fmp->cluster_size + 1;
	nr_write = 0;
	i = 0;
	do {
		/* First and last cluster must be read before write */
		if (i == 0 || i == cl_size) {
			if (fat_read_cluster(fmp, cl)) {
				error = EIO;
				goto out;
			}
		}
		nr_copy = fmp->cluster_size;
		if (buf_pos > 0) {
			nr_copy -= buf_pos;
		}
		if (buf_pos + size < fmp->cluster_size) {
			nr_copy = size;
		}
		memcpy(fmp->io_buf + buf_pos, buf, nr_copy);

		if (fat_write_cluster(fmp, cl)) {
			error = EIO;
			goto out;
		}
		file_pos += nr_copy;
		nr_write += nr_copy;
		size -= nr_copy;
		if (size <= 0) {
			break;
		}

		error = fat_next_cluster(fmp, cl, &cl);
		if (error) {
			goto out;
		}

		buf = (void *)((u_long)buf + nr_copy);
		buf_pos = 0;
		i++;
	} while (!IS_EOFCL(fmp, cl));

	fp->f_offset = file_pos;

	/*
	 * XXX: Todo!
	 *    de.time = ?
	 *    de.date = ?
	 *    if (dirent_set(fp, &de))
	 *        return EIO;
	 */
	*result = nr_write;
	error = 0;
 out:
	mutex_unlock(&fmp->lock);
	return error;
}

static int fat_readdir(vnode_t vp, file_t fp, struct dirent *dir) {
	struct fatfsmount *fmp;
	struct fatfs_node np;
	struct fat_dirent *de;
	int error;

	fmp = vp->v_mount->m_data;
	mutex_lock(&fmp->lock);

	error = fatfs_get_node(vp, fp->f_offset, &np);
	if (error) {
		goto out;
	}
	de = &np.dirent;
	fat_restore_name((char *)&de->name, dir->d_name);

	if (de->attr & FA_SUBDIR) {
		dir->d_type = DT_DIR;
	} else if (de->attr & FA_DEVICE) {
		dir->d_type = DT_BLK;
	} else {
		dir->d_type = DT_REG;
	}

	dir->d_fileno = fp->f_offset;
	dir->d_namlen = strlen(dir->d_name);

	fp->f_offset++;
	error = 0;
 out:
	mutex_unlock(&fmp->lock);
	return error;
}

/*
 * Create empty file.
 */
static int fat_create(vnode_t dvp, char *name, mode_t mode) {
	struct fatfsmount *fmp;
	struct fatfs_node np;
	struct fat_dirent *de;
	u_long cl;
	int error;

	DPRINTF(("fatfs_create: %s\n", name));

	if (!S_ISREG(mode)) {
		return EINVAL;
	}

	if (!fat_valid_name(name)) {
		return EINVAL;
	}

	fmp = dvp->v_mount->m_data;
	mutex_lock(&fmp->lock);

	/* Allocate free cluster for new file. */
	error = fat_alloc_cluster(fmp, 0, &cl);
	if (error) {
		goto out;
	}

	de = &np.dirent;
	memset(de, 0, sizeof(struct fat_dirent));
	fat_convert_name(name, (char *)de->name);
	de->cluster = cl;
	de->time = TEMP_TIME;
	de->date = TEMP_DATE;
	fat_mode_to_attr(mode, &de->attr);
	error = fatfs_add_node(dvp, &np);
	if (error) {
		goto out;
	}
	error = fat_set_cluster(fmp, cl, fmp->fat_eof);
 out:
	mutex_unlock(&fmp->lock);
	return error;
}

static int fat_remove(vnode_t dvp, vnode_t vp, char *name) {
	struct fatfsmount *fmp;
	struct fatfs_node np;
	struct fat_dirent *de;
	int error;

	if (*name == '\0') {
		return ENOENT;
	}

	fmp = dvp->v_mount->m_data;
	mutex_lock(&fmp->lock);

	error = fatfs_lookup_node(dvp, name, &np);
	if (error) {
		goto out;
	}
	de = &np.dirent;
	if (IS_DIR(de)) {
		error = EISDIR;
		goto out;
	}
	if (!IS_FILE(de)) {
		error = EPERM;
		goto out;
	}

	/* Remove clusters */
	error = fat_free_clusters(fmp, de->cluster);
	if (error) {
		goto out;
	}

	/* remove directory */
	de->name[0] = 0xe5;
	error = fatfs_put_node(fmp, &np);
 out:
	mutex_unlock(&fmp->lock);
	return error;
}

static int fat_rename(vnode_t dvp1, vnode_t vp1, char *name1,
	     vnode_t dvp2, vnode_t vp2, char *name2) {
	struct fatfsmount *fmp;
	struct fatfs_node np1;
	struct fat_dirent *de1, *de2;
	int error;

	fmp = dvp1->v_mount->m_data;
	mutex_lock(&fmp->lock);

	error = fatfs_lookup_node(dvp1, name1, &np1);
	if (error) {
		goto out;
	}
	de1 = &np1.dirent;

	if (IS_FILE(de1)) {
		/* Remove destination file, first */
		error = fat_remove(dvp2, vp1, name2);
		if (error == EIO) {
			goto out;
		}

		/* Change file name of directory entry */
		fat_convert_name(name2, (char *)de1->name);

		/* Same directory ? */
		if (dvp1 == dvp2) {
			/* Change the name of existing file */
			error = fatfs_put_node(fmp, &np1);
			if (error) {
				goto out;
			}
		} else {
			/* Create new directory entry */
			error = fatfs_add_node(dvp2, &np1);
			if (error) {
				goto out;
			}

			/* Remove souce file */
			error = fat_remove(dvp1, vp2, name1);
			if (error) {
				goto out;
			}
		}
	} else {

		/* remove destination directory */
		error = fat_rmdir(dvp2, NULL, name2);
		if (error == EIO) {
			goto out;
		}

		/* Change file name of directory entry */
		fat_convert_name(name2, (char *)de1->name);

		/* Same directory ? */
		if (dvp1 == dvp2) {
			/* Change the name of existing directory */
			error = fatfs_put_node(fmp, &np1);
			if (error) {
				goto out;
			}
		} else {
			/* Create new directory entry */
			error = fatfs_add_node(dvp2, &np1);
			if (error) {
				goto out;
			}

			/* Update "." and ".." for renamed directory */
			if (fat_read_cluster(fmp, de1->cluster)) {
				error = EIO;
				goto out;
			}

			de2 = (struct fat_dirent *)fmp->io_buf;
			de2->cluster = de1->cluster;
			de2->time = TEMP_TIME;
			de2->date = TEMP_DATE;
			de2++;
			de2->cluster = dvp2->v_blkno;
			de2->time = TEMP_TIME;
			de2->date = TEMP_DATE;

			if (fat_write_cluster(fmp, de1->cluster)) {
				error = EIO;
				goto out;
			}

			/* Remove souce directory */
			error = fat_rmdir(dvp1, NULL, name1);
			if (error) {
				goto out;
			}
		}
	}
 out:
	mutex_unlock(&fmp->lock);
	return error;
}

static int fat_mkdir(vnode_t dvp, char *name, mode_t mode) {
	struct fatfsmount *fmp;
	struct fatfs_node np;
	struct fat_dirent *de;
	u_long cl;
	int error;

	if (!S_ISDIR(mode)) {
		return EINVAL;
	}

	if (!fat_valid_name(name)) {
		return ENOTDIR;
	}

	fmp = dvp->v_mount->m_data;
	mutex_lock(&fmp->lock);

	/* Allocate free cluster for directory data */
	error = fat_alloc_cluster(fmp, 0, &cl);
	if (error) {
		goto out;
	}

	memset(&np, 0, sizeof(struct fatfs_node));
	de = &np.dirent;
	fat_convert_name(name, (char *)&de->name);
	de->cluster = cl;
	de->time = TEMP_TIME;
	de->date = TEMP_DATE;
	fat_mode_to_attr(mode, &de->attr);
	error = fatfs_add_node(dvp, &np);
	if (error) {
		goto out;
	}

	/* Initialize "." and ".." for new directory */
	memset(fmp->io_buf, 0, fmp->cluster_size);

	de = (struct fat_dirent *)fmp->io_buf;
	memcpy(de->name, ".          ", 11);
	de->attr = FA_SUBDIR;
	de->cluster = cl;
	de->time = TEMP_TIME;
	de->date = TEMP_DATE;
	de++;
	memcpy(de->name, "..         ", 11);
	de->attr = FA_SUBDIR;
	de->cluster = dvp->v_blkno;
	de->time = TEMP_TIME;
	de->date = TEMP_DATE;

	if (fat_write_cluster(fmp, cl)) {
		error = EIO;
		goto out;
	}
	/* Add eof */
	error = fat_set_cluster(fmp, cl, fmp->fat_eof);
 out:
	mutex_unlock(&fmp->lock);
	return error;
}

/*
 * remove can be done only with empty directory
 */
static int fat_rmdir(vnode_t dvp, vnode_t vp, char *name) {
	struct fatfsmount *fmp;
	struct fatfs_node np;
	struct fat_dirent *de;
	int error;

	if (*name == '\0') {
		return ENOENT;
	}

	fmp = dvp->v_mount->m_data;
	mutex_lock(&fmp->lock);

	error = fatfs_lookup_node(dvp, name, &np);
	if (error) {
		goto out;
	}

	de = &np.dirent;
	if (!IS_DIR(de)) {
		error = ENOTDIR;
		goto out;
	}

	/* Remove clusters */
	error = fat_free_clusters(fmp, de->cluster);
	if (error) {
		goto out;
	}

	/* remove directory */
	de->name[0] = 0xe5;

	error = fatfs_put_node(fmp, &np);
 out:
	mutex_unlock(&fmp->lock);
	return error;
}

static int fat_getattr(vnode_t vp, struct vattr *vap) {
	/* XXX */
	return 0;
}

static int fat_setattr(vnode_t vp, struct vattr *vap) {
	/* XXX */
	return 0;
}


static int fat_inactive(vnode_t vp) {

	free(vp->v_data);
	return 0;
}

static int fat_truncate(vnode_t vp, off_t length) {
	struct fatfsmount *fmp;
	struct fatfs_node *np;
	struct fat_dirent *de;
	int error;

	fmp = vp->v_mount->m_data;
	mutex_lock(&fmp->lock);

	np = vp->v_data;
	de = &np->dirent;

	if (length == 0) {
		/* Remove clusters */
		error = fat_free_clusters(fmp, de->cluster);
		if (error) {
			goto out;
		}
	} else if (length > vp->v_size) {
		error = fat_expand_file(fmp, vp->v_blkno, length);
		if (error) {
			error = EIO;
			goto out;
		}
	}

	/* Update directory entry */
	de->size = length;
	error = fatfs_put_node(fmp, np);
	if (error) {
		goto out;
	}
	vp->v_size = length;
 out:
	mutex_unlock(&fmp->lock);
	return error;
}

int fat_init(void) {

	return 0;
}



static int fat_mount	(mount_t mp, char *dev, int flags, void *data);
static int fat_unmount(mount_t mp);
#define fat_sync	((vfsop_sync_t)vfs_nullop)
static int fat_vget	(mount_t mp, vnode_t vp);
#define fat_statfs	((vfsop_statfs_t)vfs_nullop)

/*
 * Read BIOS parameter block.
 * Return 0 on sucess.
 */
static int fat_read_bpb(struct fatfsmount *fmp) {
	struct fat_bpb *bpb;
	size_t size;
	int error;

	bpb = sysmalloc(SEC_SIZE);
	if (bpb == NULL) {
		return ENOMEM;
	}

	/* Read boot sector (block:0) */
	size = SEC_SIZE;
	error = device_read(fmp->dev, bpb, &size, 0);
	if (error) {
		sysfree(bpb);
		return error;
	}
	if (bpb->bytes_per_sector != SEC_SIZE) {
		DPRINTF(("fatfs: invalid sector size\n"));
		sysfree(bpb);
		return EINVAL;
	}

	/* Build FAT mount data */
	fmp->fat_start = bpb->hidden_sectors + bpb->reserved_sectors;
	fmp->root_start = fmp->fat_start +
		(bpb->num_of_fats * bpb->sectors_per_fat);
	fmp->data_start =
		fmp->root_start + (bpb->root_entries / DIR_PER_SEC);
	fmp->sec_per_cl = bpb->sectors_per_cluster;
	fmp->cluster_size = bpb->sectors_per_cluster * SEC_SIZE;
	fmp->last_cluster = (bpb->total_sectors - fmp->data_start) /
		bpb->sectors_per_cluster + CL_FIRST;
	fmp->free_scan = CL_FIRST;

	if (!strncmp((const char *)bpb->file_sys_id, "FAT12   ", 8)) {
		fmp->fat_type = 12;
		fmp->fat_mask = FAT12_MASK;
		fmp->fat_eof = CL_EOF & FAT12_MASK;
	} else if (!strncmp((const char *)bpb->file_sys_id, "FAT16   ", 8)) {
		fmp->fat_type = 16;
		fmp->fat_mask = FAT16_MASK;
		fmp->fat_eof = CL_EOF & FAT16_MASK;
	} else {
		/* FAT32 is not supported now! */
		DPRINTF(("fatfs: invalid FAT type\n"));
		sysfree(bpb);
		return EINVAL;
	}
	sysfree(bpb);

	DPRINTF(("----- FAT info -----\n"));
	DPRINTF(("drive:%x\n", (int)bpb->physical_drive));
	DPRINTF(("total_sectors:%d\n", (int)bpb->total_sectors));
	DPRINTF(("heads       :%d\n", (int)bpb->heads));
	DPRINTF(("serial      :%x\n", (int)bpb->serial_no));
	DPRINTF(("cluster size:%u sectors\n", (int)fmp->sec_per_cl));
	DPRINTF(("fat_type    :FAT%u\n", (int)fmp->fat_type));
	DPRINTF(("fat_eof     :0x%x\n\n", (int)fmp->fat_eof));
	return 0;
}

/*
 * Mount file system.
 */
static int fat_mount(mount_t mp, char *dev, int flags, void *data) {
	struct fatfsmount *fmp;
	vnode_t vp;
	int error = 0;

	DPRINTF(("fatfs_mount device=%s\n", dev));

	fmp = sysmalloc(sizeof(struct fatfsmount));
	if (fmp == NULL) {
		return ENOMEM;
	}

	fmp->dev = mp->m_dev;
	if (fat_read_bpb(fmp) != 0) {
		goto err1;
	}

	error = ENOMEM;
	fmp->io_buf = sysmalloc(fmp->sec_per_cl * SEC_SIZE);
	if (fmp->io_buf == NULL) {
		goto err1;
	}

	fmp->fat_buf = sysmalloc(SEC_SIZE * 2);
	if (fmp->fat_buf == NULL) {
		goto err2;
	}

	fmp->dir_buf = sysmalloc(SEC_SIZE);
	if (fmp->dir_buf == NULL) {
		goto err3;
	}

	mutex_init(&fmp->lock);
	mp->m_data = fmp;
	vp = mp->m_root;
	vp->v_blkno = CL_ROOT;
	return 0;
 err3:
	sysfree(fmp->fat_buf);
 err2:
	sysfree(fmp->io_buf);
 err1:
	sysfree(fmp);
	return error;
}

/*
 * Unmount the file system.
 */
static int fat_unmount(mount_t mp) {
	struct fatfsmount *fmp;

	fmp = mp->m_data;
	sysfree(fmp->dir_buf);
	sysfree(fmp->fat_buf);
	sysfree(fmp->io_buf);
	mutex_destroy(&fmp->lock);
	sysfree(fmp);
	return 0;
}

/*
 * Prepare the FAT specific node and fill the vnode.
 */
static int fat_vget(mount_t mp, vnode_t vp) {
	struct fatfs_node *np;

	np = sysmalloc(sizeof(struct fatfs_node));
	if (np == NULL) {
		return ENOMEM;
	}
	vp->v_data = np;
	return 0;
}

static fat_file_info_t *fat_fi_alloc(struct nas *nas, void *fs) {
	struct fat_file_info *fi;

	fi = pool_alloc(&fat_file_pool);
	if (fi) {
		memset(fi, 0, sizeof(*fi));
		nas->fi->privdata = fi;
		nas->fs = fs;
	}

	return fi;
}

/* File operations */
static int    fatfs_open(struct node *node, struct file_desc *file_desc, int flags);
static int    fatfs_close(struct file_desc *desc);
static size_t fatfs_read(struct file_desc *desc, void *buf, size_t size);
static size_t fatfs_write(struct file_desc *desc, void *buf, size_t size);
static int    fatfs_ioctl(struct file_desc *desc, int request, ...);

static struct kfile_operations fatfs_fop = {
	.open = fatfs_open,
	.close = fatfs_close,
	.read = fatfs_read,
	.write = fatfs_write,
	.ioctl = fatfs_ioctl,
};

/*
 * file_operation
 */
static int fatfs_open(struct node *nod, struct file_desc *desc,  int flag) {
	node_t *node;
	struct nas *nas;
	uint8_t path [PATH_MAX];
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;


	node = nod;
	nas = node->nas;
	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	vfs_get_relative_path(nod, (char *) path, PATH_MAX);

	if(DFS_OK == fat_open_file(nas, (uint8_t *)path, flag, sector_buff)) {
		fi->pointer = desc->cursor;

		return 0;
	}
	return -1;
}

static int fatfs_close(struct file_desc *desc) {
	return 0;
}

static size_t fatfs_read(struct file_desc *desc, void *buf, size_t size) {
	size_t rezult;
	struct nas *nas;
	struct fat_file_info *fi;

	nas = desc->node->nas;
	fi = nas->fi->privdata;
	fi->pointer = desc->cursor;

	rezult = fat_read_file(nas, sector_buff, buf, &bytecount, size);
	if (DFS_OK == rezult) {
		desc->cursor = fi->pointer;
		return bytecount;
	}
	return rezult;
}

static size_t fatfs_write(struct file_desc *desc, void *buf, size_t size) {
	size_t rezult;
	struct nas *nas;
	struct fat_file_info *fi;

	nas = desc->node->nas;
	fi = nas->fi->privdata;
	fi->pointer = desc->cursor;

	rezult = fat_write_file(nas, sector_buff, (uint8_t *)buf,
			&bytecount, size);
	if (DFS_OK == rezult) {
		desc->cursor = fi->pointer;
		return bytecount;
	}
	return rezult;
}

static int fatfs_ioctl(struct file_desc *desc, int request, ...) {
	return 0;
}


/* File system operations */

static int fatfs_init(void * par);
static int fatfs_format(void * bdev);
static int fatfs_mount(void * dev, void *dir);
static int fatfs_create(struct path *parent_node, struct path *new_node);
static int fatfs_delete(struct path *node);
static int fatfs_truncate (struct path *node, off_t length);
static int fatfs_umount(void *dir);

static struct fsop_desc fatfs_fsop = {
	.init = fatfs_init,
	.format = fatfs_format,
	.mount = fatfs_mount,
	.create_node = fatfs_create,
	.delete_node = fatfs_delete,
	.truncate = fatfs_truncate,
	.umount = fatfs_umount,
};

static const struct fs_driver fatfs_driver = {
	.name = "reffat",
	.file_op = &fatfs_fop,
	.fsop = &fatfs_fsop,
};

static int fatfs_init(void * par) {
	return 0;
}

static int fatfs_format(void *dev) {
	node_t *dev_node;
	struct nas *dev_nas;

	if (NULL == (dev_node = dev)) {
		return -ENODEV;/*device not found*/
	}

	dev_nas = dev_node->nas;

	/* private file_info for dev node is *bdev */
	fat_create_partition(dev_nas->fi->privdata);
	fat_root_dir_record(dev_nas->fi->privdata);

	return 0;
}

static int fatfs_mount(void *dev, void *dir) {
	struct node *dir_node, *dev_node;
	struct nas *dir_nas, *dev_nas;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	struct node_fi *dev_fi;
	int rc;

	dev_node = dev;
	dev_nas = dev_node->nas;
	dir_node = dir;
	dir_nas = dir_node->nas;

	if (NULL == (dev_fi = dev_nas->fi)) {
		rc =  -ENODEV;
	}

	if (NULL == (dir_nas->fs = filesystem_create("vfat"))) {
		rc =  -ENOMEM;
	}

	dir_nas->fs->bdev = dev_fi->privdata;

	/* allocate this fs info */
	if(NULL == (fsi = pool_alloc(&fat_fs_pool))) {
		rc =  -ENOMEM;
		goto error;
	}
	memset(fsi, 0, sizeof(struct fat_fs_info));
	dir_nas->fs->fsi = fsi;
	vfs_get_path_by_node(dir_node, fsi->mntto);

	/* allocate this directory info */
	if(NULL == (fi = pool_alloc(&fat_file_pool))) {
		rc =  -ENOMEM;
		goto error;
	}
	memset(fi, 0, sizeof(struct fat_file_info));
	dir_nas->fi->privdata = (void *) fi;

	return fat_mount_files(dir_nas);

	error:
	fat_free_fs(dir_nas);
	/* TODO */
//	dir_nas->fi->privdata = prev_fi;
//	dir_nas->fs = prev_fs;
	return rc;
}

static int fatfs_create(struct node *parent_node, struct node *node) {
	struct fat_file_info *fi;
	struct nas *parent_nas, *nas;
	int rc;

	assert(parent_node && node);

	if (0 != (rc = fat_check_filename(node->name))) {
		return -rc;
	}

	nas = node->nas;
	parent_nas = parent_node->nas;

	fi = fat_fi_alloc(nas, parent_nas->fs);
	if (!fi) {
		return -ENOMEM;
	}

	if(0 != fat_create_file(parent_node, node)) {
		return -EIO;
	}

	return 0;
}

static int fatfs_delete(struct node *node) {
	struct nas *nas;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	char path [PATH_MAX];

	nas = node->nas;
	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	vfs_get_relative_path(node, path, PATH_MAX);

	/* delete file system descriptor when delete root dir */
	if(0 == *path) {
		pool_free(&fat_fs_pool, fsi);
	}
	else {
		if (node_is_directory(node)) {
			if(fat_unlike_directory(nas, (uint8_t *) path,
				(uint8_t *) sector_buff)) {
				return -1;
			}
		}
		else {
			/* delete file from fat fs*/
			if(fat_unlike_file(nas, (uint8_t *) path,
				(uint8_t *) sector_buff)) {
				return -1;
			}
		}
	}
	pool_free(&fat_file_pool, fi);

	vfs_del_leaf(node);
	return 0;
}

static int fatfs_truncate(struct node *node, off_t length) {
	struct nas *nas = node->nas;

	nas->fi->ni.size = length;

	/* TODO realloc blocks*/

	return 0;
}

static int fatfs_umount(void *dir) {
	struct node *dir_node;
	struct nas *dir_nas;

	dir_node = dir;
	dir_nas = dir_node->nas;

	/* delete all entry node */
	fat_umount_entry(dir_nas);

	/* free fat file system pools and buffers*/
	fat_free_fs(dir_nas);

	return 0;
}

const struct fs_driver *fatfs_get_fs(void) {
    return &fatfs_driver;
}

DECLARE_FILE_SYSTEM_DRIVER(fatfs_driver);

