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
#include <fs/vfs.h>
#include <err.h>

static node_t *root_node;

/**
 * Save first node name in path into buff variable.
 * Return the remaining part of path.
 */
static char *vfs_get_next_node_name(const char *path, char *buff, int buff_len) {
	char *p = (char *) path;
	char *b = buff;
	while ('/' == *p) {
		p++;
	}
	while (('/' != *p) && ('\0' != *p) && (buff_len --> 1)) {
		*b++ = *p++;
	}
	*b = '\0';
	if (b != buff) {
		while ('/' == *p) {
			p++;
		}
		return p;
	}

	return NULL;
}

int vfs_get_path_by_node (node_t *nod, char *path) {

	node_t *parent, *node;
	char buff[MAX_LENGTH_PATH_NAME];

	*path = *buff= 0;
	node = nod;
	strncpy((char *) buff, (const char *) &node->name, MAX_LENGTH_FILE_NAME);

	while(NULL !=
			(parent = vfs_find_parent((const char *) &node->name, node))) {
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

int vfs_add_leaf(node_t *child, node_t *parent) {
	tree_add_link(&(parent->tree_link), &(child->tree_link));
	return 0;
}

static node_t *vfs_add_new_path(node_t *parent,
		char *p_path, char *child_name) {
	node_t *child;
	child = alloc_node(child_name);
	vfs_add_leaf(child, parent);
	while (NULL != (p_path = vfs_get_next_node_name(p_path, child_name,
											MAX_LENGTH_FILE_NAME))) {
		parent->properties = DIRECTORY_NODE_TYPE;
		parent = child;
		child = alloc_node(child_name);
		vfs_add_leaf(child, parent);
	}
	return child;
}

extern node_t *vfs_find_child(const char *name, node_t *parrent);
extern node_t *vfs_get_root (void);

node_t *vfs_add_path(const char *path, node_t *parent) {
	node_t *node = parent;
	char node_name[MAX_LENGTH_FILE_NAME];
	char *p_path = (char *) path;

	if (NULL == parent) {
		node = vfs_get_root();
	}
	while (NULL != (p_path = vfs_get_next_node_name(p_path,	node_name,
													sizeof(node_name)))) {
		parent = node;
		if (NULL == (node = vfs_find_child(node_name, node))) {
			return vfs_add_new_path(parent, p_path, node_name);
		}
	}

	return NULL;
}

int vfs_del_leaf(node_t *node) {
	return tree_unlink_link(&(node->tree_link));
}

static int compare_children_names(struct tree_link* link, void *name) {
	node_t *node = tree_element(link, node_t, tree_link);
	return 0 == strcmp(node->name, (char *)name);
}

node_t *vfs_find_parent(const char *name, node_t *child) {
	struct tree_link *tlink;

	tlink = &child->tree_link;

	return tree_element(tlink->par, struct node, tree_link);
}

node_t *vfs_find_child(const char *name, node_t *parent) {
	struct tree_link *tlink;

	tlink = tree_children_arg_find(&(parent->tree_link), (void *)name, compare_children_names);

	return tree_element(tlink, struct node, tree_link);
}

node_t *vfs_find_node(const char *path, node_t *parent) {
	node_t *node = parent;
	char node_name[MAX_LENGTH_FILE_NAME];
	char *p_path = (char *) path;

	if (NULL == parent) {
		node = vfs_get_root();
	}
	//FIXME if we return immediately we return root node
	while (NULL != (p_path = vfs_get_next_node_name(p_path, node_name,
													sizeof(node_name)))) {
		if (NULL == (node = vfs_find_child(node_name, node))) {
			return NULL;
		}
	}

	return node;
}

node_t *vfs_get_root(void) {
	if(NULL == root_node) {
		root_node = alloc_node("/");
		root_node->properties = DIRECTORY_NODE_TYPE;
	}
	return root_node;
}
