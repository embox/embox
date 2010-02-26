/*
 * @file
 *
 * @date 28.08.2009
 * @author Roman Evstifeev
 */
#include <common.h>
#include <fs/file.h>
#include <fs/rootfs.h>

global_file_id_t fopen (const char* file_path){
	return rootfs_fopen(file_path);
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
