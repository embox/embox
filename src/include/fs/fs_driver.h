/**
 * @file
 *
 * @date 05.10.10
 * @author Nikolay Korotky
 */

#ifndef FS_DRV_H_
#define FS_DRV_H_


#include <util/array.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>

struct node;

struct fsop_desc {
	int (*init)(void *par);
	int (*format)(void *par);
	int (*mount)(void *dev_node, void *dir_node);
	int (*create_node)(struct node *parent_node, struct node *new_node);
	int (*delete_node)(struct node *node);

	/* TODO: consider following to accept nas * as first arg (Anton Kozlov) */
	int (*getxattr)(struct node *node, const char *name,
			char *value, size_t len);
	int (*setxattr)(struct node *node, const char *name,
			const char *value, size_t len, int flags);
	int (*listxattr)(struct node *node, char *list, size_t len);

	int (*truncate)(struct node *node, off_t length);
	int (*umount)(void *dir_node);
};

struct kfile_operations;

/**
 * Structure of file system driver.
 * We can mount some file system with name of FS which has been registered in
 * our system.
 */
struct fs_driver {
	const char                    *name;
	bool                          mount_dev_by_string;
	const struct kfile_operations *file_op;
	const struct fsop_desc        *fsop;
};

#define DECLARE_FILE_SYSTEM_DRIVER(fs_driver_)      \
	ARRAY_SPREAD_DECLARE(const struct fs_driver *, \
			__fs_drivers_registry);                \
	ARRAY_SPREAD_ADD(__fs_drivers_registry, \
			&fs_driver_)

/**
 * allocate structure for fs_driver structure
 * @return pointer to allocated memory
 */
extern struct fs_driver *alloc_fs_drivers(void);

/**
 * free early allocated driver with function alloc_fs_drivers
 */
extern void free_fs_drivers(struct fs_driver *);

extern struct fs_driver *fs_driver_find_drv(const char *name);

/**
 * register a new filesystem driver
 * @param fs the file system structure
 */
extern int fs_driver_register_drv(struct fs_driver *);

/**
 * unregister a file system driver
 * @param fs filesystem to unregister
 */
extern int fs_driver_unregister_drv(struct fs_driver *);

#endif /* FS_DRV_H_ */
