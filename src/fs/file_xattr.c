/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    03.12.2013
 */

#include <fs/file_desc.h>
#include <fs/xattr.h>
#include <util/member.h>

#include <fs/idesc.h>

static int file_idesc_getxattr(struct idesc *idesc, const char *name, void *value,
		size_t size);
static int file_idesc_setxattr(struct idesc *idesc, const char *name,
		const void *value, size_t size, int flags);
static int file_idesc_listxattr(struct idesc *idesc, char *list, size_t size);
static int file_idesc_removexattr(struct idesc *idesc, const char *name);

const struct idesc_xattrops file_idesc_xattrops = {
	.getxattr = file_idesc_getxattr,
	.setxattr = file_idesc_setxattr,
	.listxattr = file_idesc_listxattr,
	.removexattr = file_idesc_removexattr,
};

static int file_idesc_getxattr(struct idesc *idesc, const char *name, void *value,
		size_t size) {
	struct file_desc *desc;

	desc = member_cast_out(idesc, struct file_desc, f_idesc);

	return kfile_xattr_get(desc->f_inode, name, value, size);
}

static int file_idesc_setxattr(struct idesc *idesc, const char *name,
		const void *value, size_t size, int flags) {
	struct file_desc *desc;

	desc = member_cast_out(idesc, struct file_desc, f_idesc);

	return kfile_xattr_set(desc->f_inode, name, value, size, flags);
}

static int file_idesc_listxattr(struct idesc *idesc, char *list, size_t size) {
	struct file_desc *desc;

	desc = member_cast_out(idesc, struct file_desc, f_idesc);

	return kfile_xattr_list(desc->f_inode, list, size);
}

static int file_idesc_removexattr(struct idesc *idesc, const char *name) {
	struct file_desc *desc;

	desc = member_cast_out(idesc, struct file_desc, f_idesc);

	return kfile_xattr_set(desc->f_inode, name, NULL, 0, XATTR_REMOVE);
}

