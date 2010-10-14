/**
 * @file
 *
 * @date 29.06.2009
 * @author Anton Bondarev
 */
#ifndef FS_ROOTFS_H_
#define FS_ROOTFS_H_

#include <types.h>

#define FILE_MODE_RO    0x1 /*read only     */
#define FILE_MODE_WO    0x2 /*write only    */
#define FILE_MODE_XO    0x4 /*exec only     */
#define FILE_MODE_RW    0x3 /*read write    */
#define FILE_MODE_RX    0x5 /*read execute  */
#define FILE_MODE_WX    0x6 /*write execute */
#define FILE_MODE_RWX   0x7 /*all           */

typedef struct _FILE_INFO {
	char file_name[CONFIG_MAX_LENGTH_FILE_NAME];
	unsigned int size_in_bytes;
	unsigned int size_on_disk;
	unsigned int mode;
} FILE_INFO;

typedef FILE_INFO *(*FS_FILE_ITERATOR)(FILE_INFO *file_info);

typedef void *(*FS_OPEN_FILE_FUNC)(const char *file_name, const char *mode);
typedef int (*FS_CREATE_FUNC)(void *params);
typedef int (*FS_RESIZE_FUNC)(void *params);
typedef int (*FS_DELETE_FUNC)(const char *file_name);
typedef int (*FS_GETCAPACITY_FUNC)(const char *file_name);
typedef int (*FS_GETFREESPACE_FUNC)(const char *file_name);
typedef int (*FS_GETDESCRIPTORSINFO_FUNC)(void *params);
typedef int (*FS_INIT_FUNC)(void);
typedef FS_FILE_ITERATOR (*FS_GETFILELISTITERATOR_FUNC) (void);

typedef struct fsop_desc {
	FS_INIT_FUNC init;
//	FS_OPEN_FILE_FUNC open_file;
	FS_CREATE_FUNC create_file;
//	FS_RESIZE_FUNC resize_file;
	FS_DELETE_FUNC delete_file;
//	FS_GETCAPACITY_FUNC get_capacity;
//	FS_GETFREESPACE_FUNC get_freespace;
//	FS_GETDESCRIPTORSINFO_FUNC get_descriptors_info;
//	FS_GETFILELISTITERATOR_FUNC get_file_list_iterator;
} fsop_desc_t;

#include <fs/file.h>
#if 0
fsop_desc_t *rootfs_get_fsopdesc(char *fs_name);


typedef struct _FILE_NAME_STRUCT {
	/* fs name (flash ramdisc and so on) */
	char fs_name[CONFIG_FS_MAX_DISK_NAME_LENGTH];
	char *file_name;
} FILE_NAME_STRUCT;

FILE_NAME_STRUCT *parse_file_name(const char *file_name, FILE_NAME_STRUCT *file_name_struct);
fsop_desc_t *rootfs_get_fsopdesc(char *fs_name);

void *rootfs_fopen(const char *file_name, const char *mode);
#endif

#endif /* FS_ROOTFS_H_ */
