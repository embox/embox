/**
 * @file
 *
 * @date 05.10.2010
 * @author Nikolay Korotky
 */
#ifndef FS_H_
#define FS_H_

#include <lib/list.h>
#include <fs/file.h>


//typedef void *(*FS_OPEN_FILE_FUNC)(const char *file_name, const char *mode);
typedef int (*FS_CREATE_FUNC)(void *params);
//typedef int (*FS_RESIZE_FUNC)(void *params);
typedef int (*FS_DELETE_FUNC)(const char *file_name);
//typedef int (*FS_GETCAPACITY_FUNC)(const char *file_name);
//typedef int (*FS_GETFREESPACE_FUNC)(const char *file_name);
//typedef int (*FS_GETDESCRIPTORSINFO_FUNC)(void *params);
typedef int (*FS_INIT_FUNC)(void *par);

typedef struct fsop_desc {
        FS_INIT_FUNC init;
//      FS_OPEN_FILE_FUNC open_file;
        FS_CREATE_FUNC create_file;
//      FS_RESIZE_FUNC resize_file;
        FS_DELETE_FUNC delete_file;
//      FS_GETCAPACITY_FUNC get_capacity;
//      FS_GETFREESPACE_FUNC get_freespace;
//      FS_GETDESCRIPTORSINFO_FUNC get_descriptors_info;
//      FS_GETFILELISTITERATOR_FUNC get_file_list_iterator;
} fsop_desc_t;

/**
 * Structure of file system driver.
 * We can mount some file system with name of FS which has been registered in
 * our system.
 */
typedef struct file_system_driver {
	const char          *name;
	file_operations_t   *file_op;
	fsop_desc_t         *fsop;
#if 0
	int fs_flags;
	struct list_head fs_supers;
#endif
} file_system_driver_t;

#define DECLARE_FILE_SYSTEM_DRIVER(fs_driver) \
	static const file_system_driver_t *p##fs_driver \
		__attribute__ ((used, section(".drivers.fs"))) \
		= &fs_driver

/**
 * allocate structure for fs_driver structure
 * @return pointer to allocated memory
 */
extern file_system_driver_t *alloc_fs_drivers(void);

/**
 * free early allocated driver with function alloc_fs_drivers
 */
extern void free_fs_drivers(file_system_driver_t *);

extern file_system_driver_t *find_filesystem(const char *name);

/**
 * register a new filesystem
 * @param fs the file system structure
 */
extern int register_filesystem(file_system_driver_t *);

/**
 * unregister a file system
 * @param fs filesystem to unregister
 */
extern int unregister_filesystem(file_system_driver_t *);

#endif /* FS_H_ */
