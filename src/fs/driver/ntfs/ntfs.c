/**
 * @file
 *
 * @brief File System Driver wrapper for NTFS
 *
 * @date Jul 23, 2013
 * @author: L'auteur est à l'avance embarassé par la qualité du code
 *          ci-dessous donc il voudrait encore rester inconnu.
 */

#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <endian.h>
#include <libgen.h>
#include <unistd.h>
#include <stdio.h>

#include <fs/vfs.h>

#include <fs/fs_driver.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/file_desc.h>
#include <fs/hlpr_path.h>
#include <fs/super_block.h>
#include <fs/dir_context.h>
#include <fs/inode_operation.h>

#include <drivers/block_dev.h>

#include <mem/misc/pool.h>
#include <embox/unit.h>

#include <util/err.h>


#define __timespec_defined

#include <ntfs-3g/volume.h>
#include <ntfs-3g/device.h>
#include <ntfs-3g/cache.h>
#include <ntfs-3g/misc.h>
#include <ntfs-3g/dir.h>
#include <ntfs-3g/layout.h>

struct ntfs_fs_info {
	struct ntfs_device *ntfs_dev;
	ntfs_volume *ntfs_vol;
};

struct ntfs_file_info {
	MFT_REF mref;
};

struct ntfs_desc_info {
	ntfs_inode *ni;
	ntfs_attr *attr;
};

struct ntfs_dir_context {
	char *next_name;
	struct inode *node;
	struct dir_ctx *dir_ctx;
	int idx;
};

/* ntfs filesystem description pool */
POOL_DEF(ntfs_fs_pool, struct ntfs_fs_info,
		OPTION_GET(NUMBER,ntfs_descriptor_quantity));

/* ntfs file description pool */
POOL_DEF(ntfs_file_pool, struct ntfs_file_info,
		OPTION_GET(NUMBER,ntfs_inode_quantity));

/* ntfs open file descriptor pool */
POOL_DEF(ntfs_desc_pool, struct ntfs_desc_info,
		OPTION_GET(NUMBER,ntfs_desc_quantity));


static int ntfs_iterate(struct inode *next, char *name, struct inode *parent,
		struct dir_ctx *dir_ctx);

static int embox_ntfs_simultaneous_mounting_descend(struct inode *node, ntfs_inode *ni, bool);

static int embox_ntfs_node_create(struct inode *new_node, struct inode *parent_node, int i_mode) {
	ntfs_inode *ni, *pni;
	ntfschar *ufilename;
	int ufilename_len;
	struct ntfs_fs_info *pfsi;
	struct ntfs_file_info *pfi;
	mode_t mode;

	pfi = inode_priv(parent_node);
	pfsi = parent_node->i_sb->sb_data;

	/* ntfs_mbstoucs(...) will allocate memory for ufilename if it's NULL */
	ufilename = NULL;
	ufilename_len = ntfs_mbstoucs(inode_name(new_node), &ufilename);
	if (ufilename_len == -1) {
		return -errno;
	}

	pni = ntfs_inode_open(pfsi->ntfs_vol, pfi->mref);
	if (!pni) {
		free(ufilename);
		return -errno;
	}

	if (node_is_directory(new_node)) {
		mode = S_IFDIR;
	} else {
		mode = S_IFREG;
	}

	ni = ntfs_create(pni, 0, ufilename, ufilename_len, mode);
	if (!ni) {
		int err = errno;
		ntfs_inode_close(pni);
		free(ufilename);
		errno = err;
		return -errno;
	}

	if (embox_ntfs_simultaneous_mounting_descend(new_node, ni, false)) {
		int err = errno;
		ntfs_delete(pfsi->ntfs_vol, NULL, ni, pni, ufilename, ufilename_len);
		ntfs_inode_close(pni);
		free(ufilename);
		errno = err;
		return -errno;
	}

	if (ntfs_inode_close(pni)) {
		// ToDo: it is not exactly clear what to do in this case - IINM close does fsync.
		//       most appropriate solution would be to completely unmount file system.
		int err = errno;
		ni = ntfs_inode_open(pfsi->ntfs_vol, ((struct ntfs_file_info *)inode_priv(new_node))->mref);
		ntfs_delete(pfsi->ntfs_vol, NULL, ni, pni, ufilename, ufilename_len);
		pool_free(&ntfs_file_pool, inode_priv(new_node));
                free(ufilename);
		errno = err;
		return -errno;
	}

	free(ufilename);

	return 0;
}

static int embox_ntfs_node_delete(struct inode *node) {
	ntfs_inode *ni, *pni;
	struct inode *parent_node;
	ntfschar *ufilename;
	int ufilename_len;
	struct ntfs_fs_info *pfsi;
	struct ntfs_file_info *pfi, *fi;

	parent_node = vfs_subtree_get_parent(node);
	if (!parent_node) {
		return -EINVAL;
	}
	pfi = inode_priv(parent_node);
	pfsi = parent_node->i_sb->sb_data;
	fi = inode_priv(node);

	/* ntfs_mbstoucs(...) will allocate memory for ufilename if it's NULL */
	ufilename = NULL;
	ufilename_len = ntfs_mbstoucs(inode_name(node), &ufilename);
	if (ufilename_len == -1) {
		return -errno;
	}

	pni = ntfs_inode_open(pfsi->ntfs_vol, pfi->mref);
	if (!pni) {
		free(ufilename);
		return -errno;
	}

	ni = ntfs_inode_open(pfsi->ntfs_vol, fi->mref);
	if (!ni) {
		int err = errno;
		ntfs_inode_close(pni);
		free(ufilename);
		errno = err;
		return -errno;
	}

	if (ntfs_delete(pfsi->ntfs_vol, NULL, ni, pni, ufilename, ufilename_len)) {
		int err = errno;
		ntfs_inode_close(ni);
		ntfs_inode_close(pni);
		free(ufilename);
		errno = err;
		return -errno;
	}

	free(ufilename);

	pool_free(&ntfs_file_pool, inode_priv(node));

	if (ntfs_inode_close(pni)) {
		// ToDo: it is not exactly clear what to do in this case - IINM close does fsync.
		//       most appropriate solution would be to completely unmount file system.
		return -errno;
	}

	return 0;
}

static int embox_ntfs_truncate(struct inode *node, off_t length) {
	struct ntfs_file_info *fi;
	struct ntfs_fs_info *fsi;
	ntfs_inode *ni;
	ntfs_attr *attr;
	int ret;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;

	ni = ntfs_inode_open(fsi->ntfs_vol, fi->mref);
	if (!ni) {
		return -errno;
	}

	attr = ntfs_attr_open(ni, AT_DATA, NULL, 0);
	if (!attr) {
		int err = errno;
		ntfs_inode_close(ni);
		errno = err;
		return -errno;
	}

	ret = ntfs_attr_truncate(attr, length);

	ntfs_attr_close(attr);
	if (ntfs_inode_close(ni)) {
		return -errno;
	}

	return ret;
}


extern struct ntfs_device_operations ntfs_device_bdev_io_ops;

static mode_t ntfs_type_to_mode_fmt(const unsigned dt_type) {
	switch (dt_type) {
	case NTFS_DT_UNKNOWN: return 0;
	case NTFS_DT_FIFO:    return S_IFIFO;
	case NTFS_DT_CHR:     return S_IFCHR;
	case NTFS_DT_DIR:     return S_IFDIR;
	case NTFS_DT_BLK:     return S_IFBLK;
	case NTFS_DT_REG:     return S_IFREG;
	case NTFS_DT_LNK:     return S_IFLNK;
	case NTFS_DT_SOCK:    return S_IFSOCK;
	case NTFS_DT_WHT:     return 0; // No support for whiteout - should never happen
	default: return 0;
	}
}

static int embox_ntfs_filldir(void *dirent, const ntfschar *name,
		const int name_len, const int name_type, const s64 pos,
		const MFT_REF mref, const unsigned dt_type) {
	struct inode *dir_node = dirent;
	struct inode *node;
	struct ntfs_fs_info *fsi;
	ntfs_inode *ni;
	mode_t mode;

	if (MREF(mref) < FILE_first_user) {
		return 0;
	}

	// ToDo: it is not clear whether name_type should be checked or not

	{
		char filename[PATH_MAX];
		char *filename_ptr = filename;

		if(ntfs_ucstombs(name, name_len, &filename_ptr, PATH_MAX) < 0) {
			return -1;
		}

		if (path_is_dotname(filename, strlen(filename))) {
			return 0;
		}

		// It turned out there exist nodes with 0 type
		mode = ntfs_type_to_mode_fmt(dt_type);
		if (!mode) {
			return 0;
		}

		//
		node = vfs_subtree_create(dir_node, filename, mode);
		if (!node) {
			errno = ENOMEM;
			return -1;
		}
	}

	fsi = dir_node->i_sb->sb_data;

	// There is a room for optimization here, it is necessary to open only directory nodes
	ni = ntfs_inode_open(fsi->ntfs_vol, mref);
	if (!ni) {
		vfs_del_leaf(node);
		return -1;
	}

    return embox_ntfs_simultaneous_mounting_descend(node, ni, true);
}

static int embox_ntfs_fill_node(void *dirent, const ntfschar *name,
		const int name_len, const int name_type, const s64 pos,
		const MFT_REF mref, const unsigned dt_type) {
	struct inode *node;
	struct ntfs_fs_info *fsi;
	struct ntfs_file_info *fi;
	ntfs_inode *ni;
	mode_t mode;
	char filename[PATH_MAX];
	char *filename_ptr = filename;
	struct ntfs_dir_context *ctx = dirent;

	if (MREF(mref) < FILE_first_user) {
		return 0;
	}

	node = dirent;

	if(ntfs_ucstombs(name, name_len, &filename_ptr, PATH_MAX) < 0) {
		return -1;
	}

	if (path_is_dotname(filename, strlen(filename))) {
		return 0;
	}

	mode = ntfs_type_to_mode_fmt(dt_type);
	if (!mode) {
		return 0;
	}

	node = ctx->node;

	fsi = node->i_sb->sb_data;

	if (ctx->idx++ < (int)(uintptr_t)ctx->dir_ctx->fs_ctx) {
		return 0;
	}

	ni = ntfs_inode_open(fsi->ntfs_vol, mref);
	if (!ni) {
		return -1;
	}
	node->i_mode = mode;
	inode_size_set(node, ni->data_size);

	strncpy(ctx->next_name, filename, NAME_MAX - 1);
	ctx->next_name[NAME_MAX] = '\0';

	ctx->dir_ctx->fs_ctx = (void *)(uintptr_t)ctx->idx;

	if (NULL == (fi = pool_alloc(&ntfs_file_pool))) {
	 	ntfs_inode_close(ni);
		return -1;
	}

	memset(fi, 0, sizeof(*fi));
	inode_priv_set(node, fi);

	fi->mref = ni->mft_no;

	ntfs_inode_close(ni);
	return 1;
}

static int embox_ntfs_simultaneous_mounting_descend(struct inode *node, ntfs_inode *ni, bool close_on_err) {
	struct ntfs_file_info *fi;
	s64 pos;

	fi = 0;

	if (NULL == (fi = pool_alloc(&ntfs_file_pool))) {
		errno = ENOMEM;
		goto error;
	}

	memset(fi, 0, sizeof(*fi));
	inode_priv_set(node, fi);

	// ToDo: remplir la structure de l'inode
	// ToDo: en fait, seulement l'utilisateur et le groupe
	fi->mref = ni->mft_no;

	pos = 0;
    if (ni->mrec->flags & MFT_RECORD_IS_DIRECTORY) {
    	if (0 != ntfs_readdir(ni, &pos, node, embox_ntfs_filldir)) {
			goto error;
    	}
    }

 	ntfs_inode_close(ni);
	return 0;

 error:
 	if (close_on_err) {
 		ntfs_inode_close(ni);
 	}
    return -1;
}

static int ntfs_umount_entry(struct inode *node) {
	pool_free(&ntfs_file_pool, inode_priv(node));

	return 0;
}

static int ntfs_clean_sb(struct super_block *sb) {
	struct ntfs_fs_info *fsi = sb->sb_data;

	if (fsi->ntfs_vol) {
		// ToDo: check if everything passed Ok
		ntfs_umount(fsi->ntfs_vol, FALSE);
	}
	if (fsi->ntfs_dev) {
		// ToDo: check if everything passed Ok
		ntfs_device_free(fsi->ntfs_dev);
	}
	pool_free(&ntfs_fs_pool, fsi);

	return 0;
}

static int ntfs_destroy_inode(struct inode *inode) {
	return 0;
}

static struct super_block_operations ntfs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = ntfs_destroy_inode,
};

static struct inode_operations ntfs_iops = {
	.ino_create = embox_ntfs_node_create,
	.ino_remove = embox_ntfs_node_delete,
	.ino_iterate = ntfs_iterate,
	.ino_truncate = embox_ntfs_truncate,
};

static int ntfs_iterate(struct inode *next, char *name, struct inode *parent,
		struct dir_ctx *dir_ctx) {
	struct ntfs_fs_info *fsi;
	ntfs_volume *vol;
	ntfs_inode *ni;
	char dir_path[PATH_MAX];
	s64 pos = 0;
	struct ntfs_dir_context ntfs_dir_ctx;

	fsi = parent->i_sb->sb_data;
	vol = fsi->ntfs_vol;

	vfs_get_relative_path(parent, dir_path, sizeof(dir_path));
	ni = ntfs_pathname_to_inode(vol, NULL, dir_path);
	if (ni == NULL) {
		return -1;
	}

	next->i_sb = parent->i_sb;

	ntfs_dir_ctx.dir_ctx = dir_ctx;
	ntfs_dir_ctx.next_name = name;
	ntfs_dir_ctx.node = next;
	ntfs_dir_ctx.idx = 0;
	if (0 <= ntfs_readdir(ni, &pos, &ntfs_dir_ctx, embox_ntfs_fill_node)) {
	 	ntfs_inode_close(ni);
		return -1;
	}

 	ntfs_inode_close(ni);

	return 0;
}

static const struct file_operations ntfs_fop;
static int ntfs_fill_sb(struct super_block *sb, const char *source) {
	ntfs_volume *vol;
	struct block_dev *bdev;
	struct ntfs_fs_info *fsi;
	struct ntfs_device *ntfs_dev;

	bdev = bdev_by_path(source);
	if (NULL == bdev) {
		return -ENODEV;
	}

	sb->bdev = bdev;

	/* allocate this fs info */
	if (NULL == (fsi = pool_alloc(&ntfs_fs_pool))) {
		/* ToDo: error: exit without deallocation of filesystem */
		return -ENOMEM;
	}
	memset(fsi, 0, sizeof(*fsi));
	sb->sb_data = fsi;
	sb->sb_ops = &ntfs_sbops;
	sb->sb_iops = &ntfs_iops;
	sb->sb_fops = &ntfs_fop;

	/* Allocate an ntfs_device structure. */
	ntfs_dev = ntfs_device_alloc(block_dev_name(bdev), 0, &ntfs_device_bdev_io_ops, NULL);
	if (!ntfs_dev) {
		pool_free(&ntfs_fs_pool, fsi);
		return -ENOMEM;
	}
	/* Call ntfs_device_mount() to do the actual mount. */
	vol = ntfs_device_mount(ntfs_dev, NTFS_MNT_NONE);
	if (!vol) {
		int eo = errno;
		pool_free(&ntfs_fs_pool, fsi);
		ntfs_device_free(ntfs_dev);
		errno = eo;
		return errno;
	} else {
		// ToDo: it is probably possible not to use caches
		ntfs_create_lru_caches(vol);
	}

	fsi->ntfs_dev = ntfs_dev;
	fsi->ntfs_vol = vol;

	return 0;
}

static int embox_ntfs_mount(struct super_block *sb, struct inode *dest) {
	ntfs_volume *vol;
	int rc;
	ntfs_inode *ni;
	struct ntfs_fs_info *fsi;
	struct ntfs_file_info *fi;

	fsi = sb->sb_data;
	vol = fsi->ntfs_vol;

	if (NULL == (ni = ntfs_pathname_to_inode(vol, NULL, "/"))) {
		rc = errno;
		goto error;
	}

	if (NULL == (fi = pool_alloc(&ntfs_file_pool))) {
		errno = ENOMEM;
		goto error;
	}

	memset(fi, 0, sizeof(*fi));
	inode_priv_set(dest, fi);

	// ToDo: remplir la structure de l'inode
	// ToDo: en fait, seulement l'utilisateur et le groupe
	fi->mref = ni->mft_no;

	return 0;
error:
	ntfs_clean_sb(sb);

	return -rc;
}

static struct idesc *ntfs_open(struct inode *node, struct idesc *idesc, int __oflag)
{
	struct ntfs_file_info *fi;
	struct ntfs_fs_info *fsi;
	struct ntfs_desc_info *desc;
	ntfs_inode *ni;
	ntfs_attr *attr;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;

	// ToDo: it is not necessary to allocate dedicated structure
	//       ntfs_attr already contains pointer to ntfs_inode, so it is
	//       necessary to keep only ntfs_attr
	desc = pool_alloc(&ntfs_desc_pool);
	if (!desc) {
		return err_ptr(ENOMEM);
	}

	ni = ntfs_inode_open(fsi->ntfs_vol, fi->mref);
	if (!ni) {
		pool_free(&ntfs_desc_pool, desc);
		return err_ptr(errno);
	}

	attr = ntfs_attr_open(ni, AT_DATA, NULL, 0);
	if (!attr) {
		int err = errno;
		pool_free(&ntfs_desc_pool, desc);
		ntfs_inode_close(ni);
		errno = err;
		return err_ptr(errno);
	}

	desc->attr = attr;
	desc->ni = ni;
	file_desc_set_file_info(file_desc_from_idesc(idesc), desc);

	// Yet another bullshit: size is not valid until open
	file_set_size(file_desc_from_idesc(idesc), attr->data_size);

	return idesc;
}

static int ntfs_close(struct file_desc *file_desc)
{
	struct ntfs_desc_info *desc;
	int res;

	desc = (struct ntfs_desc_info *) file_desc->file_info;

	ntfs_attr_close(desc->attr);
	res = ntfs_inode_close(desc->ni);
	pool_free(&ntfs_desc_pool, desc);

	if (res) {
		return -errno;
	}

	return 0;
}

static size_t ntfs_read(struct file_desc *file_desc, void *buf, size_t size)
{
	struct ntfs_desc_info *desc;
	size_t res;
	off_t pos;

	pos = file_get_pos(file_desc);

	desc = file_desc->file_info;

	res = ntfs_attr_pread(desc->attr, pos, size, buf);

	return res;
}

static size_t ntfs_write(struct file_desc *file_desc, void *buf, size_t size) {
	struct ntfs_desc_info *desc;
	size_t res;
	off_t pos;

	pos = file_get_pos(file_desc);

	desc = file_desc->file_info;

	res = ntfs_attr_pwrite(desc->attr, pos, size, buf);

	if (res > 0) {
		file_set_size(file_desc, desc->attr->data_size);
	}

	return res;
}


struct ntfs_bdev_desc {
	struct block_dev *dev;
	size_t pos;
};

/**
 * ntfs_device_bdev_io_open - Open a device and lock it exclusively
 * @dev:
 * @flags:
 *
 * Description...
 *
 * Returns:
 */
static int ntfs_device_bdev_io_open(struct ntfs_device *dev, int flags)
{
	int err;
	struct block_dev *bdev;

	if (NDevOpen(dev)) {
		errno = EBUSY;
		return -1;
	}

	/* Always a block device */
	NDevSetBlock(dev);

	dev->d_private = ntfs_malloc(sizeof(struct ntfs_bdev_desc));
	if (!dev->d_private) {
		return -1;
	}

	bdev = block_dev_find(dev->d_name);
	if (!bdev) {
		err = ENODEV;
		goto err_out;
	}

	((struct ntfs_bdev_desc*)dev->d_private)->dev = bdev;
	((struct ntfs_bdev_desc*)dev->d_private)->pos = 0;

	if ((flags & O_RDWR) != O_RDWR)
		NDevSetReadOnly(dev);

	NDevSetOpen(dev);
	return 0;
err_out:
	free(dev->d_private);
	dev->d_private = NULL;
	errno = err;
	return -1;
}

/**
 * ntfs_device_bdev_io_close - Close the device, releasing the lock
 * @dev:
 *
 * Description...
 *
 * Returns:
 */
static int ntfs_device_bdev_io_close(struct ntfs_device *dev)
{
	if (!NDevOpen(dev)) {
		errno = EBADF;
		ntfs_log_perror("Device %s is not open", dev->d_name);
		return -1;
	}
	/*
	if (NDevDirty(dev))
		if (ntfs_fsync(DEV_FD(dev))) {
			ntfs_log_perror("Failed to fsync device %s", dev->d_name);
			return -1;
		}
	*/
	NDevClearOpen(dev);
	free(dev->d_private);
	dev->d_private = NULL;
	return 0;
}

/**
 * ntfs_device_bdev_io_seek - Seek to a place on the device
 * @dev:
 * @offset:
 * @whence:
 *
 * Description...
 *
 * Returns:
 */
static s64 ntfs_device_bdev_io_seek(struct ntfs_device *dev, s64 offset,
		int whence)
{
	printf(">>> ntfs_bdev_seek, dev - %s\n", dev->d_name);
	switch (whence) {
	case SEEK_SET:
		((struct ntfs_bdev_desc*)dev->d_private)->pos = offset;
		break;
	case SEEK_CUR:
		((struct ntfs_bdev_desc*)dev->d_private)->pos += offset;
		break;
	case SEEK_END:
		errno = ENOSYS;
		return -1;
	default:
		errno = EINVAL;
		return -1;
	}
	return ((struct ntfs_bdev_desc*)dev->d_private)->pos;
}

/**
 * ntfs_device_bdev_io_read - Read from the device, from the current location
 * @dev:
 * @buf:
 * @count:
 *
 * Description...
 *
 * Returns:
 */
static s64 ntfs_device_bdev_io_read(struct ntfs_device *dev, void *buf,
		s64 count)
{
	printf(">>> ntfs_bdev_read, dev - %s\n", dev->d_name);
	errno = ENOSYS;
	return -1;
}

/**
 * ntfs_device_bdev_io_write - Write to the device, at the current location
 * @dev:
 * @buf:
 * @count:
 *
 * Description...
 *
 * Returns:
 */
static s64 ntfs_device_bdev_io_write(struct ntfs_device *dev, const void *buf,
		s64 count)
{
	if (NDevReadOnly(dev)) {
		errno = EROFS;
		return -1;
	}
	NDevSetDirty(dev);
	printf(">>> ntfs_bdev_write, dev - %s\n", dev->d_name);
	errno = ENOSYS;
	return -1;
}

/**
 * ntfs_device_bdev_io_pread - Perform a positioned read from the device
 * @dev:
 * @buf:
 * @count:
 * @offset:
 *
 * Description...
 *
 * Returns:
 */
static s64 ntfs_device_bdev_io_pread(struct ntfs_device *dev, void *buf,
		s64 count, s64 offset)
{
	struct block_dev *bdev = ((struct ntfs_bdev_desc*)dev->d_private)->dev;
	if (count == block_dev_read_buffered(bdev, buf, count, offset)) {
		return count;
	}
	errno = EIO;
	return -1;
}

/**
 * ntfs_device_bdev_io_pwrite - Perform a positioned write to the device
 * @dev:
 * @buf:
 * @count:
 * @offset:
 *
 * Description...
 *
 * Returns:
 */
static s64 ntfs_device_bdev_io_pwrite(struct ntfs_device *dev, const void *buf,
		s64 count, s64 offset)
{
	struct block_dev *bdev = ((struct ntfs_bdev_desc*)dev->d_private)->dev;
	if (NDevReadOnly(dev)) {
		errno = EROFS;
		return -1;
	}
	NDevSetDirty(dev);
	if (count == block_dev_write_buffered(bdev, buf, count, offset)) {
		return count;
	}
	errno = EIO;
	return -1;
}

/**
 * ntfs_device_bdev_io_sync - Flush any buffered changes to the device
 * @dev:
 *
 * Description...
 *
 * Returns:
 */
static int ntfs_device_bdev_io_sync(struct ntfs_device *dev)
{
	int res = 0;

	if (!NDevReadOnly(dev)) {
		res = 0;//ntfs_fsync(DEV_FD(dev));
		if (res)
			ntfs_log_perror("Failed to sync device %s", dev->d_name);
		else
			NDevClearDirty(dev);
	}
	return res;
}

/**
 * ntfs_device_bdev_io_stat - Get information about the device
 * @dev:
 * @buf:
 *
 * Description...
 *
 * Returns:
 */
static int ntfs_device_bdev_io_stat(struct ntfs_device *dev, struct stat *buf)
{
	printf(">>> ntfs_bdev_read, dev - %s\n", dev->d_name);
	errno = ENOSYS;
	return -1;
}

/**
 * ntfs_device_bdev_io_ioctl - Perform an ioctl on the device
 * @dev:
 * @request:
 * @argp:
 *
 * Description...
 *
 * Returns:
 */
static int ntfs_device_bdev_io_ioctl(struct ntfs_device *dev, int request,
		void *argp)
{
	struct block_dev *bdev = ((struct ntfs_bdev_desc*)dev->d_private)->dev;
	return block_dev_ioctl(bdev, request, argp, 0);
}


struct ntfs_device_operations ntfs_device_bdev_io_ops = {
	.open		= ntfs_device_bdev_io_open,
	.close		= ntfs_device_bdev_io_close,
	.seek		= ntfs_device_bdev_io_seek,
	.read		= ntfs_device_bdev_io_read,
	.write		= ntfs_device_bdev_io_write,
	.pread		= ntfs_device_bdev_io_pread,
	.pwrite		= ntfs_device_bdev_io_pwrite,
	.sync		= ntfs_device_bdev_io_sync,
	.stat		= ntfs_device_bdev_io_stat,
	.ioctl		= ntfs_device_bdev_io_ioctl,
};

static const struct fsop_desc ntfs_fsop = {
	.mount = embox_ntfs_mount,
	.umount_entry = ntfs_umount_entry,
};

static const struct file_operations ntfs_fop = {
	.open = ntfs_open,
	.close = ntfs_close,
	.read = ntfs_read,
	.write = ntfs_write,
};

static const struct fs_driver ntfs_driver = {
	.name     = "ntfs",
	.fill_sb  = ntfs_fill_sb,
	.clean_sb = ntfs_clean_sb,
	.fsop     = &ntfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(ntfs_driver);
