/**
 * @file
 *
 * @brief Describes tree of virtual file system.
 *
 * @date 12.10.2010
 * @author Anton Bondarev
 */

#include <fs/node.h>
#include <fs/vfs.h>
#include <string.h>

static char *get_next_node_name(const char *path, char *buff, int buff_len) {
	char *p = (char *)path;
	char *b = buff;
	while('/' == *p) {
		p ++;
	}
	while(('/' != *p) && ('\0' != *p) && (buff_len != 0)) {
		*b ++ = *p ++;
	}
	*b = '\0';
	if (b != buff) {
		while('/' == *p ) {
			p ++;
		}
		return p;
	}

	return NULL;
}

int vfs_add_leaf(node_t *child, node_t *parrent) {
	list_add(&(child->neighbors), &(parrent->leaves));
	return 0;
}

static int vfs_add_new_path(node_t *parrent, char *p_path, char *child_name) {
	node_t *child;
	child = alloc_node(child_name);
	vfs_add_leaf(child, parrent);
	while(NULL != (p_path = get_next_node_name(p_path, child_name, CONFIG_MAX_LENGTH_FILE_NAME))) {
		parrent = child;
		child = alloc_node(child_name);
		vfs_add_leaf(child, parrent);
	}
	return 0;
}

extern node_t *vfs_find_child(const char *name, node_t *parrent);
extern node_t *rootfs_get_node (void);

int vfs_add_path(const char *path, node_t *parrent) {
	node_t *node = NULL;
	char node_name[CONFIG_MAX_LENGTH_FILE_NAME];
	char *p_path = (char *)path;

	if (NULL == parrent) {
		node = rootfs_get_node();
	}
	while(NULL != (p_path = get_next_node_name(p_path, node_name, sizeof(node_name)))) {
		parrent = node;
		if (NULL == (node = vfs_find_child(node_name, node))) {
			return vfs_add_new_path(parrent, p_path, node_name);
		}
	}

	return 0;
}

int vfs_del_leaf(node_t *nod) {

	return 0;
}

node_t *vfs_find_child(const char *name, node_t *parrent) {
	node_t *child;
	struct list_head *head;
	list_for_each(head, &(parrent->leaves)) {
		child = list_entry(head, node_t, neighbors);
		if(0 == strcmp(child->name, name)) {
			return child;
		}
	}
	return NULL;
}

node_t *vfs_find_node(const char *path, node_t *parrent) {
	node_t *node = NULL;
	char node_name[CONFIG_MAX_LENGTH_FILE_NAME];
	char *p_path = (char *)path;

	if (NULL == parrent) {
		node = rootfs_get_node();
	}
	//FIXME if we return immediately we return root node
	while(NULL != (p_path = get_next_node_name(p_path, node_name, sizeof(node_name)))) {
		if (NULL == (node = vfs_find_child(node_name, node))) {
			return NULL;
		}
	}

	return node;
}

