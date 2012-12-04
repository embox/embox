/**
 * @file
 * @brief Describes tree of VFS (Virtual Filesystem Switch).
 *
 * @date 15.10.10
 * @author Nikolay Korotky
 */

#ifndef FS_VFS_H_
#define FS_VFS_H_

#include <fs/node.h>

#define LAST_IN_PATH         0x01


extern int vfs_get_path_by_node (node_t *nod, char *path);

extern int vfs_add_leaf(node_t *child, node_t *parrent);

extern int vfs_del_leaf(node_t *nod);

extern node_t *vfs_get_root(void);

extern node_t *vfs_add_path(const char *path, node_t *parrent);

extern node_t *vfs_get_child(const char *name, node_t *parrent);

extern node_t *vfs_get_parent(node_t *child);

extern node_t *vfs_find_node(const char *path, node_t *parrent);

/**
 * Checks the path on the node_tree and forms correct string with the path which already exist.
 *
 * @param path - file system path which you want to get in ideal
 * @param exist_path - buffer for path which will be formed
 * @param buff_len - length of buffer for exist _path parameter
 */
extern node_t *vfs_get_exist_path(const char *path, char *exist_path, size_t buff_len);

#endif /* FS_VFS_H_ */
