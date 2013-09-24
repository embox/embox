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
