/**
 *
 * @date 04.02.2013
 * @author Alexander Kalmuk
 * @author Anton Kozlov
 */

#include <types.h>
#include <errno.h>
#include <dirent.h>
#include <fs/vfs.h>
#include <mem/objalloc.h>
#include <util/dlist.h>
#include <string.h>

#define MAX_DIR_QUANTITY OPTION_GET(NUMBER, dir_quantity)

OBJALLOC_DEF(dir_pool, struct DIR, MAX_DIR_QUANTITY);

DIR *opendir(const char *path) {
	node_t *node;
	DIR *d;

	if (NULL == (node = vfs_lookup(NULL, path))) {
		SET_ERRNO(EBADF);
		return NULL;
	}

	if (!node_is_directory(node)) {
		SET_ERRNO(ENOTDIR);
		return NULL;
	}

	if (NULL == (d = objalloc(&dir_pool))) {
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	d->node = node;
	d->child_lnk = tree_children_begin(&node->tree_link);

	return d;
}

int closedir(DIR *dir) {

	if (NULL == dir) {
		SET_ERRNO(EBADF);
		return -1;
	}

	objfree(&dir_pool, dir);

	return 0;
}

struct dirent *readdir(DIR *dir) {
	struct node *chldnod;

	SET_ERRNO(0);

	if (NULL == dir) {
		SET_ERRNO(EBADF);
		return NULL;
	}

	if (tree_children_end(&dir->node->tree_link) == dir->child_lnk) {
		return NULL;
	}

	chldnod = tree_element(dir->child_lnk, struct node, tree_link);

	strcpy(dir->current.d_name, chldnod->name);

	dir->child_lnk = tree_children_next(dir->child_lnk);

	return &dir->current;
}
