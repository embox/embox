/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.02.2013
 */

#ifndef SECURITY_SECURITY_H_
#define SECURITY_SECURITY_H_

#include <types.h>

struct node;

/**
 * @brief Checks permission to create node in @a dir with mode @a mode
 * @a dir is already checked for #FS_MAY_WRITE
 *
 * @param dir directory node where new node is creating
 * @param mode mode of creating node
 *
 * @return 0 on allowed
 * @return -EACCESS in access denied
 */
extern int security_node_create(struct node *dir, mode_t mode);

/**
 * @brief Checks permission on node delete. @a dir already checked for
 * #FS_MAY_WRITE
 *
 * @param dir directory node from where node is deletting
 * @param node node is deletting
 *
 * @return 0 on allowed
 * @return -EACCESS in access denied
 */
extern int security_node_delete(struct node *dir, struct node *node);

/**
 * @brief Checks node for access.
 *
 * @param node node to check
 * @param flags access flags in MAY format
 *
 * @return 0 on allowed
 * @return -EACCESS in access denied
 */
extern int security_node_permissions(struct node *node, int flags);

/**
 * @brief Checks for mount operation. @a dev is already checked for
 * #FS_MAY_READ and #FS_MAY_EXEC
 *
 * @param dev device node which is mounted
 *
 * @return 0 on allowed
 * @return -EACCESS in access denied
 */
extern int security_mount(struct node *dev, struct node *mountpoint);

#endif /* SECURITY_SECURITY_H_ */
