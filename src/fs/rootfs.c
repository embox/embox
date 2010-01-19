/**
 * @file rootfs.c
 * @date Jun 29, 2009
 * @author Anton Bondarev
 * @details
 */
#include "string.h"
#include "common.h"
#include "fs/rootfs.h"
#include "fs/ramfs.h"
#include "kernel/init.h"

DECLARE_INIT("rootfs", rootfs_init, INIT_FS_LEVEL);


typedef struct _FS_DESCRIPTION {
     const char * name;
    const FSOP_DESCRIPTION *fsop;
}FS_DESCRIPTION;


static FS_DESCRIPTION const fs_list[] = {
#include "rootfs_desc.inc"
};
#define NUMBER_OF_FS    array_len(fs_list)


static int file_list_cnt;
static FILE_INFO * file_list_iterator(FILE_INFO *finfo){
    if (NUMBER_OF_FS  <= file_list_cnt)
        return NULL;
    strncpy (finfo->file_name, fs_list[file_list_cnt].name, array_len(finfo->file_name));
    finfo->mode = FILE_MODE_RO;
    finfo->size_in_bytes = 0;
    finfo->size_on_disk = 0;
    file_list_cnt ++;
    return finfo;
}

static FS_FILE_ITERATOR get_file_list_iterator(void){
    file_list_cnt = 0;
    return file_list_iterator;
}

static FSOP_DESCRIPTION rootfs_op = { get_file_list_iterator : get_file_list_iterator};
static int rootfs_init(){
    int i;
    for (i = 0; i < NUMBER_OF_FS; i++){
        if ((NULL == fs_list[i].fsop) || (NULL == fs_list[i].fsop ->init)){
            TRACE("Error: during init fs fs with id has wrong operations desc\n");
            continue;
        }
        fs_list[i].fsop ->init();
    }

    return 0;
}

FILE_NAME_STRUCT *parse_file_name(const char *file_name, FILE_NAME_STRUCT *file_name_struct){
    int i;
    if ('/' != file_name[0]){
        return NULL;
    }
    file_name_struct->fs_name[0] = '/';
    for (i = 0; i < array_len(file_name_struct->fs_name); i ++){
        if ('/' == file_name[i+1]){
            file_name_struct->fs_name[i + 1] = 0;
            file_name_struct->file_name = (char *)&file_name[i + 1 + 1];
            return file_name_struct;
        }
        file_name_struct->fs_name[i + 1] = file_name[i+1];
    }
    return NULL;
}

FSOP_DESCRIPTION *rootfs_get_fsopdesc(char *fs_name){
    int i;
    printf("fs_name %10s\n", fs_name);
    if (0 == strncmp(fs_name, "/",FS_MAX_DISK_NAME_LENGTH)){
        printf("oO\n");
        return &rootfs_op;
    }
    for (i = 0; i < NUMBER_OF_FS; i++){
        if (0 == strncmp(fs_list[i].name, fs_name + 1, array_len(fs_list[i].name))){
            printf("%d", i);
            return (FSOP_DESCRIPTION *)fs_list[i].fsop;
        }
    }
    return NULL;
}

void *rootfs_fopen(const char *file_name, char *mode){
    FILE_NAME_STRUCT fname_struct;
    FSOP_DESCRIPTION *fsop;
    if (NULL == parse_file_name(file_name, &fname_struct)){
        TRACE("can't parse file name %s\n (may be wrong format)\n", file_name); //??
        return NULL;
    }
    TRACE("try open: disk %s\tfile %s\n", fname_struct.fs_name, fname_struct.file_name);
    if (NULL == (fsop = rootfs_get_fsopdesc((char*) /*file_name*/fname_struct.fs_name))){
        TRACE("can't find file system description for file %s\n (may be file %s didn't create)\n",
    									file_name, file_name);
        return NULL;
    }
    if (NULL == fsop->open_file){
        TRACE("can't find open_file function wrong fs operation descriptor for file %s\n", file_name);
        return NULL;
    }
    return fsop->open_file(fname_struct.file_name, mode);
}

