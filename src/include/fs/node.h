/**
 * @file
 * @brief pool of nodes
 *
 * @date 06.10.10
 * @author Nikolay Korotky
 */

#ifndef FS_NODE_H_
#define FS_NODE_H_

#include <sys/stat.h>
#include <fs/file_system.h>
#include <util/tree.h>
#include <limits.h>

struct nas;

typedef struct node {
	/* node name (use vfs_get_path_by_node() for get full path*/
	char                  name[NAME_MAX + 1];

	int                   type;  /* FILE, DIRECTORY, DEVICE, LINK ... */

	mode_t                mode;/* discrete access mode Read-Write-Execution */
	uid_t                 uid;/* owner user ID */
	gid_t                 gid;/* owner group ID */

	/* node attribute structure (extended information about node)*/
	struct nas            *nas;

	/* service data structure for enabling tree operation */
	struct tree_link      tree_link;
} node_t;

struct node_info {
	size_t        size;
	unsigned int  mtime;
};

struct node_fi {
	struct node_info ni;
	void  *privdata;
};

typedef struct nas {
	struct node          *node;
	struct filesystem    *fs;
	struct node_fi       *fi;
} nas_t;

/**
 * @param name Non-empty string.
 * @param name_len (optional) how many bytes to take from name.
 *    If zero, the name must be a null-terminated string.
 */
extern node_t *node_alloc(const char *name, size_t name_len);

extern void node_free(node_t *node);

static inline struct node *node_parent(struct node *node) {
	return tree_element(node->tree_link.par, struct node, tree_link);
}

static inline int node_is_block_dev(struct node *node) {
	return S_ISBLK(node->mode);
}

static inline int node_is_directory(struct node *node) {
	return S_ISDIR(node->mode);
}

static inline int node_is_file(struct node *node) {
	return S_ISREG(node->mode);
}


#endif /* FS_NODE_H_ */
