/**
 * @file
 * @brief
 *
 * @date 24.04.2012
 * @author Andrey Gazukin
 */
#include <fs/node.h>

#ifndef MOUNT_H_
#define MOUNT_H_

typedef struct mount_params {
	char    *dev;
	char    *dir;
	node_t  *dev_node;
} mount_params_t;

#endif /* MOUNT_H_ */
