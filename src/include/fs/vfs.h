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
#include <fs/node.h>

#define LAST_IN_PATH         0x01


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
extern int vfs_get_pathbynode_tilln(node_t *node, node_t *parent, char *path,
		size_t plen);

extern int vfs_add_leaf(node_t *child, node_t *parent);

extern int vfs_del_leaf(node_t *nod);

extern node_t *vfs_get_root(void);
extern node_t *vfs_get_leaf(void);

extern node_t *vfs_create(node_t *parent, const char *path, mode_t mode);
extern node_t *vfs_create_child(node_t *parent, const char *name, mode_t mode);
extern node_t *vfs_create_intermediate(node_t *parent, const char *path,
		mode_t mode);

extern node_t *vfs_lookup(node_t *parent, const char *path);
extern node_t *vfs_lookup_child(node_t *parent, const char *name);
extern node_t *vfs_lookup_childn(node_t *parent, const char *name, size_t
		npreflen);

extern node_t *vfs_get_parent(node_t *child);

extern node_t *vfs_get_child_next(node_t *parent);

/**
 * Checks the path on the node_tree and forms correct string with the path
 * which already exist.
 *
 * @param path - file system path which you want to get in ideal
 * @param exist_path - buffer for path which will be formed
 * @param buff_len - length of buffer for exist _path parameter
 */
extern node_t *vfs_get_exist_path(const char *path, char *exist_path,
		size_t buff_len);

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
static inline int vfs_get_path_till_root(node_t *nod, node_t *root, char *path,
		size_t pathlen) {

	if (nod == root) {
		if (pathlen <= 1) {
			return -ERANGE;
		}
		strcpy(path, "/");
		return 0;
	}

	return vfs_get_pathbynode_tilln(nod, root, path, pathlen);
}


/**
 * @brief Get path of node in system vfs tree.
 *
 * @param nod Node to get path from
 * @param path Pointer to char array, which size is at least PATH_MAX.
 *
 * @return See \a vfs_get_pathbynode_tilln
 */
static inline int vfs_get_path_by_node(node_t *nod, char *path) {
	return vfs_get_path_till_root(nod, vfs_get_root(), path, PATH_MAX);
}

#endif /* FS_VFS_H_ */
