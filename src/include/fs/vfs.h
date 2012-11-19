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

extern int vfs_get_path_by_node (node_t *nod, char *path);
extern int vfs_add_leaf(node_t *child, node_t *parrent);
extern int vfs_del_leaf(node_t *nod);
extern node_t *vfs_get_root(void);
extern node_t *vfs_add_path(const char *path, node_t *parrent);
extern node_t *vfs_find_child(const char *name, node_t *parrent);
extern node_t *vfs_find_parent(const char *name, node_t *parrent);
extern node_t *vfs_find_node(const char *path, node_t *parrent);

extern void path_cut_mount_dir(char *path, char *mount_dir) ;
extern int path_nip_tail(char *head, char *tail);
extern int path_increase_tail(char *head, char *tail);
extern char *path_dir_to_canonical(char *dest, char *src, char dir);
extern char *path_canonical_to_dir(char *dest, char *src);

#endif /* FS_VFS_H_ */
