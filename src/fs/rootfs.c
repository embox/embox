/**
 * \file rootfs.c
 * \date Jun 29, 2009
 * \author Anton Bondarev
 * \details
 */
#include "types.h"
#include "common.h"
#include "rootfs.h"

#include "ramfs.h"



static FS_DESCRIPTION fs_list = {
#include "rootfs_desc.inc"
};
#define NUMBER_OF_FS    array_len(fs_list)


static file_list_cnt;
static FILE_INFO * file_list_iterator(FILE_INFO *finfo){
    if (NUMBER_OF_FS  >= file_list_cnt)
        return NULL;
    strncpy (finfo->file_name, fs_list[file_list_cnt].name, array_len(finfo->file_name));
    finfo->mode = FILE_MODE_RO;
    finfo->size_in_bytes = 0;
    finfo->size_on_disk = 0;
    file_list_cnt ++;
    return finfo;
}

static FS_FILE_ITERATOR get_file_list_iterator(){
    file_list_cnt = 0;
    return file_list_iterator;
}

static FSOP_DESCRIPTION rootfs_op;
int rootfs_init(){
    rootfs_op.get_file_list_iterator = get_file_list_iterator;
    return 0;
}

//for parsing filename
typedef struct _FILE_NAME_STRUCT{
    char fs_name[FS_MAX_DISK_NAME_LENGTH];//fs name (flash ramdisc and so on)
    char *file_name; //
}FILE_NAME_STRUCT;

static FILE_NAME_STRUCT *parse_file_name(const char *file_name, FILE_NAME_STRUCT *file_name_struct){
    int i;
    if ('/' != file_name[0]){
        return NULL;
    }

    for (i = 0; i < FS_MAX_DISK_NAME_LENGTH; i ++){
        if ('/' == file_name[i+1]){
            file_name_struct->fs_name[i] = 0;
            file_name_struct->file_name = file_name[i+1];
            return file_name_struct;
        }
        file_name_struct->fs_name[i] = file_name[i+1];
    }
    return NULL;
}

FSOP_DESCRIPTION *rootfs_get_fsopdesc(char *fs_name){
    int i;
    if (0 == strncmp(fs_name, "/",FS_MAX_DISK_NAME_LENGTH)){
        return &rootfs_op;
    }
    for (int i = 0; i < NUMBER_OF_FS; i++){
        if (0 == strncmp(fs_list[i].name, fs_name, array_len(fs_name))){
            return fs_list[i].fsop;
        }
    }
    return NULL;
}

void *rootfs_fopen(const char *file_name, char *mode){
    FILE_NAME_STRUCT fname_struct;
    FSOP_DESCRIPTION *fsop;
    if (NULL == parse_file_name(file_name, &fs_desc)){
        TRACE("can't parse file name %s\n (may be wrong format)\n", file_name);
        return NULL;
    }
    if (NULL == (fsop = rootfs_get_fsopdesc(fname_struct.fs_name))){
        TRACE("can't find file system description for file %s\n (may be file ", file_name);
        return NULL;
    }
    if (NULL == fsop->fopen_func){
        TRACE("can't find alloc fdesc function wrong fs operation descriptor for file %s\n", file_name);
        return NULL;
    }
    return fsop->alloc_desc(file_name, mode);
}

