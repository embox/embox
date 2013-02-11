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
		if ('/' != *path) {
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

int vfs_add_leaf(node_t *child, node_t *parent) {
	tree_add_link(&(parent->tree_link), &(child->tree_link));
	return 0;
}

static node_t *vfs_add_new_path(node_t *parent,
		char *p_path, char *child_name) {
	node_t *child;
	if (NULL == (child = node_alloc(child_name))) {
		return NULL;
	}
	vfs_add_leaf(child, parent);
	while (NULL != (p_path = path_get_next_name(p_path, child_name,
											MAX_LENGTH_FILE_NAME))) {
		parent->type = NODE_TYPE_DIRECTORY;
		parent = child;
		if (NULL == (child = node_alloc(child_name))) {
			return NULL;
		}
		vfs_add_leaf(child, parent);
	}
	return child;
}

node_t *vfs_add_path(const char *path, node_t *parent) {
	node_t *node = parent;
	char node_name[MAX_LENGTH_FILE_NAME];
	char *p_path = (char *) path;

	if (NULL == parent) {
		node = vfs_get_root();
	}
	while (NULL != (p_path = path_get_next_name(p_path,	node_name,
													sizeof(node_name)))) {
		parent = node;
		if (NULL == (node = vfs_get_child(node_name, node))) {
			return vfs_add_new_path(parent, p_path, node_name);
		}
	}

	return NULL;
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

static int node_name_is(struct tree_link *link, void *name) {
	node_t *node = tree_element(link, node_t, tree_link);
	return strcmp(node->name, name) == 0;
}

node_t *vfs_get_child(const char *name, node_t *parent) {
	struct tree_link *tlink;

	tlink = tree_lookup_child(&(parent->tree_link), node_name_is, (void *) name);

	return tree_element(tlink, struct node, tree_link);
}

node_t *vfs_find_node(const char *path, node_t *parent) {
	node_t *node;
	char node_name[MAX_LENGTH_FILE_NAME];
	char *p_path = (char *) path;

	if (!parent) {
		parent = vfs_get_root();
	}

	node = parent;

	//FIXME if we return immediately we return root node
	while ((p_path = path_get_next_name(p_path, node_name, sizeof(node_name)))) {
		node = vfs_get_child(node_name, node);
		if (!node) {
			return NULL;
		}
	}

	return node;
}

node_t *vfs_get_root(void) {
	if (NULL == root_node) {
		root_node = node_alloc("/");
		assert(NULL != root_node);
		root_node->type = NODE_TYPE_DIRECTORY;
		//TODO set pseudofs driver
	}
	return root_node;
}

node_t *vfs_get_exist_path(const char *path, char *exist_path, size_t buff_len) {
	struct node *node;
	struct node *parent;
	char cname[MAX_LENGTH_FILE_NAME]; /* child name buffer*/
	char *p_path;

	if (path[0] != '/') { /* we can operate only with full path now */
		return NULL;
	}

	parent = node = vfs_get_root(); /* we always start search from the root node */

	exist_path[0] = '\0';
	p_path = (char *)path;

	do {
		parent = node;
		p_path = path_get_next_name(p_path, cname, sizeof(cname));

		if (0 == strlen(cname)) {
			return parent; /* we found full path */
		}

		if (NULL != (node = vfs_get_child(cname, node))) {
			/*add node to exist_path*/
			strncat(exist_path, "/", buff_len);
			strncat((char *)exist_path, cname, buff_len);

			if (!node_is_directory(node)) { /* only directory may has children */
				return node; /* this is the last element in the path */
			}
			continue;
		}
		//TODO try to caching node from a block dev

	} while (NULL != (p_path) && NULL != node);


	return parent;
}
