/**
 * @file
 *
 * @date 05.10.10
 * @author Nikolay Korotky
 */

#ifndef FS_DRV_H_
#define FS_DRV_H_


#include <fs/file_operation.h>
#include <util/array.h>

typedef int (fsop_init_ft)(void *par);
typedef int (fsop_format_ft)(void *par);
typedef int (fsop_mount_ft)(void *dev_node, void *dir_node);
typedef int (fsop_create_ft)(struct node *parent_node, struct node *new_node);
typedef int (fsop_delete_ft)(struct node *node);

/* TODO: consider following to accept nas * as first arg (Anton Kozlov) */
typedef int (fsop_getxattr_ft)(struct node *node, const char *name,
                char *value, size_t len);
typedef int (fsop_setxattr_ft)(struct node *node, const char *name,
                const char *value, size_t len, int flags);
typedef int (fsop_listxattr_ft)(struct node *node, char *list,
                size_t len);

typedef struct fsop_desc {
        fsop_init_ft      *init;
        fsop_format_ft    *format;
        fsop_mount_ft     *mount;
        fsop_create_ft    *create_node;
        fsop_delete_ft    *delete_node;

        fsop_getxattr_ft  *getxattr;
        fsop_setxattr_ft  *setxattr;
        fsop_listxattr_ft *listxattr;
} fsop_desc_t;

struct kfile_operations;
/**
 * Structure of file system driver.
 * We can mount some file system with name of FS which has been registered in
 * our system.
 */
typedef struct fs_drv {
        const char                    *name;
        const struct kfile_operations *file_op;
        const fsop_desc_t             *fsop;
} fs_drv_t;


extern const fs_drv_t * __fs_drivers_registry[];

#define DECLARE_FILE_SYSTEM_DRIVER(fs_driver) \
                ARRAY_SPREAD_ADD(__fs_drivers_registry, &fs_driver)

/**
 * allocate structure for fs_driver structure
 * @return pointer to allocated memory
 */
extern fs_drv_t *alloc_fs_drivers(void);

/**
 * free early allocated driver with function alloc_fs_drivers
 */
extern void free_fs_drivers(fs_drv_t *);

extern fs_drv_t *fs_driver_find_drv(const char *name);

/**
 * register a new filesystem driver
 * @param fs the file system structure
 */
extern int fs_driver_register_drv(fs_drv_t *);

/**
 * unregister a file system driver
 * @param fs filesystem to unregister
 */
extern int fs_driver_unregister_drv(fs_drv_t *);

#endif /* FS_DRV_H_ */
