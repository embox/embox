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
	struct dlist_head mount_link;
};

extern struct dlist_head *mount_table(void);

extern int mount_table_add(struct node *dir_node);

extern int mount_table_del(struct node *dir_node);

extern int mount_table_check(struct node *dir_node);


#endif /* FS_MOUNT_H_ */
