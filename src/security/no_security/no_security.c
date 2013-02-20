/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.02.2013
 */

#include <security/security.h>

int security_node_permissions(struct node *node, int flags) {
	return 0;
}

int security_node_create(struct node *dir, mode_t mode) {
	return 0;
}

int security_node_delete(struct node *dir, struct node *node) {
	return 0;
}

int security_mount(struct node *dev, struct node *mountpoint) {
	return 0;
}

int security_xattr_get(struct node *node, const char *name, char *value, size_t len) {
	return 0;
}

int security_xattr_set(struct node *node, const char *name,
			const char *value, size_t len, int flags) {
	return 0;
}

int security_xattr_list(struct node *node, char *list, size_t len) {
	return 0;
}
