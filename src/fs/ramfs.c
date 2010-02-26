/**
 * @file
 * @details realize file operation function in line address space
 *
 * @date 29.06.2009
 * @author Anton Bondarev
 */
#include <string.h>
#include <common.h>
#include <fs/rootfs.h>
#include <fs/ramfs.h>
#include <netutils.h>

typedef struct _FILE_DESC {
	unsigned int start_addr;
	unsigned int size;
	char name[MAX_LENCTH_FILE_NAME];
	unsigned int mode;
	unsigned int is_busy;
} FILE_DESC;

static int file_desc_cnt;
#define MAX_FILE_QUANTITY 0x10
static FILE_DESC fdesc[MAX_FILE_QUANTITY];

static void *fopen(const char *file_name, char *mode);
static int fclose(void * file);
static size_t fread(const void *buf, size_t size, size_t count, void *file);
static size_t fwrite(const void *buf, size_t size, size_t count, void *file);
static int fseek(void *file, long offset, int whence);

static FILEOP fop = { fopen, fclose, fread, fwrite, fseek };

static int create_file(void *params);

#define FILE_HANDLERS_QUANTITY 0x10

typedef struct _FILE_HANDLER {
	FILEOP *fileop;
	FILE_DESC *fdesc;
	int cur_pointer;
	unsigned int mode;
} FILE_HANDLER;
static FILE_HANDLER file_handlers[FILE_HANDLERS_QUANTITY];

//static int file_handler_cnt;

static int file_list_cnt;

static FILE_INFO * file_list_iterator(FILE_INFO *finfo) {
	if (MAX_FILE_QUANTITY <= file_list_cnt)
		return NULL;
	while (!fdesc[file_list_cnt].is_busy) {
		if (MAX_FILE_QUANTITY <= file_list_cnt)
			return NULL;
		file_list_cnt++;
	}
	strncpy(finfo->file_name, fdesc[file_list_cnt].name, array_len(finfo->file_name) );
	finfo->mode = fdesc[file_list_cnt].mode;
	finfo->size_in_bytes = fdesc[file_list_cnt].size;
	finfo->size_on_disk = 0;
	file_list_cnt++;
	return finfo;
}

static FS_FILE_ITERATOR get_file_list_iterator(void) {
	file_list_cnt = 0;
	return file_list_iterator;
}

static FILE_DESC * find_free_desc(void) {
	int i;
	if (MAX_FILE_QUANTITY <= file_desc_cnt)
		return NULL;

	for (i = 0; i < MAX_FILE_QUANTITY; i++) {
		if (0 == fdesc[i].is_busy) {
			return &fdesc[i];
		}
	}
	return NULL;
}

static FILE_DESC * find_file_desc(const char * file_name) {
	int i;

	for (i = 0; i < MAX_FILE_QUANTITY; i++) {
		if (0 == strncmp(fdesc[i].name, file_name, array_len(fdesc[i].name))) {
			return &fdesc[i];
		}
	}
	return NULL;
}

static FILE_HANDLER * find_free_handler(void) {
	int i;
	if (FILE_HANDLERS_QUANTITY <= file_desc_cnt)
		return NULL;

	for (i = 0; i < FILE_HANDLERS_QUANTITY; i++) {
		if (0 == file_handlers[i].fileop) {
			return &file_handlers[i];
		}
	}
	return NULL;
}

static int init(void) {
	extern char _data_start, _data_end, _text_start, _text_end,
					_piggy_end, _piggy_start;
	RAMFS_CREATE_PARAM param;

	//create file section_text
	strncpy(param.name, "section_text", array_len(param.name));
	param.size = (unsigned int) (&_text_end - &_text_start);
	param.start_addr = (unsigned int) (&_text_start);
	create_file(&param);//text
	//create file section_data
	strncpy(param.name, "section_data", array_len(param.name));
	param.size = (unsigned int) (&_data_end - &_data_start);
	param.start_addr = (unsigned int) (&_data_start);
	create_file(&param);//ram
//#ifdef INCLUDE_OS_IMAGE
	//create section piggy if need
	if (&_piggy_end - &_piggy_start) {
		strncpy(param.name, "piggy", array_len(param.name));
		param.size = (unsigned int )(&_piggy_end - &_piggy_start);
		param.start_addr = (unsigned int )(&_piggy_start);
		create_file(&param);// /ramfs/piggy
	}
//#endif //INCLUDE_OS_IMAGE
	return 0;
}

static void *open_file(const char *file_name, char *mode) {
	FILE_HANDLER *fh;
	FILE_DESC *fd;

	if (NULL == (fd = find_file_desc(file_name))){
		TRACE("can't find file %s\n", file_name);
		return NULL;
	}

	if (NULL == (fh = find_free_handler())){
		TRACE("can't find free handler\n");
		return NULL;
	}
	fh->cur_pointer = 0;
	fh->fdesc = fd;
	//TODO must check permitions
	fh->mode = (unsigned int) *mode;
	fh->fileop = &fop;
	printf ("fh = 0x%08X\tfop = 0x%08X\n", (unsigned)fh, (unsigned)&fop);
	printf ("fread = 0x%08X\n", (unsigned)fh->fileop->read);
	printf ("fwrite = 0x%08X\n", (unsigned)fh->fileop->write);
	printf ("start_addr = 0x%08X\t size = %d\n", fh->fdesc->start_addr, fh->fdesc->size);
	fh->fileop->fopen(file_name, mode);
	return fh;
}

static int create_file(void *params) {
	RAMFS_CREATE_PARAM *par = (RAMFS_CREATE_PARAM *) params;
	FILE_DESC *fd;

	if (NULL == (fd = find_free_desc())) {
		TRACE("can't find free descriptor\n");
		return -1;
	}

	strncpy(fd->name, par->name, array_len(fd->name));
	fd->start_addr = par->start_addr;
	fd->size = par->size;
	fd->mode = par->mode;
	fd->is_busy = 1;
	return 0;
}

static int resize_file(void *params) {
	return -1;
}

static int delete_file(const char * file_name) {
	FILE_DESC *fd;
	if (NULL == (fd = find_file_desc(file_name))) {
		TRACE("file %s not found\n", file_name);
		return -1;
	}
	fd->is_busy = 0;
	return 0;
}

static int get_capacity(const char * file_name) {
	return 0;
}
static int get_freespace(const char * file_name) {
	return 0;
}
static int get_descriptors_info(void *params) {
	return 0;
}

FSOP_DESCRIPTION ramfsop = { init, open_file, create_file, resize_file,
		delete_file, get_capacity, get_freespace, get_descriptors_info,
		get_file_list_iterator };

static void *fopen(const char *file_name, char *mode) {
	TRACE("ramfs file %s was opened\n", file_name);
	return NULL;
}

static int fclose(void * file) {
	FILE_HANDLER *fh = (FILE_HANDLER *) file;
	fh->fileop = NULL;

	return 0;
}

#define TRACE_FREQ 0x10000

static size_t fread(const void *buf, size_t size, size_t count, void *file) {
	FILE_HANDLER *fh = (FILE_HANDLER *) file;
	if (fh->cur_pointer >= fh->fdesc->size){
		TRACE("end read\n");
		return 0;
	}
	memcpy((void*)buf, (const void *)(fh->fdesc->start_addr + fh->cur_pointer), size * count);
	fh->cur_pointer += size * count;
	if (0 == (fh->cur_pointer & (TRACE_FREQ - 1))){
		TRACE("cur = 0x%08X\t size = %d\n",fh->cur_pointer,fh->fdesc->size);
	}
	return size * count;
}

static size_t fwrite(const void *buf, size_t size, size_t count, void *file) {
	FILE_HANDLER *fh = (FILE_HANDLER *) file;
	if (0==fh->cur_pointer){
			TRACE("start write\n");
		}
	memcpy((void *)(fh->fdesc->start_addr + fh->cur_pointer),buf, size * count);
	fh->cur_pointer += size * count;
	return size * count;
}

static int fseek(void *file, long offset, int whence) {
	//FILE_HANDLER *fh = (FILE_HANDLER *) file;
	return 0;
}
