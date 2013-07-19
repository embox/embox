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

#include <module/embox/security/smac.h>
#include <framework/mod/options.h>

const char *smac_def_file_label = OPTION_STRING_GET(default_file_label);

/**
 * @brief Get label from node
 *
 * @param n
 * @param label
 * @param lablen
 *
 * @return 0 or negative when further mac check is meaningless
 * @return positive when label stored
 */
static int node_getlabel(struct node *n, char *label, size_t lablen) {
	int res = 0;

	if (0 > (res = kfile_xattr_get(n, smac_xattrkey, label,
					lablen))) {
		strcpy(label, smac_def_file_label);
	}

	return 1;
}


int security_node_create(struct node *dir, mode_t mode) {
	char label[SMAC_LABELLEN];
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__);

	if (0 >= (res = node_getlabel(dir, label, SMAC_LABELLEN))) {
		return res;
	}

	return smac_access(task_self_security(), label, FS_MAY_WRITE, &audit);
}

int security_node_permissions(struct node *node, int flags) {
	char label[SMAC_LABELLEN];
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__);

	if (0 >= (res = node_getlabel(node, label, SMAC_LABELLEN))) {
		return res;
	}

	return smac_access(task_self_security(), label, flags, &audit);
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
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__);

	if (0 == strcmp(name, smac_xattrkey)) {
		return smac_access(task_self_security(), smac_admin,
				FS_MAY_READ, &audit);
	}

	if (0 >= (res = node_getlabel(node, label, SMAC_LABELLEN))) {
		return res;
	}

	return smac_access(task_self_security(), label, FS_MAY_READ, &audit);
}

int security_xattr_set(struct node *node, const char *name,
			const char *value, size_t len, int flags) {
	char label[SMAC_LABELLEN];
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__);

	if (0 == strcmp(name, smac_xattrkey)) {
		return smac_access(task_self_security(), smac_admin,
				FS_MAY_WRITE, &audit);
	}

	if (0 >= (res = node_getlabel(node, label, SMAC_LABELLEN))) {
		return res;
	}

	return smac_access(task_self_security(), label, FS_MAY_WRITE, &audit);
}

int security_xattr_list(struct node *node, char *list, size_t len) {
	char label[SMAC_LABELLEN];
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__);

	if (0 >= (res = node_getlabel(node, label, SMAC_LABELLEN))) {
		return res;
	}

	return smac_access(task_self_security(), label, FS_MAY_READ, &audit);
}
