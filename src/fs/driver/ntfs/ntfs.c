/**
 * @file
 *
 * @brief File System Driver wrapper for NTFS
 *
 * @date Jul 23, 2013
 * @author: L'auteur est à l'avance embarassé par la qualité du code
 *          ci-dessous donc il voudrais encore rester inconnu.
 */

#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <embox/block_dev.h>
#include <limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <mem/misc/pool.h>
#include <embox/unit.h>


#include <time.h>
#define __timespec_defined

static void __x86_verificator__(void) {
	// This is to make sure this file only compiles on x86
	asm ("mov %cr2, %eax");
}
#define	__LITTLE_ENDIAN	1234
#define	__BIG_ENDIAN	4321
#define	__PDP_ENDIAN	3412
// x86 architecture only
#define __BYTE_ORDER __LITTLE_ENDIAN

#include <ntfs-3g/volume.h>
#include <ntfs-3g/device.h>
#include <ntfs-3g/cache.h>
#include <ntfs-3g/misc.h>
#include <ntfs-3g/dir.h>


struct ntfs_fs_info {
	struct ntfs_device *ntfs_dev;
	ntfs_volume *ntfs_vol;
	char mntto[PATH_MAX];
};

struct ntfs_file_info {
	MFT_REF mref;
};


/* ntfs filesystem description pool */
POOL_DEF(ntfs_fs_pool, struct ntfs_fs_info,
		OPTION_GET(NUMBER,ntfs_descriptor_quantity));

/* ntfs file description pool */
POOL_DEF(ntfs_file_pool, struct ntfs_file_info,
		OPTION_GET(NUMBER,ntfs_inode_quantity));


static int embox_ntfs_node_create(struct node *parent_node, struct node *new_node) {
	(void)__x86_verificator__;
	return 0;
}

static int embox_ntfs_node_delete(struct node *nod) {
	return 0;
}

extern struct ntfs_device_operations ntfs_device_bdev_io_ops;

static int embox_ntfs_filldir(void *dirent, const ntfschar *name,
		const int name_len, const int name_type, const s64 pos,
		const MFT_REF mref, const unsigned dt_type) {
	struct nas *dir_nas = dirent;

	{
		char filename[PATH_MAX];
		// Add this bullshit due to shitty API
		char *filename_ptr = filename;

		if(ntfs_ucstombs(name, name_len, &filename_ptr, PATH_MAX)) {
			// ToDo: error
		}

		vfs_create(dir_nas->node, filename, 0/*ToDo: mode*/);

	}

	ntfs_inode_open(NULL/*vol*/, mref);

        return 0;
}

static int embox_ntfs_simultaneous_mounting_descend(struct nas *dir_nas, ntfs_inode *ni) {
	struct ntfs_file_info *fi;
	int rc;
	s64 pos;

	if (NULL == (fi = pool_alloc(&ntfs_file_pool))) {
		dir_nas->fi->privdata = (void *) fi;
		rc = ENOMEM;
		goto error;
	}

	memset(fi, 0, sizeof(*fi));
	dir_nas->fi->privdata = (void *) fi;

	// ToDo: remplir la structure de l'inode
	fi->mref = ni->mft_no;

	pos = 0;
    if (ni->mrec->flags & MFT_RECORD_IS_DIRECTORY) {
    	rc = ntfs_readdir(ni, &pos, dir_nas, embox_ntfs_filldir);
    	if (rc) {
    		// ToDo:
    	}
    }
 error:
    // ToDo:
    return -1;
}

static int embox_ntfs_mount(void *dev, void *dir) {
	ntfs_volume *vol;

	int rc;
	struct node *dir_node, *dev_node;
	struct nas *dir_nas, *dev_nas;
	struct node_fi *dev_fi;
	struct ntfs_device *ntfs_dev;
	struct ntfs_fs_info *fsi;
	ntfs_inode *ni;


	dev_node = dev;
	dev_nas = dev_node->nas;
	dir_node = dir;
	dir_nas = dir_node->nas;

	if (NULL == (dev_fi = dev_nas->fi)) {
		rc = ENODEV;
		return -rc;
	}

	if(NULL != vfs_get_child_next(dir_node)) {
		return -ENOTEMPTY;
	}

	if (NULL == (dir_nas->fs = filesystem_create("ntfs"))) {
		rc = ENOMEM;
		goto error;
	}

	dir_nas->fs->bdev = dev_fi->privdata;

	/* allocate this fs info */
	if (NULL == (fsi = pool_alloc(&ntfs_fs_pool))) {
		dir_nas->fs->fsi = fsi;
		rc = ENOMEM;
		goto error;
	}
	memset(fsi, 0, sizeof(*fsi));
	dir_nas->fs->fsi = fsi;
	vfs_get_path_by_node(dir_node, fsi->mntto);

	/* Allocate an ntfs_device structure. */
	ntfs_dev = ntfs_device_alloc(dir_nas->fs->bdev->name, 0, &ntfs_device_bdev_io_ops, NULL);
	if (!ntfs_dev) {
		rc = ENOMEM;
		goto error;
	}
	/* Call ntfs_device_mount() to do the actual mount. */
	vol = ntfs_device_mount(ntfs_dev, 0/*flags*/);
	if (!vol) {
		int eo = errno;
		ntfs_device_free(ntfs_dev);
		errno = eo;
		rc = errno;
		goto error;
	} else
		ntfs_create_lru_caches(vol);

	fsi->ntfs_dev = ntfs_dev;
	fsi->ntfs_vol = vol;

	if (NULL == (ni = ntfs_pathname_to_inode(vol, NULL, "/"))) {
		// ToDo: free qch
		rc = errno;
		goto error;
	}

        rc = embox_ntfs_simultaneous_mounting_descend(dir_node->nas, ni);
	if (rc) {
		// ToDo: free qch
		goto error;
	}

	return 0;

	error:
	//ext2_free_fs(dir_nas);

	return -rc;
}


struct ntfs_bdev_desc {
	block_dev_t *dev;
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
	node_t *dev_node;

	if (NDevOpen(dev)) {
		errno = EBUSY;
		return -1;
	}

	/* Always a block device */
	NDevSetBlock(dev);

	dev->d_private = ntfs_malloc(sizeof(struct ntfs_bdev_desc));
	if (!dev->d_private)
		return -1;

	dev_node = vfs_lookup_child(vfs_lookup_child(0,"dev"),dev->d_name);
	if (dev_node) {
		((struct ntfs_bdev_desc*)dev->d_private)->dev = dev_node->nas->fi->privdata;
		((struct ntfs_bdev_desc*)dev->d_private)->pos = 0;
	}
	if (!((struct ntfs_bdev_desc*)dev->d_private)->dev) {
		err = ENODEV;
		goto err_out;
	}

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
	block_dev_t *bdev = ((struct ntfs_bdev_desc*)dev->d_private)->dev;
	int blksize = block_dev_ioctl(bdev, IOCTL_GETBLKSIZE, NULL, 0);
	if ((offset%blksize) || (count%blksize)) {
		errno = EINVAL;
		return -1;
	}
	if (count == block_dev_read(bdev, buf, count, offset/blksize)) {
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
	block_dev_t *bdev = ((struct ntfs_bdev_desc*)dev->d_private)->dev;
	int blksize = block_dev_ioctl(bdev, IOCTL_GETBLKSIZE, NULL, 0);
	if (NDevReadOnly(dev)) {
		errno = EROFS;
		return -1;
	}
	NDevSetDirty(dev);
	if ((offset%blksize) || (count%blksize)) {
		errno = EINVAL;
		return -1;
	}
	if (!block_dev_write(bdev, buf, count/blksize, offset/blksize)) {
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
	block_dev_t *bdev = ((struct ntfs_bdev_desc*)dev->d_private)->dev;
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
	.create_node = embox_ntfs_node_create,
	.delete_node = embox_ntfs_node_delete,
	.mount = embox_ntfs_mount,
};

static const struct fs_driver ntfs_driver = {
	.name = "ntfs",
	.file_op = NULL,
	.fsop = &ntfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(ntfs_driver);
