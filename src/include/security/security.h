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

extern int security_node_create(struct node *dir, mode_t mode);

extern int security_node_permissions(struct node *node, int flags);

#endif /* SECURITY_SECURITY_H_ */
