/**
 * @file
 * @brief  DVFS interface implementation
 * @author Denis Deryugin
 * @date   11 Mar 2014
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include <drivers/block_dev.h> /* block_dev_block_size */
#include <fs/dentry.h>
#include <fs/file_desc.h>
#include <fs/kfile.h>
#include <fs/dvfs.h>

#include <util/atomic_rmw.h>
#include <util/math.h>

/**
 * @brief Uninitialize file descriptor
 * @param desc File descriptor to be uninitialized
 *
 * @return Negative error code
 * @retval  0 Ok
 * @retval -1 Descriptor fields are inconsistent
 */
int kclose(struct file_desc *desc) {
	if (!desc || !desc->f_dentry)
		return -1;

	if (!(desc->f_dentry->flags & VFS_DIR_VIRTUAL)) {
		assert(desc->f_ops);
	}

	/* The driver is told only about a file it still has. */
	if (desc->f_ops && desc->f_ops->close && desc->f_inode
	    && 0 == dvfs_file_valid(desc)) {
		desc->f_ops->close(desc);
	}

	/* Frees a dentry whose name was removed while this was open */
	dentry_ref_dec(desc->f_dentry);

	dvfs_destroy_file(desc);
	return 0;
}

/**
 * @brief Application level interface to write the file
 * @param desc  File to be written
 * @param buf   Source of the data
 * @param count Length of the data
 *
 * @return Bytes written or negative error code
 * @retval -ENOSYS Function is not implemented in file system driver
 * @retval  -EBADF The file was removed while this was open
 */
int kwrite(struct file_desc *desc, char *buf, int count) {
	struct inode *inode;
	int res;

	if (!desc) {
		return -EINVAL;
	}

	inode = desc->f_inode;
	assert(inode);

	if ((res = dvfs_file_valid(desc))) {
		return res;
	}

	if (!desc->f_ops || !desc->f_ops->write) {
		return -ENOSYS;
	}

	res = desc->f_ops->write(desc, buf, count);
	if (res > 0) {
		desc->f_pos += res;
		/* The size goes up after the data, so another core never reads a size
		 * covering unwritten blocks. Drivers keeping the size do it already. */
		if (!(inode->i_mode & DVFS_NO_LSEEK)
		    && desc->f_pos > (off_t) inode->i_size) {
			inode->i_size = desc->f_pos;
		}
	}

	/* What the driver wrote, which may be short of COUNT. */
	return res;
}

/**
 * @brief Application level interface to read the file
 * @param desc  File to be read
 * @param buf   Destination
 * @param count Length of the data
 *
 * @return Bytes read or negative error code
 * @retval -ENOSYS Function is not implemented in file system driver
 * @retval  -EBADF The file was removed while this was open
 */
int kread(struct file_desc *desc, char *buf, int count) {
	off_t size, pos;
	int res;

	if (!desc) {
		return -1;
	}

	if ((res = dvfs_file_valid(desc))) {
		return res;
	}

	/* Read the size once and clamp in signed arithmetic: f_pos may be past
	 * the end, and a write on another core may change the size. */
	size = (off_t) atomic_rmw_load(&desc->f_inode->i_size, __ATOMIC_RELAXED);
	pos = desc->f_pos;
	if (!(desc->f_inode->i_mode & DVFS_NO_LSEEK)) {
		if (pos >= size) {
			return 0;
		}
		if ((off_t) count > size - pos) {
			count = (int) (size - pos);
		}
	}

	if (count <= 0) {
		return 0;
	}

	if (desc->f_ops && desc->f_ops->read) {
		res = desc->f_ops->read(desc, buf, count);
	}
	else {
		return -ENOSYS;
	}

	if (res > 0) {
		desc->f_pos += res;
	}

	return res;
}

int kfstat(struct file_desc *desc, struct stat *sb) {
	size_t block_size;
	int res;

	if ((res = dvfs_file_valid(desc))) {
		return res;
	}

	if (desc->f_inode == NULL) {
		/* A virtual directory made under another virtual one has no inode */
		*sb = (struct stat){ .st_mode = S_IFDIR | S_IRWXA };
		return 0;
	}

	*sb = (struct stat){
	    .st_size = desc->f_inode->i_size,
	    .st_mode = desc->f_inode->i_mode,
	    .st_ino = desc->f_inode->i_no,
	    .st_uid = desc->f_inode->i_owner_id,
	    .st_gid = desc->f_inode->i_group_id,
	    .st_mtime = desc->f_inode->i_mtime,
	    .st_ctime = desc->f_inode->i_ctime,
	};

	sb->st_blocks = sb->st_size;

	if (desc->f_inode->i_sb && desc->f_inode->i_sb->bdev) {
		block_size = block_dev_block_size(desc->f_inode->i_sb->bdev);
		sb->st_blocks /= block_size;
		sb->st_blocks += ((sb->st_blocks % block_size) != 0);
	}

	return 0;
}

int kioctl(struct file_desc *fp, int request, void *data) {
	int res;

	if ((res = dvfs_file_valid(fp))) {
		return res;
	}
	if (!fp->f_ops || !fp->f_ops->ioctl) {
		return -ENOTTY;
	}
	return fp->f_ops->ioctl(fp, request, data);
}
