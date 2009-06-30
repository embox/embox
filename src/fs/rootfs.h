/**
 * \file rootfs.h
 * \date Jun 29, 2009
 * \author anton
 * \details
 */
#ifndef ROOTFS_H_
#define ROOTFS_H_

#define FILE_MODE_RO    0x1 //read only
#define FILE_MODE_WO    0x2 //write only
#define FILE_MODE_EO    0x4 //exec only
#define FILE_MODE_RW    0x3 //read write
#define FILE_MODE_RE    0x5 //read execute
#define FILE_MODE_WE    0x6 //write execute
#define FILE_MODE_RWE   0x7 //all

typedef void *(*FS_OPEN_FILE_FUNC) (const char *file_name, char *mode);
typedef int (*FS_CREATE_FUNC) (void *params);
typedef int (*FS_RESIZE_FUNC) (void *params);
typedef int (*FS_DELETE_FUNC) (const char *file_name);
typedef int (*FS_GETCAPACITY_FUNC) (const char *file_name);
typedef int (*FS_GETFREESPACE_FUNC) (const char *file_name);
typedef int (*FS_GETDESCRIPTORSINFO_FUNC) (void *params);
typedef int (*FS_INIT_FUNC) (void);

typedef struct _FSOP_DESCRIPTION {
    //unsigned char name[0x10];
    FS_INIT_FUNC init;
    FS_OPEN_FILE_FUNC open_file;
    FS_FCREATE_FUNC create_file;
    FS_FRESIZE_FUNC resize_file;
    FS_FDELETE_FUNC delete_file;
    FS_FGETCAPACITY_FUNC get_capacity;
    FS_FGETFREESPACE_FUNC get_freespace;
    FS_GETDESCRIPTORSINFO_FUNC get_descriptors_info;
}FSOP_DESCRIPTION;
#define FS_MAX_DISK_NAME_LENGTH 0x10

typedef struct _FS_DESCRIPTION {
    unsigned char name[FS_MAX_DISK_NAME_LENGTH];
    FSOP_DESCRIPTION *fsop;
}FS_DESCRIPTION;

int rootfs_init();



#endif /* ROOTFS_H_ */
