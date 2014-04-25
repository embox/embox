/*
 * @file
 *
 * @date May 28, 2013
 * @author: Anton Bondarev
 */

#ifndef FS_MOUNT_H_
#define FS_MOUNT_H_


extern int mount(char *dev,  char *dir, char *fs_type);

extern int umount(char *dir);

struct node;
struct path;

struct mount_descriptor {
	struct node *mnt_point;
	struct node *mnt_root;
	struct mount_descriptor *mnt_parent;
	struct dlist_head mnt_mounts;
	struct dlist_head mnt_child;
};

extern struct mount_descriptor *mount_table(void);

extern struct mount_descriptor *mount_table_add(struct path *mnt_point_path, struct node *root);

extern int mount_table_del(struct mount_descriptor *mdesc);

extern struct mount_descriptor *mount_table_get_child(struct mount_descriptor *parent, struct node *mnt_point);


#endif /* FS_MOUNT_H_ */
