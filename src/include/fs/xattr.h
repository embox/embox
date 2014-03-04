/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.01.2013
 */

#ifndef FS_XATTR_H_
#define FS_XATTR_H_

#include <fs/node.h>
#include <fs/fs_driver.h>

#define XATTR_CREATE  (1 << 0)
#define XATTR_REMOVE  (1 << 1)
#define XATTR_REPLACE (1 << 2)

#define XATTR_MAX_VSIZE 0xFF
#define XATTR_MAX_NSIZE 32

extern int kfile_xattr_get(struct node *node, const char *name, char *value,
		size_t len);

extern int kfile_xattr_set(struct node *node, const char *name,
		const char *value, size_t len, int flags);

extern int kfile_xattr_list(struct node *node, char *list, size_t len);

#endif /* FS_XATTR_H_ */
