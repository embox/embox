/**
 * @file
 *
 * @date 05.10.2010
 * @author Nikolay Korotky
 */
#ifndef FS_H_
#define FS_H_

#include <lib/list.h>
#include <fs/rootfs.h>

/**
 * Structure of file system driver.
 * We can mount some file system with name of FS which has been registered in
 * our system.
 */
typedef struct file_system_driver {
	struct list_head *next;
	struct list_head *prev;
	const char *name;

	file_op_t file_op;
	fsop_desc_t *fsop;
#if 0
int fs_flags;
struct list_head fs_supers;
#endif
} file_system_driver_t;

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
