/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    04.02.2013
 */

#ifndef POSIX_SYS_XATTR_H_
#define POSIX_SYS_XATTR_H_

#include <fs/xattr.h>

/**
 * @brief Get xattr
 *
 * @param path
 * @param name Key
 * @param value Buffer to store value
 * @param len @value buffer size
 *
 * @return Number of bytes successfully written or number of bytes
 * needed to complete
 */
extern int getxattr(const char *path, const char *name, char *value, size_t size);

/**
 * @brief Get xattr
 *
 * @param fd
 * @param name Key
 * @param value Buffer to store value
 * @param size @value buffer size
 *
 * @return Number of bytes successfully written or number of bytes
 * needed to complete
 */
extern int fgetxattr(int fd, const char *name, void *value, size_t size);

/**
 * @brief Set xattr
 *
 * @param path
 * @param name
 * @param value
 * @param len Lenght of @value
 * @param flags
 *
 * @return Zero on succes, -1 otherwise and errno setted
 */
extern int setxattr(const char *path, const char *name, const char *value,
		size_t len, int flags);

/**
 * @brief Set xattr
 *
 * @param fd File desc
 * @param name Key
 * @param value Value
 * @param len Size of value
 * @param flags Modifiers of operation
 *
 * @return Zero on succes, -1 otherwise and errno setted
 */
extern int fsetxattr(int fd, const char *name, const char *value,
		size_t len, int flags);
/**
 * @brief List xattr keys separated by \0
 *
 * @param path Path
 * @param list Buffer to store keys or NULL
 * @param len @list size or zero
 *
 * @return Number of bytes successfully written or number of bytes
 * needed to complete.
 */
extern int listxattr(const char *path, char *list, size_t len);

/**
 * @brief List xattr keys separated by \0
 *
 * @param fd File desc
 * @param list Buffer to store keys or NULL
 * @param len @list size or zero
 *
 * @return Number of bytes successfully written or number of bytes
 * needed to complete.
 */
extern int flistxattr(int fd, char *list, size_t len);

#endif /* POSIX_SYS_XATTR_H_ */
