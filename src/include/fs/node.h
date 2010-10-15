/**
 * @file
 * @brief pool of nodes
 *
 * @date 06.10.10
 * @author Nikolay Korotky
 */

#ifndef FS_NODE_H_
#define FS_NODE_H_

#include <fs/fs.h>

typedef struct node {
	const char            name[CONFIG_MAX_LENGTH_FILE_NAME];
	void                 *file_info;
	file_system_driver_t *fs_type;
	struct list_head neighbors;
	struct list_head leaves;
} node_t;

extern node_t *alloc_node(const char *name);
extern void free_node(node_t *node);

#endif /* FS_NODE_H_ */
