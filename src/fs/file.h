#ifndef FILE_H_
#define FILE_H_

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


#endif /*FILE_H_*/
