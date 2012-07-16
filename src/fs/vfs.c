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

/**
 * Save first node name in path into buff variable.
 * Return the remaining part of path.
 */
static char *get_next_node_name(const char *path, char *buff, int buff_len) {
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

int nip_tail(char *head, char *tail) {
	char *p_tail;
	char *p;

	p = p_tail = head + strlen(head);
	strcat(head, tail);

	do {
		p_tail--;
		if(head >= p_tail) {
			*p = '\0';
			return -1;
		}
	} while ('/' != *p_tail);

	strcpy (tail, p_tail);
	*p_tail = '\0';

	return 0;
}

int increase_tail(char *head, char *tail) {
	char *p_tail;

		p_tail = head + strlen(head);
		strcat(head, tail);

		do {
			if('\0' == *p_tail) {
				break;
			}
			p_tail++;
		} while ('/' != *p_tail);

		strcpy (tail, p_tail);
		*p_tail = '\0';

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
	while (NULL != (p_path = get_next_node_name(p_path, child_name,
											MAX_LENGTH_FILE_NAME))) {
		parent = child;
		child = alloc_node(child_name);
		vfs_add_leaf(child, parent);
	}
	return child;
}

extern node_t *vfs_find_child(const char *name, node_t *parrent);
extern node_t *rootfs_get_node (void);

node_t *vfs_add_path(const char *path, node_t *parent) {
	node_t *node = parent;
	char node_name[MAX_LENGTH_FILE_NAME];
	char *p_path = (char *) path;

	if (NULL == parent) {
		node = rootfs_get_node();
	}
	while (NULL != (p_path = get_next_node_name(p_path,	node_name,
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
		node = rootfs_get_node();
	}
	//FIXME if we return immediately we return root node
	while (NULL != (p_path = get_next_node_name(p_path, node_name,
													sizeof(node_name)))) {
		if (NULL == (node = vfs_find_child(node_name, node))) {
			return NULL;
		}
	}

	return node;
}
