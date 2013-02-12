/**
 *
 * @date 04.02.2013
 * @author Alexander Kalmuk
 */

#include <types.h>
#include <errno.h>
#include <dirent.h>
#include <fs/vfs.h>
#include <mem/objalloc.h>
#include <util/dlist.h>
#include <string.h>

#define MAX_DIR_QUANTITY OPTION_GET(NUMBER, dir_quantity)

static DLIST_DEFINE(dir_list);

struct dir_info {
	DIR dir;                /**< corresponding directory */
	node_t *node;           /**< current node of directory */
	struct dlist_head link; /**< link to global list of dir_info structures */
};

OBJALLOC_DEF(__dir_pool, struct dir_info, MAX_DIR_QUANTITY);

DIR *opendir(const char *path) {
	node_t *node;
	struct dir_info *d;

	if (NULL == (node = vfs_lookup(NULL, path))) {
		SET_ERRNO(EBADF);
		return NULL;
	}

	if (!node_is_directory(node)) {
		SET_ERRNO(ENOTDIR);
		return NULL;
	}

	if (NULL == (d = objalloc(&__dir_pool))) {
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	dlist_head_init(&d->link);
	dlist_add_prev(&d->link, &dir_list);
	/* Initialization: set current node's name to empty string and
	 * current node to current directory */
	d->node = node;
	d->dir.current.d_name[0] = '\0';

	return &d->dir;
}

static struct dir_info *get_dirinfo(DIR *dir) {
	struct dir_info *d, *nxt;

	dlist_foreach_entry(d, nxt, &dir_list, link) {
		if (dir == &d->dir) {
			return d;
		}
	}

	return NULL;
}

int closedir(DIR *dir) {
	struct dir_info *d;

	if (NULL == (d = get_dirinfo(dir))) {
		SET_ERRNO(EBADF);
		return -1;
	}

	dlist_del(&d->link);
	objfree(&__dir_pool, d);

	return 0;
}

struct dirent *readdir(DIR *dir) {
	struct dir_info *d;
	struct node *node;
	struct tree_link *link;

	if (NULL == (d = get_dirinfo(dir))) {
		SET_ERRNO(EBADF);
		return NULL;
	}

	/* We watched all nodes already. Simply return NULL. */
	if (NULL == d->node) {
		return NULL;
	}

	/* That means we are in directory's node, not in child. So go to first child. */
	if (d->dir.current.d_name[0] == '\0') {
		link = tree_children_begin(&d->node->tree_link);
	} else {
		/* Otherwise get next child. */
		link = tree_children_next(&d->node->tree_link);
	}
	node = tree_element(link, struct node, tree_link);

	/* If current node is last child */
	if (&node->tree_link == tree_children_end(node->tree_link.par)) {
		d->node = NULL;
	} else {
		d->node = node;
	}

	strcpy(dir->current.d_name, node->name);

	return &dir->current;
}
