/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.01.2013
 */

#ifndef FS_XATTR_H_
#define FS_XATTR_H_

#include <sys/types.h>
#include <sys/xattr.h>

#define XATTR_MAX_VSIZE 0xFF
#define XATTR_MAX_NSIZE 32

struct inode;
extern int kfile_xattr_get(struct inode *node, const char *name, char *value,
		size_t len);

extern int kfile_xattr_set(struct inode *node, const char *name,
		const char *value, size_t len, int flags);

extern int kfile_xattr_list(struct inode *node, char *list, size_t len);

#endif /* FS_XATTR_H_ */
