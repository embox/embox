/**
 * @file
 *
 * @date 06.08.2009
 * @author Anton Bondarev
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fs/rootfs.h>
#include <fs/ramfs.h>

//FIXME: Actually, whole FS is unfinished now.
unsigned long file_base_addr;
//unsigned int file_base_addr = 0x40000000; /* for piggy */
//unsigned int file_base_addr = 0x40004000; /* for prom boot */
//unsigned int file_base_addr = 0x40003FC0; /* for u-boot */

FILE *fopen(const char *path, const char *mode) {
	RAMFS_CREATE_PARAM param;
	fsop_desc_t *fsop;
	FILE *fd;
	if (NULL == (fsop = rootfs_get_fsopdesc("/ramfs"))) {
		LOG_ERROR("Can't find /ramfs disk\n");
		return NULL;
	}
	if((fd = rootfs_fopen(path, mode)) == NULL) {
		param.size = 0x1000000;
		param.mode = FILE_MODE_RWX;
		param.start_addr = (unsigned long) (file_base_addr);
		sprintf(param.name, basename(path));
		if (-1 == fsop->create_file(&param)) {
			LOG_ERROR("Can't create %s\n", path);
			return NULL;
		}
		fd = rootfs_fopen(path, mode);
	}
	return fd;
}

FILE *fdopen(int fd, const char *mode) {
	return NULL;
}

size_t fwrite(const void *buf, size_t size, size_t count, FILE *file) {
	file_op_t **fop = (file_op_t **)file;
	if (NULL == fop){
		LOG_ERROR("fop is NULL handler\n");
		return -1;
	}
	if (NULL == (*fop)->write){
		LOG_ERROR("fop->write is NULL handler\n");
		return -1;
	}
	return (*fop)->write(buf, size, count, file);
}

size_t fread(void *buf, size_t size, size_t count, FILE *file) {
	file_op_t **fop = (file_op_t **)file;
	if (NULL == fop){
		LOG_ERROR("fop is NULL handler\n");
		return -1;
	}
	if (NULL == (*fop)->read){
		LOG_ERROR("fop->read is NULL handler\n");
		return -1;
	}
	return (*fop)->read(buf, size, count, file);
}

int fclose(FILE *fp) {
	file_op_t **fop = (file_op_t **)fp;
	if (NULL == fop)
		return EOF;
	if (NULL == (*fop)->close){
		return EOF;
	}
	return (*fop)->close(fp);
}

int remove(const char *pathname) {
	RAMFS_CREATE_PARAM param;
	fsop_desc_t *fsop;
	if (NULL == (fsop = rootfs_get_fsopdesc("/ramfs/"))) {
		LOG_ERROR("Can't find ramfs disk\n");
		return -1;
	}
	sprintf(param.name, basename(pathname));
	if (-1 == fsop->delete_file(param.name)) {
		LOG_ERROR("Can't delete ramfs disk\n");
		return -1;
	}
	return 0;
}

int fseek(FILE * stream, long int offset, int origin) {
	file_op_t **fop = (file_op_t **)stream;

	if (NULL == fop) {
		LOG_ERROR("fop is NULL handler\n");
		return -1;
	}

	if (NULL == (*fop)->fseek){
		LOG_ERROR("fop->fseek is NULL handler\n");
		return -1;
	}

	return (*fop)->fseek(stream, offset, origin);
}
