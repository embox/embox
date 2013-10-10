/**
 * @file
 * @brief Describes tree of VFS (Virtual Filesystem Switch).
 * @details An abstraction layer on top of a more concrete file systems.
 *
 * @date 12.10.10
 * @author Anton Bondarev
 * @author Eldar Abusalimov
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fs/path.h>
#include <fs/vfs.h>
#include <limits.h>
#include <fcntl.h>

#define ROOT_MODE 0755

int vfs_get_pathbynode_tilln(node_t *node, node_t *parent, char *path,
		size_t plen) {
	char *p;
	size_t ll = plen - 1;

	if (plen <= 0) {
		return -ERANGE;
	}

	p = path + ll;
	*p = '\0';

	while (node != parent && node != NULL) {
		size_t nnlen = strlen(node->name);

		if (nnlen + 1 > ll) {
			return -ERANGE;
		}

		p = strncpy(p - nnlen, node->name, nnlen);
		*--p = '/';
		ll -= nnlen + 1;

		node = vfs_get_parent(node);
	}

	memmove(path, p, plen - ll);

	if (node != parent) {
		return 1;
	}

	return 0;
}

struct lookup_tuple {
	const char *name;
	size_t len;
};

static int vfs_lookup_cmp(struct tree_link *link, void *data) {
	struct lookup_tuple *lookup = data;
	node_t *node = tree_element(link, node_t, tree_link);
	const char *name = node->name;
	return !(strncmp(name, lookup->name, lookup->len) || name[lookup->len]);
}

node_t *vfs_lookup_childn(node_t *parent, const char *name, size_t len) {
	struct lookup_tuple lookup = { .name = name, .len = len };
	struct tree_link *tlink;

	assert(parent);

	tlink = tree_lookup_child(&(parent->tree_link), vfs_lookup_cmp, &lookup);

	return tree_element(tlink, struct node, tree_link);
}

static node_t *__vfs_lookup_existing(node_t *parent, const char *path,
		const char **p_end_existent) {
	node_t *child;
	size_t len = 0;

	assert(parent && path);

	while ((path = path_next(path, &len))) {
		child = vfs_lookup_childn(parent, path, len);
		if (!child) {
			break;
		}

		parent = child;
		path += len;
	}

	if (p_end_existent) {
		*p_end_existent = path;
	}

	return parent;
}

node_t *vfs_lookup_child(node_t *parent, const char *name) {
	if (!parent) {
		parent = vfs_get_root();
	}

	return vfs_lookup_childn(parent, name, strlen(name));
}

node_t *vfs_lookup(node_t *parent, const char *path) {
	if (!parent) {
		parent = vfs_get_root();
	}

	parent = __vfs_lookup_existing(parent, path, &path);

	if (path_next(path, NULL)) {
		/* Have unresolved fragments in path. */
		return NULL;
	}

	return parent;
}

int vfs_add_leaf(node_t *child, node_t *parent) {
	tree_add_link(&(parent->tree_link), &(child->tree_link));
	return 0;
}

static node_t *__vfs_create_child(node_t *parent, const char *name, size_t len,
		mode_t mode) {
	node_t *child;

	assert(parent);
	assert(mode & S_IFMT, "Must provide a type of node, see S_IFXXX");

	child = node_alloc(name, len);
	if (child) {
		child->mode = mode;
		vfs_add_leaf(child, parent);
	}
	return child;
}

node_t *vfs_create_child(node_t *parent, const char *name, mode_t mode) {
	if (!parent) {
		parent = vfs_get_root();
	}

	return __vfs_create_child(parent, name, strlen(name), mode);
}

static node_t *__vfs_create(node_t *parent, const char *path, mode_t mode,
		int intermediate) {
	size_t len;

	if (!parent) {
		parent = vfs_get_root();
	}

	parent = __vfs_lookup_existing(parent, path, &path);
	path = path_next(path, &len);

	/* Here path points to the first non-existent fragment, if any. */

	if (intermediate && !path) {
		/* Node already exist, set mode. */
		parent->mode = mode;
		return parent;

	} else if (intermediate) {
		const char *next_path;
		size_t next_len;

		while ((next_path = path_next(path + len, &next_len))) {
			parent = __vfs_create_child(parent, path, len, S_IFDIR);
			if (!parent) {
				return NULL;
			}

			path = next_path;
			len = next_len;
		}

	} else if (!path || path_next(path + len, NULL)) {
		/* Node already exists or missing intermediate node. */
		return NULL;
	}

	return __vfs_create_child(parent, path, len, mode);
}

node_t *vfs_create(node_t *parent, const char *path, mode_t mode) {
	return __vfs_create(parent, path, mode, 0);
}

node_t *vfs_create_intermediate(node_t *parent, const char *path, mode_t mode) {
	return __vfs_create(parent, path, mode, 1);
}

int vfs_del_leaf(node_t *node) {
	int rc;

	assert(node);

	rc = tree_unlink_link(&(node->tree_link));
	if (rc) {
		node_free(node);
	}
	return rc;
}

node_t *vfs_get_child_next(node_t *parent) {
	struct tree_link *tlink;

	tlink = tree_children_begin(&(parent->tree_link));

	return tree_element(tlink, struct node, tree_link);
}

node_t *vfs_get_parent(node_t *child) {
	struct tree_link *tlink;

	tlink = &child->tree_link;

	return tree_element(tlink->par, struct node, tree_link);
}

node_t *vfs_get_leaf(void) {
	char *leaf_name;
	node_t *leaf;

	if ((NULL == (leaf_name = getenv("PWD")))
			|| (NULL == (leaf = vfs_lookup(NULL, leaf_name)))) {
		leaf = vfs_get_root();
	}

	return leaf;
}

node_t *vfs_get_root(void) {
	static node_t *root_node;

	if (!root_node) {
		root_node = node_alloc("/", 0);
		assert(root_node);
		root_node->mode = S_IFDIR | ROOT_MODE;
		//TODO set pseudofs driver
	}

	return root_node;
}

node_t *vfs_get_exist_path(const char *path, char *exist_path, size_t buff_len) {
	struct node *node;
	struct node *parent;
	char cname[NAME_MAX]; /* child name buffer*/
	char *p_path;

	if(path[0] != '/') { /* we can operate only with full path now */
		return NULL;
	}

	parent = node = vfs_get_root(); /* we always start search from the root node */

	exist_path[0] = '\0';
	p_path = (char *)path;

	do {
		parent = node;
		p_path = path_get_next_name(p_path, cname, sizeof(cname));

		if(0 == strlen(cname)) {
			return parent; /* we found full path */
		}

		if(NULL != (node = vfs_lookup_child(node, cname))) {
			/*add node to exist_path*/
			strncat(exist_path, "/", buff_len);
			strncat((char *)exist_path, cname, buff_len);

			if(!node_is_directory(node)) { /* only directory may has children */
				return node; /* this is the last element in the path */
			}
			continue;
		}
		//TODO try to caching node from a block dev

	} while (NULL != (p_path) && NULL != node);


	return parent;
}

/*
 * Below fcntl locking implementation and help functions
 */
#include <kernel/spinlock.h>
#include <kernel/thread.h>
#include <mem/misc/pool.h>
#include <fs/file_desc.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>

//#define MAX_KFLOCK_SHARED_QUANTITY OPTION_GET(NUMBER, flock_quantity)
POOL_DEF(kflock_shared_pool, kflock_shared_t, 128);
//#define MAX_KFLOCK_EXCLUSIVE_QUANTITY OPTION_GET(NUMBER, flock_quantity)
POOL_DEF(kflock_exclusive_pool, kflock_exclusive_t, 128);

enum {
	FLOCK_SHARED_INCLUDE,
	FLOCK_SHARED_INCLUDED,
	FLOCK_SHARED_FIT,
	FLOCK_SHARED_INTERSECT_LEFT,
	FLOCK_SHARED_INTERSECT_RIGHT,
	FLOCK_SHARED_BORDER_LEFT,
	FLOCK_SHARED_BORDER_RIGHT,
	FLOCK_EXCLUSIVE_INCLUDE,
	FLOCK_EXCLUSIVE_INCLUDED,
	FLOCK_EXCLUSIVE_FIT,
	FLOCK_EXCLUSIVE_INTERSECT_LEFT,
	FLOCK_EXCLUSIVE_INTERSECT_RIGHT,
	FLOCK_EXCLUSIVE_BORDER_LEFT,
	FLOCK_EXCLUSIVE_BORDER_RIGHT,
	FLOCK_RANGE_NOT_INTERSECT
};

static int kflock_shared_get(kflock_t *kflock, struct flock *flock) {
	kflock_shared_t *shlock;
	struct thread *current = thread_self();

	shlock = pool_alloc(&kflock_shared_pool);
	if (NULL == shlock) {
		return -ENOMEM;
	}
	dlist_add_next(dlist_head_init(&shlock->kflock_link), &kflock->shlock_list);
	shlock->holder = current;
	shlock->pid = (pid_t) current->task; /* FIXME: Task pid should be here */
	shlock->whence = flock->l_whence; /* FIXME: Do not forget to take it into account */
	shlock->start = flock->l_start;
	shlock->len = flock->l_len;
	kflock->shlock_count++;

	return -ENOERR;
}

/* Put certain shared flock if provided or
 * all shared locks of current thread if NULL */
static int kflock_shared_put(kflock_t *kflock, struct flock *flock) {
	kflock_shared_t *shlock;
	struct thread *current = thread_self();
	struct dlist_head *item, *next;

	dlist_foreach(item, next, &kflock->shlock_list) {
		shlock = dlist_entry(item, kflock_shared_t, kflock_link);
		if (current == shlock->holder) {
			if (NULL == flock || (shlock->start == flock->l_start &&
					shlock->len == flock->l_len)) {
				dlist_del(&shlock->kflock_link);
				pool_free(&kflock_shared_pool, shlock);
				kflock->shlock_count--;
			}
		}
	}

	return -ENOERR;
}

static int kflock_exclusive_get(kflock_t *kflock, struct flock *flock) {
	kflock_exclusive_t *exlock;
	struct thread *current = thread_self();

	exlock = pool_alloc(&kflock_exclusive_pool);
	if (NULL == exlock) {
		return -ENOMEM;
	}

	mutex_init(&exlock->lock);
	mutex_lock(&exlock->lock);

	dlist_add_next(dlist_head_init(&exlock->kflock_link), &kflock->exlock_list);

	exlock->pid = (pid_t) current->task; /* FIXME: Task pid should be here */
	exlock->whence = flock->l_whence;
	exlock->start = flock->l_start;
	exlock->len = flock->l_len;
	kflock->exlock_count++;

	return -ENOERR;
}

/* Put certain exclusive flock if provided or
 * all exclusive locks of current thread if NULL */
static int kflock_exclusive_put(kflock_t *kflock, struct flock *flock) {
	kflock_exclusive_t *exlock;
	struct thread *current = thread_self();
	struct dlist_head *item, *next;

	dlist_foreach(item, next, &kflock->exlock_list) {
		exlock = dlist_entry(item, kflock_exclusive_t, kflock_link);
		if (current == exlock->lock.holder) {
			if (NULL == flock || (exlock->start == flock->l_start &&
					exlock->len == flock->l_len)) {
				mutex_unlock(&exlock->lock);
				dlist_del(&exlock->kflock_link);
				pool_free(&kflock_exclusive_pool, exlock);
				kflock->exlock_count--;
			}
		}
	}

	return -ENOERR;
}

/*
 * Return disposition of flock compare to locks in kflock
 */
static int kflock_get_disposition(kflock_t *kflock, struct flock *flock)
{
	kflock_exclusive_t *exlock;
	kflock_shared_t *shlock;
	struct dlist_head *item, *next;
	off_t lock_end, flock_end = flock->l_start + flock->l_len;
	off_t delta_start, delta_end;

	dlist_foreach(item, next, &kflock->exlock_list) {
		exlock = dlist_entry(item, kflock_exclusive_t, kflock_link);
		lock_end = exlock->start + exlock->len;
		delta_start = exlock->start - flock->l_start;
		delta_end = lock_end - flock_end;

		if (0 == delta_start && 0 == delta_end)
			return FLOCK_EXCLUSIVE_FIT;
		if (delta_start <= 0 && delta_end >= 0)
			return FLOCK_EXCLUSIVE_INCLUDE;
		if (delta_start >= 0 && delta_end <= 0)
			return FLOCK_EXCLUSIVE_INCLUDED;
		if (delta_start == exlock->len)
			return FLOCK_EXCLUSIVE_BORDER_RIGHT;
		if (delta_end == -exlock->len)
			return FLOCK_EXCLUSIVE_BORDER_LEFT;
		if (delta_start < 0 && delta_end <= 0)
			return FLOCK_EXCLUSIVE_INTERSECT_LEFT;
		if (delta_start >= 0 && delta_end > 0)
			return FLOCK_EXCLUSIVE_INTERSECT_RIGHT;
	}

	dlist_foreach(item, next, &kflock->shlock_list) {
		shlock = dlist_entry(item, kflock_shared_t, kflock_link);
		lock_end = shlock->start + shlock->len;
		delta_start = shlock->start - flock->l_start;
		delta_end = lock_end - flock_end;

		if (0 == delta_start && 0 == delta_end)
			return FLOCK_SHARED_FIT;
		if (delta_start <= 0 && delta_end >= 0)
			return FLOCK_SHARED_INCLUDE;
		if (delta_start >= 0 && delta_end <= 0)
			return FLOCK_SHARED_INCLUDED;
		if (delta_start == shlock->len)
			return FLOCK_SHARED_BORDER_RIGHT;
		if (delta_end == -shlock->len)
			return FLOCK_SHARED_BORDER_LEFT;
		if (delta_start < 0 && delta_end <= 0)
			return FLOCK_SHARED_INTERSECT_LEFT;
		if (delta_start >= 0 && delta_end > 0)
			return FLOCK_SHARED_INTERSECT_RIGHT;
	}
	return FLOCK_RANGE_NOT_INTERSECT;
}

static inline int kflock_validate_and_fix(struct flock *flock, int fd) {
	int file_size, cur_offset;
	struct idx_desc *idesc;
	struct stat finfo;
	short fltype = flock->l_type;

	if (-1 == fstat(fd, &finfo))
		return errno;
	file_size = finfo.st_blksize * finfo.st_blocks;

	/* Make l_start to point to flock from the start of the file */
	switch (flock->l_whence) {
	case SEEK_CUR:
		idesc = task_self_idx_get(fd);
		cur_offset = ((struct file_desc *) idesc->data->fd_struct)->cursor;
		flock->l_start += cur_offset;
		break;
	case SEEK_END:
		flock->l_start = file_size - flock->l_start;
		break;
	}

	/* Validate l_start */
	if (flock->l_start < 0 || flock->l_start > file_size)
		return -EINVAL;

	/* Convert length to positive if negative or zero value was provided
	 * or cut it until the end of the file if it is too big */
	if (0 == flock->l_len || ((flock->l_len + flock->l_start) > file_size))
		flock->l_len = file_size - flock->l_start;
	if (flock->l_len < 0) {
		flock->l_start -= flock->l_len;
		flock->l_len = -flock->l_len;
	}

	/* Validate type of the lock */
	if (! (fltype && !(fltype & (fltype - 1)) && \
			(((F_RDLCK | F_WRLCK | F_UNLCK) & fltype) == fltype)))
		return -EINVAL;

	return -ENOERR;
}

static inline int kflock_validate_cmd(int cmd) {
	if (cmd && !(cmd & (cmd - 1)) && \
			(((F_SETLK | F_SETLKW | F_GETLK) & cmd) == cmd))
		return -ENOERR;
	return -EINVAL;
}

static int kflock_handle_exclusive(kflock_t *kflock, struct flock *flock) {
	int rc;
	rc = kflock_get_disposition(kflock, flock);
	switch(rc) {
	case FLOCK_SHARED_INCLUDE:
		//kflock_conv_shared_exclusive();
	case FLOCK_SHARED_INCLUDED:
	case FLOCK_SHARED_FIT:
	case FLOCK_SHARED_INTERSECT_LEFT:
	case FLOCK_SHARED_INTERSECT_RIGHT:
	case FLOCK_SHARED_BORDER_LEFT:
	case FLOCK_SHARED_BORDER_RIGHT:
	case FLOCK_EXCLUSIVE_INCLUDE:
	case FLOCK_EXCLUSIVE_INCLUDED:
	case FLOCK_EXCLUSIVE_FIT:
	case FLOCK_EXCLUSIVE_INTERSECT_LEFT:
	case FLOCK_EXCLUSIVE_INTERSECT_RIGHT:
	case FLOCK_EXCLUSIVE_BORDER_LEFT:
	case FLOCK_EXCLUSIVE_BORDER_RIGHT:
	case FLOCK_RANGE_NOT_INTERSECT:
		break;
	}


	return rc;
}

static int kflock_handle_shared(kflock_t *kflock, struct flock *flock) {
	/* 4 Lines below to make project able to build, should be removed */
	kflock_exclusive_get(kflock, flock);
	kflock_exclusive_put(kflock, flock);
	kflock_shared_get(kflock, flock);
	kflock_shared_put(kflock, flock);
	return -ENOERR;
}

static int kflock_handle_unlock(kflock_t *kflock, struct flock *flock) {
	return -ENOERR;
}

/**
 * Common fcntl locking implemetaion
 * Can be used by any fs if it doesn't have own implementation
 * @param File descriptor number
 * @param Operation: F_GETLK, F_SETLK and F_SETLKW
 * @param Sturcture stuct flock with lock description
 * @return ENOERR if operation succeed or -1 and set errno in other way
 */
int vfs_fcntl_lock(int fd, int cmd, struct flock *flock) {
	int rc;
	kflock_t *kflock;
	/*long *exlock_count;
	long *shlock_count;
	spinlock_t *kflock_guard;*/
	struct idx_desc *idesc;

	/* Validate lock */
	if ((rc = kflock_validate_and_fix(flock, fd)) != -ENOERR)
		return rc;

	/* - Find locks and other properties for provided file descriptor number
	 * - fd is validated inside task_self_idx_get */
	idesc = task_self_idx_get(fd);
	kflock = &((struct file_desc *) idesc->data->fd_struct)->node->kflock;
	/*exlock_count = &kflock->exlock_count;
	shlock_count = &kflock->shlock_count;
	kflock_guard = &kflock->flock_guard;*/

	switch(flock->l_type) {
	case F_WRLCK:
		return kflock_handle_exclusive(kflock, flock);
	case F_RDLCK:
		return kflock_handle_shared(kflock, flock);
	case F_UNLCK:
		return kflock_handle_unlock(kflock, flock);
	}

	return -ENOERR;
}

int vfs_fcntl(int fd, int cmd, ... /* arg */ ) {
	int rc;
	va_list args;
	struct flock *flock;

	if ((rc = kflock_validate_cmd(cmd)) == -ENOERR) {
		/* One of the fcntl locking operations requested */
		va_start(args, cmd);
		flock = va_arg(args, struct flock *);
		va_end(args);

		return vfs_fcntl_lock(fd, cmd, flock);
	}

	return rc;
}
