/**
 * @file
 * @brief
 *
 * @date 04.02.13
 * @author Alexander Kalmuk
 * @author Anton Kozlov
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <fs/perm.h>
#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>

#include <mem/objalloc.h>
#include <lib/libds/dlist.h>
#include <framework/mod/options.h>

#include <dirent.h>
#include <dirent_impl.h>

#define MAX_DIR_QUANTITY OPTION_GET(NUMBER, dir_quantity)

OBJALLOC_DEF(dir_pool, DIR, MAX_DIR_QUANTITY);

int dir_follow_down(DIR *dir) {
	struct inode *held;

	assert(dir);

	held = dir->path.node;
	if_mounted_follow_down(&dir->path);

	if (dir->path.node == held) {
		return 0;
	}

	/* The DIR now points into the mounted volume, so its
	 * reference has to be on that volume's root -- otherwise umount() frees
	 * it under the walk, and closedir() releases a reference it never took.
	 * Measured as inode_unref() on an inode with i_ref 0, a live superblock
	 * and an empty name, which is what a volume root looks like. */
	if (!inode_ref(dir->path.node)) {
		/* The volume went while we were stepping into it. Stay on what we
		 * already hold. */
		dir->path.node = held;
		return -ENOENT;
	}
	inode_unref(held);

	return 0;
}

DIR *opendir(const char *path) {
	struct path node_path;
	DIR *d;
	int res;

	if (!strcmp(path, ".")) {
		path = "";//getcwd(cur_path, PATH_MAX);
	}

	res = fs_perm_lookup(path, NULL, &node_path);

	if (res != 0) {
		SET_ERRNO(-res);
		return NULL;
	}

	if (!S_ISDIR(node_path.node->i_mode)) {
		SET_ERRNO(ENOTDIR);
		return NULL;
	}

	if (0 != fs_perm_check(node_path.node, S_IROTH)) {
		SET_ERRNO(EACCES);
		return NULL;
	}

	if (NULL == (d = objalloc(&dir_pool))) {
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	d->path = node_path;

	/* A DIR is a directory inode held from opendir() to
	 * closedir(), across everything the caller does in between. open() in
	 * this layer resolves its last path component through one of these, so
	 * "in between" includes a blocking call into the filesystem. Without a
	 * reference, umount() frees the directory under it. */
	if (!inode_ref(d->path.node)) {
		objfree(&dir_pool, d);
		SET_ERRNO(ENOENT);
		return NULL;
	}

	d->current.d_ino = 0;
	d->dir_context.fs_ctx = 0;

	slist_init(&d->inodes_list);

	return d;
}

int closedir(DIR *dir) {
	struct inode *node;

	if (NULL == dir) {
		SET_ERRNO(EBADF);
		return -1;
	}

	slist_foreach(node, &dir->inodes_list, dirent_link) {
		inode_del(node);
	}

	/* And let go of whatever readdir() stopped on, and of
	 * the directory itself. */
	inode_unref((struct inode *)(uintptr_t)dir->current.d_ino);
	dir->current.d_ino = 0;

	inode_unref(dir->path.node);
	dir->path.node = NULL;

	objfree(&dir_pool, dir);

	return 0;
}

struct dirent *readdir(DIR *dir) {
	struct path child;
	int res;
	struct inode *node;
	struct inode *prev;

	SET_ERRNO(0);

	if (NULL == dir) {
		SET_ERRNO(EBADF);
		return NULL;
	}
	prev = (struct inode *)(uintptr_t)dir->current.d_ino;

	if (0 != dir_follow_down(dir)) {
		return NULL;
	}

	res = vfs_get_child_next(&dir->path, prev, &child);

	/* The cursor is a pointer this walk held from one call to the next, across
	 * everything the caller did in between -- open() resolves its last path
	 * component by walking the parent this way, and unlink() on another core
	 * frees the inode it is standing on. Measured as a jump to
	 * 0x64452607e8c9aa8b, which is text, not code. So the cursor holds a
	 * reference for as long as it is the cursor, and lets go here, once the
	 * step past it has been taken. */
	inode_unref(prev);
	dir->current.d_ino = 0;

	if (0 != res) {
		return NULL;
	}

	node = child.node;

	if (!inode_ref(node)) {
		/* It left the tree while we were stepping onto it. The walk ends;
		 * POSIX leaves readdir() against a directory somebody is changing
		 * unspecified, and stopping is the answer that touches nothing. */
		return NULL;
	}

	strncpy(dir->current.d_name, inode_name(node), NAME_MAX - 1);
	dir->current.d_name[NAME_MAX - 1] = '\0';
	dir->current.d_ino = (ino_t) (uintptr_t) node;

	node->i_nlink ++;

	return &dir->current;
}
