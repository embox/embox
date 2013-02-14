/**
 * @file
 * @brief Describes tree of VFS (Virtual Filesystem Switch).
 * @details An abstraction layer on top of a more concrete file systems.
 *
 * @date 12.10.10
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <string.h>

#include <fs/path.h>
#include <fs/vfs.h>

static node_t *root_node;

int vfs_get_path_by_node(node_t *nod, char *path) {
	node_t *parent, *node;
	char buff[MAX_LENGTH_PATH_NAME];

	*path = *buff= 0;
	node = nod;
	strncpy((char *) buff, (const char *) &node->name, MAX_LENGTH_FILE_NAME);

	while(NULL !=
			(parent = vfs_get_parent(node))) {
		strncpy((char *) path,
				(const char *) &parent->name, MAX_LENGTH_FILE_NAME);
		if('/' != *path) {
			strcat((char *) path, (const char *) "/");
		}
		strcat((char *) path, (const char *) buff);
		node = parent;
		strncpy((char *) buff, (const char *) path, MAX_LENGTH_PATH_NAME);
	}

	strncpy((char *) buff, (char *) path, MAX_LENGTH_PATH_NAME);
	buff[MAX_LENGTH_PATH_NAME - 1] = 0;
	if (strcmp((char *) path,(char *) buff)) {
		return -1;
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

		path += len;
		parent = child;
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

node_t *vfs_create(node_t *parent, const char *path, mode_t mode) {
	size_t len = 0;

	if (!parent) {
		parent = vfs_get_root();
	}

	parent = __vfs_lookup_existing(parent, path, &path);
	path = path_next(path, &len);

	if (!path || path_next(path + len, NULL)) {
		/* Node already exists, or missing intermediate node. */
		return NULL;
	}

	return __vfs_create_child(parent, path, len, mode);
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

node_t *vfs_get_parent(node_t *child) {
	struct tree_link *tlink;

	tlink = &child->tree_link;

	return tree_element(tlink->par, struct node, tree_link);
}

node_t *vfs_get_root(void) {
	if (!root_node) {
		root_node = node_alloc("/", 0);
		assert(root_node);
		root_node->mode = S_IFDIR;
		//TODO set pseudofs driver
	}
	return root_node;
}

node_t *vfs_get_exist_path(const char *path, char *exist_path, size_t buff_len) {
	struct node *node;
	struct node *parent;
	char cname[MAX_LENGTH_FILE_NAME]; /* child name buffer*/
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
