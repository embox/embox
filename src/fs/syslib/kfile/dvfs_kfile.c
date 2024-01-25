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
	if (!desc || !desc->f_inode || !desc->f_dentry)
		return -1;

	if (!(desc->f_dentry->flags & VFS_DIR_VIRTUAL)) {
		assert(desc->f_ops);
	}

	if (desc->f_ops && desc->f_ops->close) {
		desc->f_ops->close(desc);
	}

	if (!dentry_ref_dec(desc->f_dentry))
		dvfs_destroy_dentry(desc->f_dentry);

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
 * @retval       0 Ok
 * @retval -ENOSYS Function is not implemented in file system driver
 */
int kwrite(struct file_desc *desc, char *buf, int count) {
	int res = 0; /* Assign to avoid compiler warning when use -O2 */
	int retcode = count;
	struct inode *inode;

	if (!desc) {
		return -EINVAL;
	}

	inode = desc->f_inode;
	assert(inode);

	if (!(inode->i_mode & DVFS_NO_LSEEK)
	    && (inode->length - desc->pos < count)) {
		if (inode->i_ops && inode->i_ops->truncate) {
			res = inode->i_ops->truncate(desc->f_inode, desc->pos + count);
			if (res) {
				retcode = -EFBIG;
			}
		}
		else {
			retcode = -EFBIG;
		}
	}

	if (desc->f_ops && desc->f_ops->write) {
		res = desc->f_ops->write(desc, buf, count);
	}
	else {
		retcode = -ENOSYS;
	}

	if (res > 0) {
		desc->pos += res;
	}

	return retcode;
}

/**
 * @brief Application level interface to read the file
 * @param desc  File to be read
 * @param buf   Destination
 * @param count Length of the data
 *
 * @return Bytes read or negative error code
 * @retval       0 Ok
 * @retval -ENOSYS Function is not implemented in file system driver
 */
int kread(struct file_desc *desc, char *buf, int count) {
	int res;
	int sz;

	if (!desc) {
		return -1;
	}

	sz = min(count, desc->f_inode->length - desc->pos);

	if (sz <= 0) {
		return 0;
	}

	if (desc->f_ops && desc->f_ops->read) {
		res = desc->f_ops->read(desc, buf, count);
	}
	else {
		return -ENOSYS;
	}

	if (res > 0) {
		desc->pos += res;
	}

	return res;
}

int kfstat(struct file_desc *desc, struct stat *sb) {
	size_t block_size;

	*sb = (struct stat){
	    .st_size = desc->f_inode->length,
	    .st_mode = desc->f_inode->i_mode,
	    .st_uid = 0,
	    .st_gid = 0,
	};

	sb->st_blocks = sb->st_size;

	if (desc->f_inode->i_sb->bdev) {
		block_size = block_dev_block_size(desc->f_inode->i_sb->bdev);
		sb->st_blocks /= block_size;
		sb->st_blocks += ((sb->st_blocks % block_size) != 0);
	}

	return 0;
}

int kioctl(struct file_desc *fp, int request, void *data) {
	return fp->f_ops->ioctl(fp, request, data);
}
