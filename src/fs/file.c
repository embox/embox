/*
 * @file
 *
 * @date 06.08.2009
 * @author Anton Bondarev
 */
#include <string.h>
#include <common.h>
#include <fs/file.h>
#include <fs/rootfs.h>

void *fopen (const char *file_name, char *mode){
	return rootfs_fopen(file_name, mode);
}

size_t fwrite (const void *buf, size_t size, size_t count, void *file) {
	FILEOP **fop = (FILEOP **)file;
	if (NULL == fop){
		TRACE("Error during write file: fop is NULL wrong file handler\n");
		return -1;
	}
	if (NULL == (*fop)->write){
		TRACE("Error during write file: fop->read is NULL wrong file handler\n");
		return -1;
	}
	return (*fop)->write(buf, size, count, file);
}

size_t fread (const void *buf, size_t size, size_t count, void *file) {
	FILEOP **fop = (FILEOP **)file;
	if (NULL == fop){
		TRACE("Error during read file: fop is NULL wrong file handler\n");
		return -1;
	}
	if (NULL == (*fop)->read){
		TRACE("Error during read file: fop->read is NULL wrong file handler\n");
		return -1;
	}
	return (*fop)->read(buf, size, count, file);
}

void fclose (void *file) {
	FILEOP *fop = (FILEOP *)file;
	if (NULL == fop)
		return ;
	if (NULL == fop->close){
		return ;
	}
	fop->close(file);
}
