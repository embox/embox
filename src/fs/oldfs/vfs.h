/**
 * @file
 * @brief Describes tree of VFS (Virtual Filesystem Switch).
 *
 * @date 15.10.10
 * @author Nikolay Korotky
 */

#ifndef FS_VFS_H_
#define FS_VFS_H_

#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <fs/path.h>

#define LAST_IN_PATH         0x01

//XXX
#include <fs/mount.h>

struct inode;
extern void if_mounted_follow_down(struct path *path);
extern void if_root_follow_up(struct path *path);
extern void vfs_get_root_path(struct path *path);
extern void vfs_get_leaf_path(struct path *path);

/**
 * @brief Get path of node till specified \a parent node (which is not
 * included). If \a node is equal \a parent "" is returned, which is natrual
 * for not directories.
 *
 * @param node Node to get path from
 * @param parent Node path is constructed from.
 * @param path Pointer to char array, where path will be stored.
 * @param plen Size of \a path
 *
 * @return 0 if path is successfully stored
 * @return 1 if \a parent node haven't seen while path is constructed. If so,
 * \a path contains path till natural root (which parent is NULL, including)
 * @return negative on error
 */
extern int vfs_get_pathbynode_tilln(struct path *node, struct path *parent, char *path,
	size_t plen);

extern int vfs_add_leaf(struct inode *child, struct inode *parent);

extern int vfs_del_leaf(struct inode *nod);


extern struct inode *vfs_get_root(void);
extern struct inode *vfs_set_root(struct inode *node);
extern struct inode *vfs_get_leaf(void);

extern int vfs_create(struct path *parent, const char *path, mode_t mode,
		struct path *child);
extern void vfs_create_child(struct path *parent, const char *name, mode_t mode,
		struct path *child);
extern int vfs_create_intermediate(struct path *parent, const char *path, mode_t mode,
		struct path *child);

extern int vfs_lookup(const char *str_path, struct path *path);
extern void vfs_lookup_child(struct path *parent, const char *name, struct path *child);
extern void vfs_lookup_childn(struct path *parent, const char *name, size_t len,
		struct path *child);

extern void vfs_get_parent(struct path *child_path, struct path *parent_path);

extern int vfs_get_child_next(struct path *parent_path,
		struct inode *child_prev, struct path *child_next);

/**
 * Checks the path on the node_tree and forms correct string with the path
 * which already exist.
 *
 * @param path - file system path which you want to get in ideal
 * @param exist_path - buffer for path which will be formed
 * @param buff_len - length of buffer for exist _path parameter
 */
extern void vfs_get_exist_path(const char *path, char *exist_path, size_t buff_len,
		struct path *path_node);

/**
 * @brief Wrapper for \a vfs_get_pathbynode that places leading '/' for every
 * path. Call for this with \a node equal to \a root will result \a path
 * equal "/" in contrast with \a vfs_get_pathbynode_tilln, which will
 * return "" under same conditions.
 *
 * @param nod
 * @param root
 * @param path
 *
 * @return See \a vfs_get_pathbynode_tilln
 */
static inline int vfs_get_path_till_root(struct path *node, struct path *root, char *path,
		size_t pathlen) {

	if_root_follow_up(node);
	if (node->node == root->node) {
		if (pathlen <= 1) {
			return -ERANGE;
		}
		strcpy(path, "/");
		return 0;
	}

	return vfs_get_pathbynode_tilln(node, root, path, pathlen);
}


/**
 * @brief Get path of node in system vfs tree.
 *
 * @param nod Node to get path from
 * @param path Pointer to char array, which size is at least PATH_MAX.
 *
 * @return See \a vfs_get_pathbynode_tilln
 */
static inline int vfs_get_path_by_node(struct path *node, char *path) {
	struct path root;
	vfs_get_root_path(&root);

	return vfs_get_path_till_root(node, &root, path, PATH_MAX);
}

extern int vfs_get_relative_path(struct inode *node, char *path, size_t path_len);

extern struct inode *vfs_subtree_create_child(struct inode *parent, const char *name,
		mode_t mode);

extern struct inode *vfs_subtree_lookup_childn(struct inode *parent, const char *name, size_t len);

extern struct inode *vfs_subtree_lookup(struct inode *parent, const char *str_path);

extern struct inode *vfs_subtree_lookup_child(struct inode *parent, const char *name);

extern struct inode *vfs_subtree_get_child_next(struct inode *parent, struct inode *prev_child);

extern struct inode *vfs_subtree_create(struct inode *parent, const char *path, mode_t mode);

extern struct inode *vfs_subtree_create_intermediate(struct inode *parent, const char *path, mode_t mode);

extern struct inode *vfs_subtree_get_parent(struct inode *node);


#endif /* FS_VFS_H_ */
