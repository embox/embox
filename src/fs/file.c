/**
 * @file
 *
 * @date 06.08.09
 * @author Anton Bondarev
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <lib/list.h>
#include <fs/rootfs.h>
#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <util/array.h>
#include <err.h>

#include <fs/file_desc.h>

FILE *fopen(const char *path, const char *mode) {
	node_t *nod;
	file_system_driver_t *drv;
	file_operations_t *fop;
	FILE *file;

	nod = vfs_find_node(path, NULL);

	if (NULL == nod) {
		errno = -EINVAL;
		return NULL;
	}
	/* check permissions */

	if (NULL != nod->file_info) {
		/*if fop set*/
		fop = (file_operations_t *) nod->file_info;
		if (NULL == fop->fopen(path, mode)) {
			errno = -EINVAL;
			return NULL;
		}
		return (FILE *) nod;
	}

	drv = nod->fs_type;
	if (NULL == drv->file_op->fopen) {
		errno = -EINVAL;
		LOG_ERROR("fop->fopen is NULL handler\n");
		return NULL;
	}
	nod->unchar = EOF;
	file = drv->file_op->fopen(path, mode);

	return file;
}

size_t fwrite(const void *buf, size_t size, size_t count, FILE *file) {
	node_t *nod;
	file_system_driver_t *drv;
	nod = (node_t *) file;
	if (NULL == nod) {
		return -EBADF;
	}
	drv = nod->fs_type;
	if (NULL == drv->file_op->fwrite) {
		LOG_ERROR("fop->fwrite is NULL handler\n");
		return -EBADF;
	}
	return drv->file_op->fwrite(buf, size, count, file);
}

size_t fread(void *buf, size_t size, size_t count, FILE *file) {
	node_t *nod;
	file_system_driver_t *drv;
	nod = (node_t *) file;
	if (NULL == nod) {
		return -EBADF;
	}
	drv = nod->fs_type;
	if (NULL == drv->file_op->fread) {
		LOG_ERROR("fop->fread is NULL handler\n");
		return -EBADF;
	}
	return drv->file_op->fread(buf, size, count, file);
}


int fclose(FILE *fp) {
	node_t *nod;
	file_system_driver_t *drv;
	file_operations_t *fop;

	nod = (node_t *) fp;
	if (NULL == nod) {
		return -EBADF;
	}
	if (NULL != nod->file_info) {
		/*if fop set*/
		fop = (file_operations_t *) nod->file_info;
		return fop->fclose(fp);
	}
	drv = nod->fs_type;
	if (NULL == drv->file_op->fclose) {
		return -EOF;
	}
	return drv->file_op->fclose(fp);
}

int fseek(FILE *stream, long int offset, int origin) {
	node_t *nod;
	file_system_driver_t *drv;
	nod = (node_t *) stream;

	if (NULL == nod) {
		return -EBADF;
	}
	drv = nod->fs_type;
	if (NULL == drv->file_op->fseek) {
		LOG_ERROR("fop->fseek is NULL handler\n");
		return -EBADF;
	}

	return drv->file_op->fseek(stream, offset, origin);
}

int fioctl(FILE *fp, int request, ...) {
	node_t *nod;
	file_system_driver_t *drv;
	va_list args;
	va_start(args, request);

	nod = (node_t *) fp;
	if (NULL == nod) {
		return -EBADF;
	}
	drv = nod->fs_type;
	if (NULL == drv->file_op->ioctl) {
		return -EBADF;
	}
	return drv->file_op->ioctl(fp, request, args);
}

//TODO !!!!!!!!!!!!!!!!!!!!!!!! move from here
int remove(const char *pathname) {
	node_t *nod = vfs_find_node(pathname, NULL);
	file_system_driver_t *drv = nod->fs_type;
	if (NULL == drv->fsop->delete_file) {
		LOG_ERROR("fsop->delete_file is NULL handler\n");
		return -EBADF;
	}
	return drv->fsop->delete_file(pathname);
}

int fstat(const char *path, stat_t *buf) {
	//FIXME: workaround, ramfs depend.
	node_t *nod;
	ramfs_file_description_t *desc;
	nod = vfs_find_node(path, NULL);
	desc = (ramfs_file_description_t *) nod->attr;
	buf->st_size = desc->size;
	buf->st_mode = desc->mode;
	buf->st_mtime = desc->mtime;
	return 0;
}

int fgetc(FILE *file) {
	node_t *nod = (node_t *) file;
	char ch;
	if (NULL == nod) {
		return -EBADF;
	}
	if (nod->unchar != EOF) {
		ch = nod->unchar;
		nod->unchar = EOF;
		return ch;
	}
	fread(&ch, 1, 1, file);
	return ch;
}

int ungetc(int ch, FILE *file) {
	node_t *nod = (node_t *) file;
	nod->unchar = (char) ch;
	return ch;
}

