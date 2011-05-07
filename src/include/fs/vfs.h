/**
 * @file
 * @brief Describes tree of VFS (Virtual Filesystem Switch).
 *
 * @date 15.10.10
 * @author Nikolay Korotky
 */

#ifndef VFS_H_
#define VFS_H_

#include <fs/node.h>

extern int vfs_add_leaf(node_t *child, node_t *parrent);
extern node_t *vfs_add_path(const char *path, node_t *parrent);
extern int vfs_del_leaf(node_t *nod);
extern node_t *vfs_find_child(const char *name, node_t *parrent);
extern node_t *vfs_find_node(const char *path, node_t *parrent);

#endif /* VFS_H_ */
