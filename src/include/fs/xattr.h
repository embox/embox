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
#include <fs/fs_drv.h>

/**
 * @brief Get xattr
 *
 * @param path
 * @param name
 * @param value Place @value to store
 * @param len Max length of @value
 *
 * @return
 *	Negative on error, positive number of bytes stored
 */
extern int getxattr(const char *path, const char *name, char *value, size_t len);

/**
 * @brief Set xattr
 *
 * @param path
 * @param name
 * @param value
 *	if NULL, remove xattr
 * @param len Lenght of @value
 * @param flags
 *
 * @return
 *	Negativ on error, zero at success
 */
extern int setxattr(const char *path, const char *name, const char *value,
		size_t len, int flags);

/**
 * @brief List xattr keys separated by \0
 *
 * @param node
 * @param list
 * @param len Max length of list
 *
 * @return
 *	Negative on error, positive number of bytes stored
 */
extern int listxattr(const char *path, char *list, size_t len);

#endif /* FS_XATTR_H_ */
