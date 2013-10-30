/**
 * @file
 * @brief
 *
 * @date 04.02.13
 * @author Alexander Kalmuk
 * @author Anton Kozlov
 */

#include <errno.h>
#include <string.h>
#include <fs/perm.h>
#include <fs/vfs.h>
#include <mem/objalloc.h>
#include <util/dlist.h>
#include <fs/flags.h>
#include <framework/mod/options.h>

#include <dirent.h>

#define MAX_DIR_QUANTITY OPTION_GET(NUMBER, dir_quantity)

OBJALLOC_DEF(dir_pool, struct directory, MAX_DIR_QUANTITY);

DIR *opendir(const char *path) {
	node_t *node;
	DIR *d;
	int res;

	if (0 != (res = fs_perm_lookup(vfs_get_leaf(), path, NULL, &node))) {
		SET_ERRNO(-res);
		return NULL;
	}

	if (!node_is_directory(node)) {
		SET_ERRNO(ENOTDIR);
		return NULL;
	}

	if (0 != fs_perm_check(node, FS_MAY_READ)) {
		SET_ERRNO(EACCES);
		return NULL;
	}

	if (NULL == (d = objalloc(&dir_pool))) {
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	d->node = node;
	d->current.d_ino = 0;// = tree_children_begin(&node->tree_link);

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
	//struct node *dirnod;
	struct tree_link *chld_link;

	SET_ERRNO(0);

	if (NULL == dir) {
		SET_ERRNO(EBADF);
		return NULL;
	}

	if (0 == dir->current.d_ino) {
		chld_link = tree_children_begin(&dir->node->tree_link);
		if (chld_link == NULL) {
			return NULL;
		}
	} else {
		chldnod = (struct node *)dir->current.d_ino;
		if (tree_children_end(&chldnod->tree_link) == &chldnod->tree_link) {
			return NULL;
		}

		chld_link = tree_children_next(&chldnod->tree_link);
	}

	chldnod = tree_element(chld_link, struct node, tree_link);

	strcpy(dir->current.d_name, chldnod->name);

	dir->current.d_ino = (ino_t)chldnod;

	return &dir->current;
}
