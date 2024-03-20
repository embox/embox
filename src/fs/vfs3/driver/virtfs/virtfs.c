/**
 * @brief virtfs driver
 *
 * @date 02.01.24
 * @author Aleksey Zhmulin
 */

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <framework/mod/options.h>
#include <lib/libds/bitmap.h>
#include <lib/libds/dlist.h>
#include <mem/misc/pool.h>
#include <util/err.h>
#include <util/log.h>
#include <vfs/core.h>
#include <vfs/dir_idesc.h>

struct virtfs_dir {
	struct dlist_head list_item;
	struct dlist_head child_list;
	struct inode_info info;
	int usage_count;
	ino_t parent;
	char name[NAME_MAX];
};

struct virtfs_super_block {
	struct super_block sb;
	struct dlist_head dir_list;
};

#define SB_POOL_SIZE  OPTION_GET(NUMBER, sb_pool_size)
#define DIR_POOL_SIZE OPTION_GET(NUMBER, node_pool_size)

struct idesc;
struct block_dev;

static const struct dir_idesc_ops virtfs_dir_ops;

static BITMAP_DECL(virtfs_ino_table, DIR_POOL_SIZE);
static struct virtfs_dir virtfs_dir_pool[DIR_POOL_SIZE];

POOL_DEF(virtfs_sb_pool, struct virtfs_super_block, SB_POOL_SIZE);

static ino_t virtfs_dir2ino(const struct virtfs_dir *dir) {
	return (ino_t)(dir - virtfs_dir_pool);
}

static struct virtfs_dir *virtfs_ino2dir(ino_t ino) {
	return &virtfs_dir_pool[ino];
}

static struct virtfs_dir *virtfs_dir_alloc(void) {
	ino_t ino;

	ino = bitmap_find_zero_bit(virtfs_ino_table, DIR_POOL_SIZE, 0);
	if (ino == DIR_POOL_SIZE) {
		log_error("failed to allocate directory");
		return NULL;
	}

	bitmap_set_bit(virtfs_ino_table, ino);

	return virtfs_ino2dir(ino);
}

static void virtfs_dir_free(struct virtfs_dir *dir) {
	bitmap_clear_bit(virtfs_ino_table, virtfs_dir2ino(dir));
}

static struct virtfs_dir *virtfs_inode2dir(const struct inode *inode) {
	assert(vfs_inode_is_normal(inode));

	return virtfs_ino2dir(inode->ino);
}

static struct dlist_head *virtfs_inode2list(const struct inode *inode) {
	struct virtfs_dir *dir;

	if (vfs_inode_is_mount_point(inode)) {
		return &((struct virtfs_super_block *)inode->sb)->dir_list;
	}

	dir = virtfs_inode2dir(inode);

	return &dir->child_list;
}

static struct idesc *virtfs_open(const struct inode *inode, int oflag) {
	struct virtfs_dir *dir;

	if (oflag & O_PATH) {
		return dir_idesc_open(NULL, NULL, oflag);
	}

	dir = virtfs_inode2dir(inode);

	assert(dir->usage_count >= 0);
	dir->usage_count++;

	return dir_idesc_open(inode, &virtfs_dir_ops, oflag);
}

static int virtfs_mkdir(const struct inode *dir_inode, const char *name,
    struct inode *new_inode) {
	struct dlist_head *dir_list;
	struct virtfs_dir *dir;

	dir = virtfs_dir_alloc();
	if (!dir) {
		return -ENOMEM;
	}

	strcpy(dir->name, name);
	dir->parent = dir_inode->ino;
	dir->usage_count = 0;

	dlist_init(&dir->list_item);
	dlist_init(&dir->child_list);

	dir_list = virtfs_inode2list(dir_inode);
	dlist_add_next(&dir->list_item, dir_list);

	if (new_inode) {
		new_inode->ino = virtfs_dir2ino(dir);
		new_inode->sb = dir_inode->sb;
	}

	return 0;
}

static int virtfs_remove(const struct inode *dir_inode,
    const struct inode *inode) {
	struct virtfs_dir *dir;

	dir = virtfs_inode2dir(inode);

	if (dir->usage_count > 0) {
		return -EBUSY;
	}

	if (!dlist_empty(&dir->child_list)) {
		return -ENOTEMPTY;
	}

	dlist_del_init(&dir->list_item);
	virtfs_dir_free(dir);

	return 0;
}

static int virtfs_rename(const struct inode *inode, const char *new_name) {
	struct virtfs_dir *dir;

	dir = virtfs_inode2dir(inode);

	strcpy(dir->name, new_name);

	return 0;
}

static void virtfs_lock(const struct inode *inode) {
	struct virtfs_dir *dir;

	dir = virtfs_inode2dir(inode);

	assert(dir->usage_count >= 0);
	dir->usage_count++;
}

static void virtfs_unlock(const struct inode *inode) {
	struct virtfs_dir *dir;

	dir = virtfs_inode2dir(inode);

	assert(dir->usage_count > 0);
	dir->usage_count--;
}

static void virtfs_load_info(const struct inode *inode,
    struct inode_info *info) {
	struct virtfs_dir *dir;

	dir = virtfs_inode2dir(inode);

	memcpy(info, &dir->info, sizeof(struct inode_info));
}

static void virtfs_save_info(const struct inode *inode,
    const struct inode_info *info) {
	struct virtfs_dir *dir;

	dir = virtfs_inode2dir(inode);

	memcpy(&dir->info, info, sizeof(struct inode_info));
}

static int virtfs_readdir(const struct inode *dir_inode, struct inode *pos,
    struct dirent *dirent) {
	struct dlist_head *dir_list;
	struct virtfs_dir *dir;

	dir_list = virtfs_inode2list(dir_inode);

	if (vfs_inode_is_bad(pos)) {
		dir = dlist_first_entry_or_null(dir_list, struct virtfs_dir, list_item);
	}
	else {
		dir = virtfs_inode2dir(pos);
		dir = dlist_next_entry_if_not_last(dir, dir_list, struct virtfs_dir,
		    list_item);
	}

	if (!dir) {
		return -ENOENT;
	}

	pos->ino = virtfs_dir2ino(dir);

	dirent->d_ino = 0;
	strcpy(dirent->d_name, dir->name);

	return 0;
}

static int virtfs_lookup(const struct inode *dir_inode, const char *name,
    struct inode *lookup) {
	struct dlist_head *dir_list;
	struct virtfs_dir *dir;

	dir_list = virtfs_inode2list(dir_inode);

	dlist_foreach_entry(dir, dir_list, list_item) {
		if (!strcmp(dir->name, name)) {
			lookup->ino = virtfs_dir2ino(dir);
			lookup->sb = dir_inode->sb;
			return 0;
		}
	}

	return -ENOENT;
}

static struct super_block *virtfs_mount(struct block_dev *bdev) {
	struct virtfs_super_block *virtfs_sb;

	virtfs_sb = pool_alloc(&virtfs_sb_pool);
	if (!virtfs_sb) {
		return err2ptr(ENOMEM);
	}

	dlist_init(&virtfs_sb->dir_list);

	return (struct super_block *)virtfs_sb;
}

static void virtfs_remove_recursively(struct virtfs_dir *dir) {
	struct virtfs_dir *child;

	dlist_foreach_entry(child, &dir->child_list, list_item) {
		virtfs_remove_recursively(child);
	}

	virtfs_dir_free(dir);
}

static int virtfs_umount(struct super_block *sb) {
	struct virtfs_super_block *virtfs_sb;
	struct virtfs_dir *dir;

	virtfs_sb = (struct virtfs_super_block *)sb;

	dlist_foreach_entry(dir, &virtfs_sb->dir_list, list_item) {
		virtfs_remove_recursively(dir);
	}

	vfs_super_block_deinit(sb);

	pool_free(&virtfs_sb_pool, sb);

	return 0;
}

static void virtfs_dir_close(const struct inode *inode) {
	struct virtfs_dir *dir;

	dir = virtfs_inode2dir(inode);

	assert(dir->usage_count > 0);
	dir->usage_count--;
}

static const struct dir_idesc_ops virtfs_dir_ops = {
    .close = virtfs_dir_close,
};

static const struct vfs_driver virtfs_driver = {
    .name = "virtfs",
    .ops = ((struct vfs_driver_ops){
        .mount = virtfs_mount,
        .umount = virtfs_umount,
        .open = virtfs_open,
        .mkdir = virtfs_mkdir,
        .remove = virtfs_remove,
        .rename = virtfs_rename,
        .lock = virtfs_lock,
        .unlock = virtfs_unlock,
        .load_info = virtfs_load_info,
        .save_info = virtfs_save_info,
        .readdir = virtfs_readdir,
        .lookup = virtfs_lookup,
    }),
};

VFS_DRIVER_DEF(virtfs_driver);
