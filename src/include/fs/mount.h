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

struct mount_descriptor {
	struct node *dir_node;
	struct node *mnt_root;
	struct mount_descriptor *mnt_parent;
	struct dlist_head mnt_link;
};

extern struct dlist_head *mount_table(void);

extern int mount_table_add(struct node *dir_node, struct node *root);
extern struct mount_descriptor *mount_table_find(struct node *dir_node);
extern struct mount_descriptor *mount_table_find_by_root(struct node *root_node);

extern int mount_table_del(struct node *dir_node);

extern int mount_table_check(struct node *dir_node);


#endif /* FS_MOUNT_H_ */
