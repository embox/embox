#ifndef FSOP_H_
#define FSOP_H_

#include "file_new.h" // FLIST_ITEM

typedef BOOL (*FS_INIT_FUNC) (void);

typedef size_t (*FS_LISTFILES_FUNC) (FLIST_ITEM* out_flist, int max_nitems);
typedef short  (*FS_GETFILEIDX_FUNC) (const char *file_name);

typedef BOOL (*FILE_DELETE_FUNC) (const char *file_name);
typedef short (*FILE_CREATE_FUNC) (const char *file_name);

typedef size_t (*FILE_READ_FUNC) (short fidx, long s_offset, void *buf, size_t nbytes);
typedef size_t (*FILE_WRITE_FUNC) (short fidx, long s_offset, const void *buf, size_t nbytes);

typedef BOOL (*FS_FORMAT_FUNC) (void);
typedef BOOL (*FS_SYNC_FUNC) (void);

typedef int (*FS_GETCAPACITY_FUNC) (void);
typedef int (*FS_GETFREESPACE_FUNC) (void);


typedef struct {
	FS_INIT_FUNC init;

	FS_LISTFILES_FUNC list_files;
	FS_GETFILEIDX_FUNC get_file_idx;

	FILE_CREATE_FUNC create_file;
	FILE_DELETE_FUNC delete_file;

	FILE_READ_FUNC file_read;
	FILE_WRITE_FUNC file_write;

	FS_FORMAT_FUNC format;
	FS_SYNC_FUNC sync;

	FS_GETCAPACITY_FUNC get_capacity;
	FS_GETFREESPACE_FUNC get_freespace;
} FSOP;

#endif // FSOP_H_
