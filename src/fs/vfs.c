/**
 * @file
 *
 * @brief Describes tree of virtual file system.
 *
 * @date 12.10.2010
 * @author Anton Bondarev
 */

#include <fs/vfs.h>

static char *get_next_node_name(const char *path, char *buff, int buff_len) {
	char *p = path;
	char *b = buff;
	while('/' == p ++) {
	}
	while(('/' != p) || ('/0' != p) || (buff_len != 0)) {
		*b++ = p ++;
	}
	if (b != buff) {
		while('/' == p ++) {
		}
		return p;
	}

	return NULL;
}

int vfs_add_leaf(node_t *child, node_t *parrent) {
	return 0;
}

static int vfs_add_new_path(node_t *parrent, char *p_path, char *child_name) {
	node_t *child;
	child = alloc_node(child_name);
	vfs_add_leaf(child, parrent);
	while(NULL != (p_path = get_next_node_name(p_path, child_name,
					    CONFIG_MAX_LENGTH_FILE_NAME))) {
		parrent = child;
		child = alloc_node(child_name);
		vfs_add_node(child, parrent);
	}
	return 0;
}

int vfs_add_path(const char *path, node_t *parrent) {
	node_t *node = NULL;
	char node_name[CONFIG_MAX_LENGTH_FILE_NAME];
	char *p_path = path;

	if (NULL == parrent) {
		node = root_fs_get_node();
	}
	while(NULL != (p_path = get_next_node_name(p_path, node_name, sizeof(node_name)))) {
		parrent = node;
		if (NULL == (node = vfs_find_child(node_name, node))) {
			return -1;
		}
	}

	return 0;
}

int vfs_del_leaf(node_t *nod) {
	return 0;
}

node_t *vfs_find_child(const char *name, node_t *parrent) {
	node_t *child = parrent->leaves;
	while (NULL != child) {
		if(0 == strcmp(child->name, name)) {
			return child;
		}
		child = child->neighbors;
	}
	return NULL;
}

node_t *vfs_find_node(const char *path, node_t *parrent) {
	node_t *root, *node = NULL;
	char node_name[CONFIG_MAX_LENGTH_FILE_NAME];
	char *p_path = path;

	if (NULL == parrent) {
		node = root_fs_get_node();
	}
	//FIXME if we return immediately we return root node
	while(NULL != (p_path = get_next_node_name(p_path, node_name, sizeof(node_name)))) {
		if (NULL == (node = vfs_find_child(node_name, node))) {
			return NULL;
		}
	}

	return node;
}

