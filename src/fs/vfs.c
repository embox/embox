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

#include <fs/hlpr_path.h>
#include <fs/vfs.h>

#include <limits.h>

#define ROOT_MODE 0755

int vfs_get_pathbynode_tilln(struct path *node, struct path *parent, char *path,
		size_t plen) {
	char *p;
	size_t ll = plen - 1;

	if (plen <= 0) {
		return -ERANGE;
	}

	p = path + ll;
	*p = '\0';

	while (node->node != parent->node && node->node != NULL) {
		size_t nnlen = strlen(node->node->name);

		if (nnlen + 1 > ll) {
			return -ERANGE;
		}

		p = strncpy(p - nnlen, node->node->name, nnlen);
		*--p = '/';
		ll -= nnlen + 1;

		vfs_get_parent(node, node);
	}

	memmove(path, p, plen - ll);

	if (node->node != parent->node) {
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

void vfs_lookup_childn(struct path *parent, const char *name, size_t len,
		struct path *child) {
	struct lookup_tuple lookup = { .name = name, .len = len };
	struct tree_link *tlink;

	assert(parent);
#if 0
	if (len && !(S_IFDIR(parent))) {
		return NULL;
	}
#endif

	if_mounted_follow_down(parent);
	*child = *parent;

	if (len == 1 && *name == '.') {
		return;
	}

	tlink = tree_lookup_child(&(parent->node->tree_link), vfs_lookup_cmp, &lookup);

	child->node = tree_element(tlink, struct node, tree_link);
}

static void __vfs_lookup_existing(struct path *parent, const char *str_path,
		const char **p_end_existent, struct path *path) {
	struct path child;
	size_t len = 0;

	assert(parent && str_path);
	*path = *parent;

	while ((str_path = path_next(str_path, &len))) {
		if_mounted_follow_down(path);
		vfs_lookup_childn(path, str_path, len, &child);
		if (!child.node) {
			break;
		}

		*path = child;
		str_path += len;
	}

	if (p_end_existent) {
		*p_end_existent = str_path;
	}

	return;
}

void vfs_lookup_child(struct path *parent, const char *name, struct path *child) {
	vfs_lookup_childn(parent, name, strlen(name), child);
}

int vfs_lookup(struct path *parent, const char *str_path, struct path *path) {
	if (!parent) {
		vfs_get_root_path(parent);
	}

	if_mounted_follow_down(parent);

	__vfs_lookup_existing(parent, str_path, &str_path, path);

	if (path_next(str_path, NULL)) {
		/* Have unresolved fragments in path. */
		return -1;
	}

	return 0;
}

int vfs_add_leaf(node_t *child, node_t *parent) {
	tree_add_link(&(parent->tree_link), &(child->tree_link));
	return 0;
}

void __vfs_create_child(struct path *parent, const char *name, size_t len,
		mode_t mode, struct path *child) {

	assert(parent);
	assert(mode & S_IFMT, "Must provide a type of node, see S_IFXXX");

	if_mounted_follow_down(parent);

	child->node = node_alloc(name, len);
	if (child->node) {
		child->node->mode = mode;
		vfs_add_leaf(child->node, parent->node);
	}
}

void vfs_create_child(struct path *parent, const char *name, mode_t mode,
		struct path *child) {
	__vfs_create_child(parent, name, strlen(name), mode, child);
}

int __vfs_create(struct path *parent, const char *path, mode_t mode,
		int intermediate, struct path *child) {
	size_t len;
	struct path tmp_parent;

	assert(parent);

	tmp_parent = *parent;

	__vfs_lookup_existing(&tmp_parent, path, &path, &tmp_parent);
	path = path_next(path, &len);

	/* Here path points to the first non-existent fragment, if any. */

	if (intermediate && !path) {
		/* Node already exist, set mode. */
		parent->node->mode = mode;
		*child = tmp_parent;
		return 0;

	} else if (intermediate) {
		const char *next_path;
		size_t next_len;

		while ((next_path = path_next(path + len, &next_len))) {
			__vfs_create_child(&tmp_parent, path, len, S_IFDIR, child);

			if (!child) {
				return -1;
			}

			tmp_parent = *child;
			path = next_path;
			len = next_len;
		}

	} else if (!path || path_next(path + len, NULL)) {
		/* Node already exists or missing intermediate node. */
		return -1;
	}

	__vfs_create_child(&tmp_parent, path, len, mode, child);
	return child->node ? 0: -1;
}

//TODO
int vfs_create(struct path *parent, const char *path, mode_t mode,
		struct path *child) {
	return __vfs_create(parent, path, mode, 0, child);
}

int vfs_create_intermediate(struct path *parent, const char *path, mode_t mode,
		struct path *child) {
	return __vfs_create(parent, path, mode, 1, child);
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

void vfs_get_child_next(struct path *parent_path, struct path *child_next) {
	struct tree_link *tlink;

	 *child_next = *parent_path;
	if_mounted_follow_down(child_next);

	tlink = tree_children_begin(&(child_next->node->tree_link));

	child_next->node = tree_element(tlink, struct node, tree_link);
}

void vfs_get_parent(struct path *child_path, struct path *parent_path) {
	struct tree_link *tlink;

	*parent_path = *child_path;
	if_root_follow_up(parent_path);

	tlink = &parent_path->node->tree_link;

	parent_path->node = tree_element(tlink->par, struct node, tree_link);
}

node_t *vfs_get_leaf(void) {
	struct path leaf;

	vfs_get_leaf_path(&leaf);

	return leaf.node;
}

node_t *vfs_create_root(void) {
	node_t *root_node;

	root_node = node_alloc("/", 0);
	assert(root_node);
	root_node->mode = S_IFDIR | ROOT_MODE;

	return root_node;
}

node_t *vfs_get_root(void) {
	static node_t *root_node;

	if (!root_node) {
		root_node = vfs_create_root();
		//TODO set pseudofs driver
	}

	return root_node;
}

void vfs_get_root_path(struct path *path) {
	path->node = vfs_get_root();
	path->mnt_desc = mount_table();
}

void vfs_get_leaf_path(struct path *path) {
	char *leaf_name;
	struct path root;

	vfs_get_root_path(&root);
	if ((NULL == (leaf_name = getenv("PWD")))
			|| (0 != vfs_lookup(&root, leaf_name, path))) {
		vfs_get_root_path(path);
	}
}

////todo is it necessary?
//void vfs_get_exist_path(const char *path, char *exist_path, size_t buff_len, struct path *path_node) {
//	struct node *node;
//	char cname[NAME_MAX]; /* child name buffer*/
//	char *p_path;
//
//	if(path[0] != '/') { /* we can operate only with full path now */
//		path_node = NULL;
//		return;
//	}
//
//	path_node->node = node = vfs_get_root(); /* we always start search from the root node */
//
//	exist_path[0] = '\0';
//	p_path = (char *)path;
//
//	do {
//		path_node->node = node;
//		p_path = path_get_next_name(p_path, cname, sizeof(cname));
//
//		if(0 == strlen(cname)) {
//			return; /* we found full path */
//		}
//
//		if(NULL != (node = vfs_lookup_child(node, cname))) {
//			/*add node to exist_path*/
//			strncat(exist_path, "/", buff_len);
//			strncat((char *)exist_path, cname, buff_len);
//
//			if(!node_is_directory(node)) { /* only directory may has children */
//				return; /* this is the last element in the path */
//			}
//			continue;
//		}
//		//TODO try to caching node from a block dev
//
//	} while (NULL != (p_path) && NULL != node);
//
//
//	return;
//}

void if_mounted_follow_down(struct path *path) {
	if (path->node->mounted) {
		path->mnt_desc = mount_table_get_child(path->mnt_desc, path->node);

		assert(path->mnt_desc);

		path->node = path->mnt_desc->mnt_root;
	}
}

void if_root_follow_up(struct path *path) {
	if (path->node == path->mnt_desc->mnt_root) {
		path->node = path->mnt_desc->mnt_point;
		path->mnt_desc = path->mnt_desc->mnt_parent;
	}
}
