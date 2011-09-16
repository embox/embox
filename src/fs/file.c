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
#include <kernel/prom_printf.h>
FILE *fopen(const char *path, const char *mode) {
	node_t *nod;
	fs_drv_t *drv;
	FILE *file;
	struct file_desc *desc;

	prom_printf("openning %s\n", path);
	if (NULL == (nod = vfs_find_node(path, NULL))) {
		errno = -EINVAL;
		return NULL;
	}
	/* check permissions */

	/* allocate new descriptor */
	if (NULL == (desc = file_desc_alloc())) {
		errno = -EINVAL;
		return NULL;
	}

	desc->cursor = 0;
	desc->node = nod;
#if 0
	if (NULL != nod->file_info) {
		/*if fop set devfs for example*/
		fop = (file_operations_t *) nod->file_info;

		desc->ops = fop;

		if (NULL == fop->fopen(desc)) {
			errno = -EINVAL;
			return NULL;
		}
		//return (FILE *) nod;
		return (FILE *)desc;
	}
#endif
	drv = nod->fs_type;

	desc->ops = (struct file_operations *) drv->file_op;

	if (NULL == drv->file_op->fopen) {
		errno = -EINVAL;
		LOG_ERROR("fop->fopen is NULL handler\n");
		return NULL;
	}
	file = drv->file_op->fopen(desc);

	return file;
}

size_t fwrite(const void *buf, size_t size, size_t count, FILE *file) {
	//node_t *nod;
	//fs_drv_t *drv;
	struct file_desc *desc;

	if(NULL == file) {
		return -EBADF;
	}

	desc = (struct file_desc *)file;

	return desc->ops->fwrite(buf, size, count, file);
//	drv = desc->nod->fs_type;
//	if (NULL == drv->file_op->fwrite) {
//		LOG_ERROR("fop->fwrite is NULL handler\n");
//		return -EBADF;
//	}
//	return drv->file_op->fwrite(buf, size, count, file);
}

size_t fread(void *buf, size_t size, size_t count, FILE *file) {
//	node_t *nod;
//	fs_drv_t *drv;
	struct file_desc *desc;

	if(NULL == file) {
		return -EBADF;
	}

	desc = (struct file_desc *)file;

	return desc->ops->fread(buf, size, count, file);

//	drv = nod->fs_type;
//	if (NULL == drv->file_op->fread) {
//		LOG_ERROR("fop->fread is NULL handler\n");
//		return -EBADF;
//	}
//	return drv->file_op->fread(buf, size, count, file);
}


int fclose(FILE *file) {
	struct file_desc *desc;

	if(NULL == file) {
		return -EBADF;
	}

	desc = (struct file_desc *)file;
	desc->ops->fclose(desc);;
	file_desc_free(desc);

	return 0;


//	if (NULL != nod->file_info) {
//		/*if fop set*/
//		fop = (file_operations_t *) nod->file_info;
//		return fop->fclose(file);
//	}
//	drv = nod->fs_type;
//	if (NULL == drv->file_op->fclose) {
//		return -EOF;
//	}
//	return drv->file_op->fclose(file);
}

int fseek(FILE *stream, long int offset, int origin) {
	node_t *nod;
	fs_drv_t *drv;
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
	struct file_desc *desc = (struct file_desc *) fp;
	va_list args;
	va_start(args, request);

	if (NULL == fp) {
		return -EBADF;
	}

	if (NULL == desc->ops->ioctl) {
		return -EBADF;
	}
	return desc->ops->ioctl(fp, request, args);
}

//TODO !!!!!!!!!!!!!!!!!!!!!!!! move from here
int remove(const char *pathname) {
	node_t *nod = vfs_find_node(pathname, NULL);
	fs_drv_t *drv = nod->fs_type;
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
	struct file_desc *desc = (struct file_desc *) file;
	char ch;
	if (NULL == desc) {
		return -EBADF;
	}
	if (desc->has_ungetc) {
		desc->has_ungetc = 0;
		return desc->ungetc;
	}
	if (fread(&ch, 1, 1, file) == 0) {
		ch = EOF;
	}
	return ch;
}

int ungetc(int ch, FILE *file) {
	struct file_desc *desc = (struct file_desc *) file;
	desc->ungetc = (char) ch;
	desc->has_ungetc = 1;
	return ch;
}

