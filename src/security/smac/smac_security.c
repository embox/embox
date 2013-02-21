/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.02.2013
 */

#include <string.h>
#include <fs/node.h>
#include <kernel/task.h>
#include <fs/xattr.h>
#include <security/smac.h>

#include <security/security.h>

int security_node_create(struct node *dir, mode_t mode) {
	char label[SMAC_LABELLEN];
	int res;

	if (0 > (res = kfile_xattr_get(dir, smac_xattrkey, label,
					SMAC_LABELLEN))) {
		return 0;
	}

	return smac_access(task_self_security(), label, FS_MAY_WRITE);
}

int security_node_permissions(struct node *node, int flags) {
	char label[SMAC_LABELLEN];
	int res;

	if (0 > (res = kfile_xattr_get(node, smac_xattrkey, label,
					SMAC_LABELLEN))) {
		return 0;
	}

	return smac_access(task_self_security(), label, flags);
}

int security_node_delete(struct node *dir, struct node *node) {
	return 0;
}

int security_mount(struct node *dev, struct node *mountpoint) {
	return 0;
}

int security_umount(struct node *mountpoint) {
	return 0;
}

int security_xattr_get(struct node *node, const char *name, char *value,
		size_t len) {
	char label[SMAC_LABELLEN];
	int res;

	if (0 == strcmp(name, smac_xattrkey)) {
		return smac_access(task_self_security(), smac_admin,
				FS_MAY_READ);
	}

	if (0 > (res = kfile_xattr_get(node, smac_xattrkey, label,
					SMAC_LABELLEN))) {
		return 0;
	}

	return smac_access(task_self_security(), label, FS_MAY_READ);
}

int security_xattr_set(struct node *node, const char *name,
			const char *value, size_t len, int flags) {
	char label[SMAC_LABELLEN];
	int res;

	if (0 == strcmp(name, smac_xattrkey)) {
		return smac_access(task_self_security(), smac_admin,
				FS_MAY_WRITE);
	}

	if (0 > (res = kfile_xattr_get(node, smac_xattrkey, label,
					SMAC_LABELLEN))) {
		return 0;
	}

	return smac_access(task_self_security(), label, FS_MAY_WRITE);
}

int security_xattr_list(struct node *node, char *list, size_t len) {
	char label[SMAC_LABELLEN];
	int res;

	if (0 > (res = kfile_xattr_get(node, smac_xattrkey, label,
					SMAC_LABELLEN))) {
		return 0;
	}

	return smac_access(task_self_security(), label, FS_MAY_READ);
}
