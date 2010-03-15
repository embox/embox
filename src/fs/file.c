/*
 * @file
 *
 * @date 06.08.2009
 * @author Anton Bondarev
 */
#include <string.h>
#include <common.h>
#include <stdio.h>
#include <fs/rootfs.h>

FILE *fopen(const char *path, const char *mode) {
	return rootfs_fopen(path, mode);
}

size_t fwrite (const void *buf, size_t size, size_t count, FILE *file) {
	FILEOP **fop = (FILEOP **)file;
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

size_t fread (void *buf, size_t size, size_t count, FILE *file) {
	FILEOP **fop = (FILEOP **)file;
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

int fclose (FILE *fp) {
	FILEOP *fop = (FILEOP *)fp;
	if (NULL == fop)
		return EOF;
	if (NULL == fop->close){
		return EOF;
	}
	return fop->close(fp);
}
