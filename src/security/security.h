/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.02.2013
 */

#ifndef SECURITY_SECURITY_H_
#define SECURITY_SECURITY_H_

#include <stddef.h>
#include <sys/types.h>

struct inode;

/**
 * @brief Checks permission to create node in @a dir with mode @a mode
 * @a dir is already checked for #S_IWOTH
 *
 * @param dir directory node where new node is creating
 * @param mode mode of creating node
 *
 * @return 0 on allowed
 * @return -EACCES in access denied
 */
extern int security_node_create(struct inode *dir, mode_t mode);

/**
 * @brief Sets node credentials of current user
 *
 * @param node
 */
extern void security_node_cred_fill(struct inode *node);

/**
 * @brief Checks permission on node delete. @a dir already checked for
 * #S_IWOTH
 *
 * @param dir directory node from where node is deletting
 * @param node node is deletting
 *
 * @return 0 on allowed
 * @return -EACCES in access denied
 */
extern int security_node_delete(struct inode *dir, struct inode *node);

/**
 * @brief Checks node for access.
 *
 * @param node node to check
 * @param flags access flags in MAY format
 *
 * @return 0 on allowed
 * @return -EACCES in access denied
 */
extern int security_node_permissions(struct inode *node, int flags);

/**
 * @brief Checks for mount operation. @a dev is already checked for
 * #S_IROTH and #S_IXOTH
 *
 * @param dev device node which is mounted
 *
 * @return 0 on allowed
 * @return -EACCES in access denied
 */
extern int security_mount(struct inode *dev, struct inode *mountpoint);

/**
 * @brief Checks for umount operation.
 *
 * @param mountpoint root directory node
 *
 * @return 0 on allowed
 * @return -EACCES in access denied
 */
extern int security_umount(struct inode *mountpoint);

/**
 * @brief Check for xattr get.
 *
 * @param node
 * @param name
 * @param value
 * @param len
 *
 * @return 0 on allowed
 * @return -EACCES on denied
 *
 */
extern int security_xattr_get(struct inode *node, const char *name, char *value, size_t len);

/**
 * @brief Check for xattr set.
 *
 * @param node
 * @param name
 * @param value
 * @param len
 * @param flags
 *
 * @return 0 on allowed
 * @return -EACCES on denied
 *
 */
extern int security_xattr_set(struct inode *node, const char *name,
			const char *value, size_t len, int flags);

/**
 * @brief Check for xattr set.
 *
 * @param node
 *
 * @return 0 on allowed
 * @return -EACCES on denied
 *
 */
extern int security_xattr_list(struct inode *node, char *list, size_t len);

struct idesc;

extern int security_xattr_idesc_get(struct idesc *idesc, const char *name, char *value, size_t len);

extern int security_xattr_idesc_set(struct idesc *idesc, const char *name, const char *value, size_t len, int flags);

extern int security_xattr_idesc_list(struct idesc *idesc, char *list, size_t len);

struct sock;
extern int security_sock_create(struct sock *sock);
extern int security_sock_label(struct sock *sock, char *label, size_t len);


#endif /* SECURITY_SECURITY_H_ */
