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
#define FILE_NODE_TYPE       0x00
#define DIRECTORY_NODE_TYPE  0x10

extern int nip_tail(char *head, char *tail);
extern int increase_tail(char *head, char *tail);
extern int vfs_add_leaf(node_t *child, node_t *parrent);
extern int vfs_del_leaf(node_t *nod);
extern node_t *vfs_add_path(const char *path, node_t *parrent);
extern node_t *vfs_find_child(const char *name, node_t *parrent);
extern node_t *vfs_find_parent(const char *name, node_t *parrent);
extern node_t *vfs_find_node(const char *path, node_t *parrent);

#endif /* FS_VFS_H_ */
