/**
 * @brief
 *
 * @date 02.01.24
 * @author Aleksey Zhmulin
 */

#ifndef FS_VFS_CORE_H_
#define FS_VFS_CORE_H_

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <time.h>

#include <framework/mod/options.h>
#include <kernel/spinlock.h>
#include <kernel/thread/sync/mutex.h>
#include <lib/cwalk.h>
#include <lib/libds/array.h>
#include <lib/libds/dlist.h>

/* Special inode values */
#define VFS_BAD_INO ((ino_t)-1) /* Invalid inode */
#define VFS_MPT_INO ((ino_t)-2) /* Mount point inode */

#define VFS_DRIVER_DEF(drv)                                                   \
	ARRAY_SPREAD_DECLARE(const struct vfs_driver *const, __vfs_drv_registry); \
	ARRAY_SPREAD_ADD(__vfs_drv_registry, &drv)

struct idesc;
struct dirent;
struct block_dev;
struct vfs_driver;
struct super_block;

struct inode {
	struct super_block *sb;
	ino_t ino; /* inode number */
};

struct inode_info {
	struct timespec mtime;
	mode_t mode;
	uid_t owner;
	gid_t group;
};

struct super_block {
	const struct vfs_driver *drv;
	struct dlist_head list_item;
	struct dlist_head mount_list;
	struct inode mount_point;
	struct inode_info info;
	spinlock_t lock;
	int usage_count;
	bool bind_mount;
};

struct vfs_driver_ops {
	struct super_block *(*mount)(struct block_dev *bdev);
	int (*umount)(struct super_block *sb);
	int (*mkfs)(struct block_dev *bdev);

	struct idesc *(*open)(const struct inode *inode, int oflag);
	int (*mkfile)(const struct inode *dir_inode, const char *name,
	    struct inode *new_inode);
	int (*mkdir)(const struct inode *dir_inode, const char *name,
	    struct inode *new_inode);
	int (*remove)(const struct inode *dir_inode, const struct inode *inode);
	int (*rename)(const struct inode *inode, const char *new_name);

	void (*lock)(const struct inode *inode);
	void (*unlock)(const struct inode *inode);
	void (*load_info)(const struct inode *inode, struct inode_info *info);
	void (*save_info)(const struct inode *inode, const struct inode_info *info);

	int (*readdir)(const struct inode *dir_inode, struct inode *pos,
	    struct dirent *dirent);
	int (*lookup)(const struct inode *dir_inode, const char *name,
	    struct inode *lookup);
};

struct vfs_driver {
	const char *name;
	struct vfs_driver_ops ops;
};

__BEGIN_DECLS

extern const struct vfs_driver *vfs_driver_find(const char *name);
extern struct super_block *vfs_get_rootfs(void);

extern void vfs_super_block_init(struct super_block *sb,
    const struct vfs_driver *drv);
extern void vfs_super_block_deinit(struct super_block *sb);
extern struct super_block *vfs_super_block_mount(const char *name,
    struct block_dev *bdev, bool bind_mount);
extern int vfs_super_block_unmount(struct super_block *sb);

extern void vfs_inode_ops_lock(void);
extern void vfs_inode_ops_unlock(void);
extern void vfs_inode_get_root(struct inode *inode);
extern bool vfs_inode_is_bad(const struct inode *inode);
extern bool vfs_inode_is_root(const struct inode *inode);
extern bool vfs_inode_is_normal(const struct inode *inode);
extern bool vfs_inode_is_mount_point(const struct inode *inode);
extern bool vfs_inode_is_modified(const struct inode *inode,
    struct timespec *old);
extern bool vfs_inode_is_directory(const struct inode *inode);
extern bool vfs_inode_is_available(const struct inode *inode, int access_mode);
extern void vfs_inode_load_info(const struct inode *inode,
    struct inode_info *info);
extern void vfs_inode_save_info(const struct inode *inode,
    const struct inode_info *info);
extern void vfs_inode_get_mtime(const struct inode *inode,
    struct timespec *mtime);
extern void vfs_inode_update_mtime(const struct inode *inode);
extern mode_t vfs_inode_get_mode(const struct inode *inode);
extern void vfs_inode_set_mode(const struct inode *inode, mode_t mode);
extern uid_t vfs_inode_get_owner(const struct inode *inode);
extern void vfs_inode_set_owner(const struct inode *inode, uid_t owner);
extern gid_t vfs_inode_get_group(const struct inode *inode);
extern void vfs_inode_set_group(const struct inode *inode, gid_t group);
extern void vfs_inode_lock(const struct inode *inode);
extern void vfs_inode_unlock(const struct inode *inode);
extern void vfs_inode_init(const struct inode *inode, mode_t mode);
extern struct idesc *vfs_inode_open(const struct inode *inode, int oflag);

extern void vfs_mount_point_add_next(const struct inode *mount_point,
    struct super_block *sb);
extern void vfs_mount_point_get_first_bind(const struct inode *mount_point,
    struct inode *lookup);
extern void vfs_mount_point_get_first(const struct inode *mount_point,
    struct inode *lookup);
extern void vfs_mount_point_get_last(const struct inode *mount_point,
    struct inode *lookup);
extern bool vfs_mount_point_get_next(const struct inode *mount_point,
    struct inode *lookup);
extern bool vfs_mount_point_get_prev(const struct inode *mount_point,
    struct inode *lookup);
extern bool vfs_mount_point_get_prev_bind(const struct inode *mount_point,
    struct inode *lookup);

extern struct idesc *vfs_path_open(const char *path, int oflag, mode_t mode);
extern int vfs_path_mkdir(const char *path, mode_t mode);
extern int vfs_path_lookup(const char *path, struct inode *lookup);
extern int vfs_path_remove(const char *path);

__END_DECLS

#endif /* FS_VFS_CORE_H_ */
