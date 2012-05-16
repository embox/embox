/**
 * @file
 *
 * @date 05.10.10
 * @author Nikolay Korotky
 */

#ifndef FS_DRV_H_
#define FS_DRV_H_

#include <lib/list.h>
#include <types.h>
#include <stdio.h>
#include <kernel/file.h>
#include <unistd.h>
#include <fs/file_desc.h>


typedef int (*FS_INIT_FUNC)(void *par);
typedef int (*FS_FORMAT_FUNC)(void *par);
typedef int (*FS_MOUNT_FUNC)(void *par);
typedef int (*FS_CREATE_FUNC)(void *par);
typedef int (*FS_DELETE_FUNC)(const char *file_name);

typedef struct fsop_desc {
        FS_INIT_FUNC init;
        FS_FORMAT_FUNC format;
        FS_MOUNT_FUNC mount;
        FS_CREATE_FUNC create_file;
        FS_DELETE_FUNC delete_file;
} fsop_desc_t;

/**
 * Structure of file system driver.
 * We can mount some file system with name of FS which has been registered in
 * our system.
 */
typedef struct fs_drv {
	const char                   *name;
	const struct file_operations *file_op;
	const fsop_desc_t            *fsop;
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

extern fs_drv_t *filesystem_find_drv(const char *name);

/**
 * register a new filesystem
 * @param fs the file system structure
 */
extern int filesystem_register_drv(fs_drv_t *);

/**
 * unregister a file system
 * @param fs filesystem to unregister
 */
extern int filesystem_unregister_drv(fs_drv_t *);

#endif /* FS_DRV_H_ */
