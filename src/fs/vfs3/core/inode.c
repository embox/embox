/**
 * @brief
 *
 * @date 02.01.24
 * @author Aleksey Zhmulin
 */

#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <kernel/spinlock.h>
#include <kernel/thread/sync/mutex.h>
#include <lib/libds/dlist.h>
#include <util/err.h>
#include <vfs/core.h>

struct idesc;
struct block_dev;

static struct mutex inode_mutex = MUTEX_INIT(inode_mutex);

void vfs_inode_ops_lock(void) {
	mutex_lock(&inode_mutex);
}

void vfs_inode_ops_unlock(void) {
	mutex_unlock(&inode_mutex);
}

void vfs_inode_get_root(struct inode *inode) {
	assert(inode);

	inode->ino = VFS_MPT_INO;
	inode->sb = vfs_get_rootfs();
}

bool vfs_inode_is_root(const struct inode *inode) {
	struct inode root;

	assert(inode);
	assert(inode->sb);

	vfs_inode_get_root(&root);

	return (inode->ino == root.ino) && (inode->sb == root.sb);
}

bool vfs_inode_is_bad(const struct inode *inode) {
	assert(inode);
	assert(inode->sb);

	return inode->ino == VFS_BAD_INO;
}

bool vfs_inode_is_mount_point(const struct inode *inode) {
	assert(inode);
	assert(inode->sb);

	return inode->ino == VFS_MPT_INO;
}

bool vfs_inode_is_normal(const struct inode *inode) {
	assert(inode);
	assert(inode->sb);

	return (inode->ino != VFS_BAD_INO) && (inode->ino != VFS_MPT_INO);
}

bool vfs_inode_is_modified(const struct inode *inode, struct timespec *old) {
	struct timespec mtime;

	assert(!vfs_inode_is_bad(inode));

	vfs_inode_get_mtime(inode, &mtime);

	return (mtime.tv_sec != old->tv_sec) || (mtime.tv_nsec != old->tv_nsec);
}

bool vfs_inode_is_directory(const struct inode *inode) {
	assert(!vfs_inode_is_bad(inode));

	return vfs_inode_is_mount_point(inode)
	       || S_ISDIR(vfs_inode_get_mode(inode));
}

void vfs_inode_init(const struct inode *inode, mode_t mode) {
	struct inode_info info;

	clock_gettime(CLOCK_REALTIME, &info.mtime);
	info.owner = getuid();
	info.group = getgid();
	info.mode = mode;

	vfs_inode_save_info(inode, &info);
}

void vfs_inode_load_info(const struct inode *inode, struct inode_info *info) {
	assert(!vfs_inode_is_bad(inode));

	if (vfs_inode_is_mount_point(inode)) {
		memcpy(info, &inode->sb->info, sizeof(struct inode_info));
	}
	else {
		inode->sb->drv->ops.load_info(inode, info);
	}
}

void vfs_inode_save_info(const struct inode *inode,
    const struct inode_info *info) {
	assert(!vfs_inode_is_bad(inode));

	if (vfs_inode_is_mount_point(inode)) {
		memcpy(&inode->sb->info, info, sizeof(struct inode_info));
	}
	else {
		inode->sb->drv->ops.save_info(inode, info);
	}
}

void vfs_inode_get_mtime(const struct inode *inode, struct timespec *mtime) {
	struct inode_info info;

	vfs_inode_load_info(inode, &info);
	memcpy(mtime, &info.mtime, sizeof(struct timespec));
}

void vfs_inode_update_mtime(const struct inode *inode) {
	struct inode_info info;
	struct timespec mtime;

	clock_gettime(CLOCK_REALTIME, &mtime);

	vfs_inode_load_info(inode, &info);
	memcpy(&info.mtime, &mtime, sizeof(struct timespec));
	vfs_inode_save_info(inode, &info);
}

mode_t vfs_inode_get_mode(const struct inode *inode) {
	struct inode_info info;

	vfs_inode_load_info(inode, &info);

	return info.mode;
}

void vfs_inode_set_mode(const struct inode *inode, mode_t mode) {
	struct inode_info info;

	vfs_inode_load_info(inode, &info);
	info.mode &= ~S_IRWXA;
	info.mode |= mode & S_IRWXA;
	vfs_inode_save_info(inode, &info);
}

uid_t vfs_inode_get_owner(const struct inode *inode) {
	struct inode_info info;

	vfs_inode_load_info(inode, &info);

	return info.owner;
}

void vfs_inode_set_owner(const struct inode *inode, uid_t owner) {
	struct inode_info info;

	vfs_inode_load_info(inode, &info);
	info.owner = owner;
	vfs_inode_save_info(inode, &info);
}

gid_t vfs_inode_get_group(const struct inode *inode) {
	struct inode_info info;

	vfs_inode_load_info(inode, &info);

	return info.owner;
}

void vfs_inode_set_group(const struct inode *inode, gid_t group) {
	struct inode_info info;

	vfs_inode_load_info(inode, &info);
	info.group = group;
	vfs_inode_save_info(inode, &info);
}

void vfs_inode_lock(const struct inode *inode) {
	bool normal;

	assert(!vfs_inode_is_bad(inode));
	assert(inode->sb->usage_count >= 0);

	normal = vfs_inode_is_normal(inode);

	spin_lock_ipl_disable(&inode->sb->lock);
	{
		inode->sb->usage_count++;
		if (normal && inode->sb->drv->ops.lock) {
			inode->sb->drv->ops.lock(inode);
		}
	}
	spin_unlock_ipl_enable(&inode->sb->lock);
}

void vfs_inode_unlock(const struct inode *inode) {
	bool normal;

	assert(!vfs_inode_is_bad(inode));
	assert(inode->sb->usage_count > 0);

	normal = vfs_inode_is_normal(inode);

	spin_lock_ipl_disable(&inode->sb->lock);
	{
		inode->sb->usage_count--;
		if (normal && inode->sb->drv->ops.unlock) {
			inode->sb->drv->ops.unlock(inode);
		}
	}
	spin_unlock_ipl_enable(&inode->sb->lock);
}

bool vfs_inode_is_available(const struct inode *inode, int access_mode) {
	struct inode_info info;
	bool write_access;
	bool read_access;

	vfs_inode_load_info(inode, &info);

	if (info.owner == getuid()) {
		write_access = info.mode & S_IRUSR;
		read_access = info.mode & S_IWUSR;
	}
	else if (info.group == getgid()) {
		write_access = info.mode & S_IRGRP;
		read_access = info.mode & S_IWGRP;
	}
	else {
		write_access = info.mode & S_IROTH;
		read_access = info.mode & S_IWOTH;
	}

	switch (access_mode & O_ACCESS_MASK) {
	case O_RDONLY:
		return read_access;

	case O_WRONLY:
		return write_access;

	default:
		return read_access & write_access;
	}
}

struct idesc *vfs_inode_open(const struct inode *inode, int oflag) {
	if (!vfs_inode_is_available(inode, oflag)) {
		return err2ptr(EACCES);
	}

	return inode->sb->drv->ops.open(inode, oflag);
}
