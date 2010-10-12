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

#define CONFIG_QUANTITY_NODE 0x100

typedef struct node {
	const char            name[CONFIG_MAX_LENGTH_FILE_NAME];
	void                 *file_info;
	file_system_driver_t *fs_type;
	struct node          *neighbors;
	struct node          *leaves;
} node_t;

extern node_t *alloc_node(const char *name);
extern void free_node(node_t *node);

#endif /* FS_NODE_H_ */
