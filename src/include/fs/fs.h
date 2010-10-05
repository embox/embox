/**
 * @file
 *
 * @date 05.10.2010
 * @author Nikolay Korotky
 */
#ifndef FS_H_
#define FS_H_

#include <lib/list.h>

typedef struct {
        const char *name;
        int fs_flags;
        struct file_system_type *next;
        struct list_head fs_supers;
} file_system_type;

/**
 * register a new filesystem
 * @param fs the file system structure
 */
extern int register_filesystem(file_system_type *);

/**
 * unregister a file system
 * @param fs filesystem to unregister
 */
extern int unregister_filesystem(file_system_type *);

#endif /* FS_H_ */
