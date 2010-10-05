/**
 * @file
 * @details realize file operation function in line address space
 *
 * @date 29.06.2009
 * @author Anton Bondarev
 */
#include <string.h>
#include <fs/ramfs.h>
#include <linux/init.h>

static int file_desc_cnt;

static FILE_DESC fdesc[CONFIG_MAX_FILE_QUANTITY];

static void *ramfs_fopen(const char *path, const char *mode);
static int ramfs_fclose(void * file);
static size_t ramfs_fread(void *buf, size_t size, size_t count, void *file);
static size_t ramfs_fwrite(const void *buf, size_t size, size_t count, void *file);
static int ramfs_fseek(void *file, long offset, int whence);

static FILEOP fop = {
	ramfs_fopen,
	ramfs_fclose,
	ramfs_fread,
	ramfs_fwrite,
	ramfs_fseek
};

static int create_file(void *params);

#define FILE_HANDLERS_QUANTITY 0x10

static FILE_HANDLER file_handlers[FILE_HANDLERS_QUANTITY];

//static int file_handler_cnt;

static int file_list_cnt;

static FILE_INFO * file_list_iterator(FILE_INFO *finfo) {
	if (CONFIG_MAX_FILE_QUANTITY <= file_list_cnt)
		return NULL;
	while (!fdesc[file_list_cnt].is_busy) {
		if (CONFIG_MAX_FILE_QUANTITY <= file_list_cnt)
			return NULL;
		file_list_cnt++;
	}
	strncpy(finfo->file_name, fdesc[file_list_cnt].name, array_len(finfo->file_name));
	finfo->mode = fdesc[file_list_cnt].mode;
	finfo->size_in_bytes = fdesc[file_list_cnt].size;
	finfo->size_on_disk = fdesc[file_list_cnt].size;
	file_list_cnt++;
	return finfo;
}

static FS_FILE_ITERATOR get_file_list_iterator(void) {
	file_list_cnt = 0;
	return file_list_iterator;
}

static FILE_DESC *find_free_desc(void) {
	size_t i;
	if (CONFIG_MAX_FILE_QUANTITY <= file_desc_cnt)
		return NULL;

	for (i = 0; i < CONFIG_MAX_FILE_QUANTITY; i++) {
		if (0 == fdesc[i].is_busy) {
			return &fdesc[i];
		}
	}
	return NULL;
}

static FILE_DESC * find_file_desc(const char * file_name) {
	size_t i;
	for (i = 0; i < CONFIG_MAX_FILE_QUANTITY; i++) {
		if (0 == strncmp(fdesc[i].name, file_name, array_len(fdesc[i].name))) {
			return &fdesc[i];
		}
	}
	return NULL;
}

static FILE_HANDLER * find_free_handler(void) {
	size_t i;
	if (FILE_HANDLERS_QUANTITY <= file_desc_cnt)
		return NULL;

	for (i = 0; i < FILE_HANDLERS_QUANTITY; i++) {
		if (0 == file_handlers[i].fileop) {
			return &file_handlers[i];
		}
	}
	return NULL;
}

static int __init init(void) {
	extern char _data_start, _data_end,
				_text_start, _text_end;
	RAMFS_CREATE_PARAM param;
	TRACE("Init RAMFS\n");

	/* create file /rams/section_text */
	strncpy(param.name, "section_text", array_len(param.name));
	param.size = (unsigned long) (&_text_end - &_text_start);
	param.start_addr = (unsigned long) (&_text_start);
	param.mode = FILE_MODE_RWX;
	create_file(&param);
	/* create file /ramfs/section_data */
	strncpy(param.name, "section_data", array_len(param.name));
	param.size = (unsigned long) (&_data_end - &_data_start);
	param.start_addr = (unsigned long) (&_data_start);
	param.mode = FILE_MODE_RWX;
	create_file(&param);
	return 0;
}

static void *open_file(const char *file_name, const char *mode) {
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
	//printf ("fh = 0x%08X\tfop = 0x%08X\n", (unsigned)fh, (unsigned)&fop);
	//printf ("fread = 0x%08X\n", (unsigned)fh->fileop->read);
	//printf ("fwrite = 0x%08X\n", (unsigned)fh->fileop->write);
	//printf ("start_addr = 0x%08X\t size = %d\n", fh->fdesc->start_addr, fh->fdesc->size);
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

FSOP_DESCRIPTION ramfsop = {
	init,
	open_file,
	create_file,
	resize_file,
	delete_file,
	get_capacity,
	get_freespace,
	get_descriptors_info,
	get_file_list_iterator
};

static void *ramfs_fopen(const char *file_name, const char *mode) {
	//TRACE("ramfs file %s was opened\n", file_name);
	return NULL;
}

static int ramfs_fclose(void * file) {
	FILE_HANDLER *fh = (FILE_HANDLER *) file;
	fh->fileop = NULL;

	return 0;
}

#define TRACE_FREQ 0x10000

static size_t ramfs_fread(void *buf, size_t size, size_t count, void *file) {
	FILE_HANDLER *fh = (FILE_HANDLER *) file;

	if (fh->cur_pointer >= fh->fdesc->size){
		//TRACE("end read\n");
		return 0;
	}

	memcpy((void*)buf, (const void *)(fh->fdesc->start_addr + fh->cur_pointer), size * count);
	fh->cur_pointer += size * count;
#if 0
	if (0 == (fh->cur_pointer & (TRACE_FREQ - 1))){
		TRACE("cur = 0x%08X\t size = %d\n",fh->cur_pointer,fh->fdesc->size);
	}
#endif
	return size * count;
}

static size_t ramfs_fwrite(const void *buf, size_t size, size_t count, void *file) {
	FILE_HANDLER *fh = (FILE_HANDLER *) file;
	FILE_DESC *fd = find_file_desc(fh->fdesc->name);
#if 0
	if (0 == fh->cur_pointer) {
		TRACE("start write\n");
	}
#endif
	memcpy((void *)(fh->fdesc->start_addr + fh->cur_pointer),buf, size * count);
	fh->cur_pointer += size * count;
	fd->size += size * count;
	return size * count;
}

static int ramfs_fseek(void *file, long offset, int whence) {
	FILE_HANDLER *fh = (FILE_HANDLER *) file;
	int new_offset = offset + whence;

	if (file == NULL) {
		return -2; /*Null file descriptor*/
	}

	if (new_offset >= fh->fdesc->size) {
		return -1; /*Non-valid offset*/
	}

	fh->cur_pointer = new_offset;

	return 0;
}
