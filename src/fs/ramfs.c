/**
 * \file ramfs.c
 * \date Jun 29, 2009
 * \author anton
 * \details
 */
#include "types.h"
#include "common.h"
#include "file.h"
#include "rootfs.h"
#include "ramfs.h"



#define MAX_LENCTH_FILE_NAME 0x10
typedef struct _FILE_DESC {
    unsigned int start_addr;
    unsigned int size;
    char name[MAX_LENCTH_FILE_NAME];
    unsigned int mode;
    unsigned int is_busy;
}FILE_DESC;

static int file_desc_cnt;
#define RAMFILE_DESC_QUANTITY 0x10
static FILE_DESC fdesc[RAMFILE_DESC_QUANTITY];

static void *fopen (const char *file_name, char *mode);
static int fclose (void * file);
static size_t fread (const void *buf, size_t size, size_t count, void *file);
static size_t fwrite (const void *buf, size_t size, size_t count, void *file);
static int fseek (void *file, long offset, int whence);

static FILEOP fop = {fopen,
        fclose,
        fread,
        fwrite,
        fseek
};

#define FILE_HANDLERS_QUANTITY 0x10
typedef struct _FILE_HANDLER {
    FILEOP *fileop;
    FILE_DESC *fdesc;
    int cur_pointer;
    unsigned int mode;
}FILE_HANDLER;
static FILE_HANDLE file_handlers [FILE_HANDLERS_QUANTITY];

//static int file_handler_cnt;


static FILE_DESC * find_free_desc(){
    int i;
//    if (RAMFILE_DESC_QUANTITY >= file_desc_cnt)
//        return -1;

    for (i = 0; i < RAMFILE_DESC_QUANTITY; i ++){
        if (0 == fdesc[i].is_busy){
            return &fdesc[i];
        }
    }
    return NULL;
}

static FILE_DESC * find_file_desc(const char * file_name){
    int i;

    for (i = 0; i < RAMFILE_DESC_QUANTITY; i ++){
        if (0 == strncmp(fdesc[i].file_name, file_name, array_len(fdesc[i].file_name))){
            return &fdesc[i];
        }
    }
    return NULL;
}

static FILE_HANDLER * find_free_handler(){
    int i;
//    if (RAMFILE_DESC_QUANTITY >= file_desc_cnt)
//        return -1;

    for (i = 0; i < FILE_HANDLERS_QUANTITY; i ++){
        if (0 == file_handlers[i].fileop){
            return i;
        }
    }
    return -1;
}

static int init(){
    extern char _data_start, _data_end; _text_start, _endtext;
    RAMFS_CREATE_PARAM param;

    //create file section_text
    strcpy(param.name, "section_text");
    param.size = (unsigned int )(&_endtext - &_text_start);
    param.start_addr = (unsigned int )(&_text_start);
    create_file(&param);//text
    //create file section_data
    strcpy(param.name, "section_data");
    param.size = (unsigned int )(&_data_end - &_data_start);
    param.start_addr = (unsigned int )(&_data_start);
    create_file(&param);//ram

    return 0;
}

static void *open_file(const char *file_name, char *mode){
    FILE_HANDLER *fh;
    FILE_DESC *fd;

    if (NULL == (fd = find_file_desc(file_name)))
        return NULL;

    if (NULL == (fh = find_free_handler()))
        return NULL;

    fh->cur_pointer = 0;
    fh->fdesc = fd;
    //TODO must check permitions
    fh->mode = (unsigned int)*mode;
    fh->fileop = &fop;
    fh->fileop->fopen(file_name, mode);
    return fh;
}


static int create_file(void *params){
    RAMFS_CREATE_PARAM *par = (RAMFS_CREATE_PARAM *)params;
    FILE_DESC *fd;

    if (NULL == (fd = find_free_desc())){
        TRACE("can't find free descriptor\n");
        return -1;
    }

    strncpy (fd->name, par->name, array_len(fd->name);
    fd->start_addr = par->start_addr;
    fd->size = par->size;
    fd->mode = par->mode;
    return 0;
}

static int resize_file(void *params){
    return -1;
}

static int delete_file(const char * file_name){
    FILE_DESC *fd;
    if (fd == find_file_desc(file_name)){
        TRACE("file %s not found\n", file_name);
        return -1;
    }
    fd->is_busy = 0;
    return 0;
}

static int get_capacity(const char * file_name){
    return 0;
}
static int get_freespace(const char * file_name){
    return 0;
}
static int get_descriptors_info(void *params){
    return 0;
}

FSOP_DESCRIPTION ramfs_fsop = {init,
    open_file,
    create_file,
    resize_file,
    delete_file,
    get_capacity,
    get_freespace,
    get_descriptors_info
};

static void *fopen (const char *file_name, char *mode){
    TRACE("file %s was opened\n", file_name);
    return NULL;
}

static int fclose (void * file){
    FILE_HANDLER *fh = (FILE_HANDLE *)file;
    fh->fileop = NULL;

    return 0;
}

static size_t fread (const void *buf, size_t size, size_t count, void *file){
    FILE_HANDLER *fh = (FILE_HANDLE *)file;
    return 0;
}

static size_t fwrite (const void *buf, size_t size, size_t count, void *file){
    FILE_HANDLER *fh = (FILE_HANDLE *)file;
    return 0;
}

static int fseek (void *file, long offset, int whence){
    FILE_HANDLER *fh = (FILE_HANDLE *)file;
    return 0;
}
