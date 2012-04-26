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
#include <util/tree.h>

#define IS_DIRECTORY    0x10

typedef struct node {
	const char            name[CONFIG_MAX_LENGTH_FILE_NAME];
	uint8_t               properties;
	void                 *file_info; /* WTF? maybe introduce Node Attribute Structure(NAS)? (sikmir) */
	struct fs_drv        *fs_type;
	void                 *attr;
	struct tree_link      tree_link;
} node_t;

typedef struct file_create_param {
	void  *node;
	void  *parents_node;
	char   path[CONFIG_MAX_LENGTH_PATH_NAME];
} file_create_param_t;

extern int nip_tail(char *head, char *tail);
extern int increase_tail(char *head, char *tail);

extern node_t *alloc_node(const char *name);
extern void free_node(node_t *node);

#endif /* FS_NODE_H_ */
