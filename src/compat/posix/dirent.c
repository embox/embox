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

struct directory {
	DIR dir;
	node_t *node; /* node to increase speed when search directory */
	struct dlist_head link; /* link to global list of directories */
};

OBJALLOC_DEF(__dir_pool, struct directory, MAX_DIR_QUANTITY);

DIR *opendir(const char *path) {
	node_t *node;
	struct directory *dir;

	if (NULL == (node = vfs_find_node(path, NULL))) {
		SET_ERRNO(EBADF);
		return NULL;
	}

	if (!node_is_directory(node)) {
		SET_ERRNO(ENOTDIR);
		return NULL;
	}

	if (NULL == (dir = objalloc(&__dir_pool))) {
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	dlist_head_init(&dir->link);
	dlist_add_prev(&dir->link, &dir_list);
	dir->node = node;
	dir->dir.current.d_name[0] = '\0';

	return &dir->dir;
}

static struct directory *get_directory(DIR *dir) {
	struct directory *d, *nxt;

	dlist_foreach_entry(d, nxt, &dir_list, link) {
		if (dir == &d->dir) {
			return d;
		}
	}

	return NULL;
}

int closedir(DIR *dir) {
	struct directory *d;

	if (NULL == (d = get_directory(dir))) {
		SET_ERRNO(EBADF);
		return -1;
	}

	objfree(&__dir_pool, d);
	return 0;
}

struct dirent *readdir(DIR *dir) {
	struct directory *d;
	node_t *node;
	struct tree_link *link;

	if (NULL == (d = get_directory(dir))) {
		SET_ERRNO(EBADF);
		return NULL;
	}

	node = vfs_get_child(dir->current.d_name, d->node);
	/* we are in directory's node, not in child */
	if (NULL == node) {
		node = d->node;
	}

	link = tree_children_next(&node->tree_link);
	node = tree_element(link, struct node, tree_link);
	if (node == d->node) {
		return NULL;
	}
	strcpy(dir->current.d_name, node->name);

	return &dir->current;
}
