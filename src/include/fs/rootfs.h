/**
 * @file
 *
 * @date 29.06.2009
 * @author Anton Bondarev
 */
#ifndef ROOTFS_H_
#define ROOTFS_H_

#include <types.h>

#define FILE_MODE_RO    0x1 //read only
#define FILE_MODE_WO    0x2 //write only
#define FILE_MODE_XO    0x4 //exec only
#define FILE_MODE_RW    0x3 //read write
#define FILE_MODE_RX    0x5 //read execute
#define FILE_MODE_WX    0x6 //write execute
#define FILE_MODE_RWX   0x7 //all

typedef struct _FILE_INFO {
	char file_name[0x10];
	unsigned int size_in_bytes;
	unsigned int size_on_disk;
	unsigned int mode;
}FILE_INFO;

typedef FILE_INFO * (*FS_FILE_ITERATOR) (FILE_INFO *file_info);

typedef void *(*FS_OPEN_FILE_FUNC) (const char *file_name, char *mode);
typedef int (*FS_CREATE_FUNC) (void *params);
typedef int (*FS_RESIZE_FUNC) (void *params);
typedef int (*FS_DELETE_FUNC) (const char *file_name);
typedef int (*FS_GETCAPACITY_FUNC) (const char *file_name);
typedef int (*FS_GETFREESPACE_FUNC) (const char *file_name);
typedef int (*FS_GETDESCRIPTORSINFO_FUNC) (void *params);
typedef int (*FS_INIT_FUNC) (void);
typedef FS_FILE_ITERATOR (*FS_GETFILELISTITERATOR_FUNC) (void);

typedef struct _FSOP_DESCRIPTION {
	FS_INIT_FUNC init;
	FS_OPEN_FILE_FUNC open_file;
	FS_CREATE_FUNC create_file;
	FS_RESIZE_FUNC resize_file;
	FS_DELETE_FUNC delete_file;
	FS_GETCAPACITY_FUNC get_capacity;
	FS_GETFREESPACE_FUNC get_freespace;
	FS_GETDESCRIPTORSINFO_FUNC get_descriptors_info;
	FS_GETFILELISTITERATOR_FUNC get_file_list_iterator;
}FSOP_DESCRIPTION;

//int rootfs_init(void);
FSOP_DESCRIPTION *rootfs_get_fsopdesc(char *fs_name);

typedef void * (*FILEOP_OPEN) (const char *file_name, char *mode);
typedef int (*FILEOP_CLOSE) (void * file);
typedef size_t (*FILEOP_READ) (const void *buf, size_t size, size_t count, void *file);
typedef size_t (*FILEOP_WRITE) (const void *buf, size_t size, size_t count, void *file);
typedef int (*FILEOP_FSEEK) (void *file, long offset, int whence);

typedef struct _FILEOP{
	FILEOP_OPEN fopen;
	FILEOP_CLOSE close;
	FILEOP_READ read;
	FILEOP_WRITE write;
	FILEOP_FSEEK fseek;
}FILEOP;

//for parsing filename
#define FS_MAX_DISK_NAME_LENGTH 0x10

typedef struct _FILE_NAME_STRUCT{
	char fs_name[FS_MAX_DISK_NAME_LENGTH];//fs name (flash ramdisc and so on)
	char *file_name; //
}FILE_NAME_STRUCT;

FILE_NAME_STRUCT *parse_file_name(const char *file_name, FILE_NAME_STRUCT *file_name_struct);
FSOP_DESCRIPTION *rootfs_get_fsopdesc(char *fs_name);
void *rootfs_fopen(const char *file_name, char *mode);

#endif /* ROOTFS_H_ */
