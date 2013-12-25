/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.11.2013
 */

#ifndef FS_XATTR_LIST_H_
#define FS_XATTR_LIST_H_

#include <fs/xattr.h>

struct xattr_ent {
	struct dlist_head xe_lnk;
	char xe_name[XATTR_MAX_NSIZE];
	char xe_val[XATTR_MAX_VSIZE];
	size_t xe_len;
};

struct xattr_list {
	struct dlist_head xl_head;
};

extern void xattr_list_init(struct xattr_list *xlst);
extern struct xattr_list *xattr_list_create(void);
extern void xattr_list_delete(struct xattr_list *xlst);

extern int getxattr_generic(struct xattr_list *xlst, const char *name,
		char *value, size_t len);
extern int setxattr_generic(struct xattr_list *xlst, const char *name,
			const char *value, size_t len, int flags);
extern int listxattr_generic(struct xattr_list *xlst, char *list, size_t len);
extern int removexattr_generic(struct xattr_list *xlst, const char *name);

#endif /* FS_XATTR_LIST_H_ */

