/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.02.2013
 */

#include <string.h>
#include <sys/stat.h>

#include <fs/inode.h>
#include <kernel/task.h>
#include <fs/xattr.h>
#include <fs/idesc.h>
#include <net/sock.h>
#include <security/smac/smac.h>

#include <security/security.h>

#include <kernel/task/resource/security.h>

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
static int node_getlabel(struct inode *n, char *label, size_t lablen) {
	int res = 0;

	if (0 > (res = kfile_xattr_get(n, smac_xattrkey, label,
					lablen))) {
		strcpy(label, smac_def_file_label);
	}

	return 1;
}

static int node_setlabel(struct inode *n, const char *label) {
	return kfile_xattr_set(n, smac_xattrkey, label, strlen(label) + 1, 0);
}

static int idesc_getlabel(struct idesc *idesc, char *label, size_t lablen) {
	int res = 0;

	if (0 > (res = idesc_getxattr(idesc, smac_xattrkey, label,
					lablen))) {
		strcpy(label, smac_def_file_label);
	}

	return 1;
}

static int security_xattr_is_service_access(const char *name, int may_access,
		struct smac_audit *audit) {
	int res;

	if (0 != strcmp(name, smac_xattrkey)) {
		return 1;
	}

	res = smac_access(task_self_resource_security(), smac_admin,
			may_access, audit);
	assert(res != 1);
	return res;
}

int security_node_create(struct inode *dir, mode_t mode) {
	char label[SMAC_LABELLEN];
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__, dir->name);

	if (0 >= (res = node_getlabel(dir, label, SMAC_LABELLEN))) {
		return res;
	}

	return smac_access(task_self_resource_security(), label, S_IWOTH, &audit);
}

void security_node_cred_fill(struct inode *node) {
	node_setlabel(node, task_self_resource_security());
}

int security_node_permissions(struct inode *node, int flags) {
	char label[SMAC_LABELLEN];
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__, node->name);

	if (0 >= (res = node_getlabel(node, label, SMAC_LABELLEN))) {
		return res;
	}

	return smac_access(task_self_resource_security(), label, flags, &audit);
}

int security_node_delete(struct inode *dir, struct inode *node) {
	return 0;
}

int security_mount(struct inode *dev, struct inode *mountpoint) {
	return 0;
}

int security_umount(struct inode *mountpoint) {
	return 0;
}

int security_xattr_get(struct inode *node, const char *name, char *value,
		size_t len) {
	char label[SMAC_LABELLEN];
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__, node->name);

	if (1 != (res = security_xattr_is_service_access(name, S_IROTH,
					&audit))) {
		return res;
	}

	if (0 >= (res = node_getlabel(node, label, SMAC_LABELLEN))) {
		return res;
	}

	return smac_access(task_self_resource_security(), label, S_IROTH, &audit);
}

int security_xattr_set(struct inode *node, const char *name,
			const char *value, size_t len, int flags) {
	char label[SMAC_LABELLEN];
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__, node->name);

	if (1 != (res = security_xattr_is_service_access(name, S_IWOTH,
					&audit))) {
		return res;
	}

	if (0 >= (res = node_getlabel(node, label, SMAC_LABELLEN))) {
		return res;
	}

	return smac_access(task_self_resource_security(), label, S_IWOTH, &audit);
}

int security_xattr_list(struct inode *node, char *list, size_t len) {
	char label[SMAC_LABELLEN];
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__, node->name);

	if (0 >= (res = node_getlabel(node, label, SMAC_LABELLEN))) {
		return res;
	}

	return smac_access(task_self_resource_security(), label, S_IROTH, &audit);
}

int security_xattr_idesc_get(struct idesc *idesc, const char *name, char *value, size_t len) {
	char label[SMAC_LABELLEN];
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__, NULL);

	if (1 != (res = security_xattr_is_service_access(name, S_IROTH,
					&audit))) {
		return res;
	}

	if (0 >= (res = idesc_getlabel(idesc, label, SMAC_LABELLEN))) {
		return res;
	}

	return smac_access(task_self_resource_security(), label, S_IROTH, &audit);
}

int security_xattr_idesc_set(struct idesc *idesc, const char *name, const char *value, size_t len, int flags) {
	char label[SMAC_LABELLEN];
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__, NULL);

	if (1 != (res = security_xattr_is_service_access(name, S_IWOTH,
					&audit))) {
		return res;
	}

	if (0 >= (res = idesc_getlabel(idesc, label, SMAC_LABELLEN))) {
		return res;
	}

	return smac_access(task_self_resource_security(), label, S_IROTH, &audit);
}

int security_xattr_idesc_list(struct idesc *idesc, char *list, size_t len) {
	char label[SMAC_LABELLEN];
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__, NULL);

	if (0 >= (res = idesc_getlabel(idesc, label, SMAC_LABELLEN))) {
		return res;
	}

	return smac_access(task_self_resource_security(), label, S_IROTH, &audit);
}

int security_sock_create(struct sock *sock) {
	char *secure_label;
	if (NULL != (secure_label = task_resource_security(task_self()))) {
		if ((0 != strcmp(secure_label, smac_floor)) &&
				(0 != strcmp(secure_label, smac_admin))) { //FIXME problem with su -c dropbeard
			idesc_setxattr(&sock->idesc, smac_xattrkey, secure_label, strlen(secure_label) + 1, 0);
		}
	}
	return 0;
}

int security_sock_label(struct sock *sock, char *label, size_t len) {
	return idesc_getxattr(&sock->idesc, smac_xattrkey, label, len);
}
