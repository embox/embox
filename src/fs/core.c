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
#include <err.h>
#include <fcntl.h>
#include <lib/list.h>
#include <fs/rootfs.h>
#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <fs/file_desc.h>

struct file_desc *kopen(const char *path, const char *mode) {
	node_t *nod;
	fs_drv_t *drv;
	struct file_desc *desc;

	if (NULL == (nod = vfs_find_node(path, NULL))) {
		if (strchr(mode, 'w') == NULL) {
			errno = ENOENT;
			return NULL;
		}

		if (create(path, 0) < 0) {
			return NULL;
		}

		if (NULL == (nod = vfs_find_node(path, NULL))) {
			return NULL;
		}
	}

	if (DIRECTORY_NODE_TYPE == (nod->properties & DIRECTORY_NODE_TYPE)) {
		errno = EISDIR;
		return NULL;
	}

	/* check permissions */

	/* allocate new descriptor */
	if (NULL == (desc = file_desc_alloc())) {
		errno = ENOMEM;
		return NULL;
	}

	desc->cursor = 0;
	desc->node = nod;

	drv = nod->fs_type;
	assert(drv != NULL);

	if (NULL != nod->file_info) {
		desc->ops = (struct file_operations *)nod->file_info;
	} else {
		desc->ops = (struct file_operations *)drv->file_op;
	}

	if (NULL == desc->ops->fopen) {
			errno = EBADF;
			LOG_ERROR("fop->fopen is NULL handler\n");
			return NULL;
		}
	return desc->ops->fopen(desc, mode);

}

size_t kwrite(const void *buf, size_t size, size_t count, struct file_desc *file) {

	if (NULL == file) {
		errno = EBADF;
		return -1;
	}


	if (NULL == file->ops->fwrite) {
		errno = EBADF;
		LOG_ERROR("fop->fwrite is NULL handler\n");
		return -1;
	}

	return file->ops->fwrite(buf, size, count, file);
}

size_t kread(void *buf, size_t size, size_t count, struct file_desc *desc) {

	if (NULL == desc) {
		errno = EBADF;
		return -1;
	}

	if (NULL == desc->ops->fread) {
		errno = EBADF;
		LOG_ERROR("fop->fread is NULL handler\n");
		return -1;
	}

	return desc->ops->fread(buf, size, count, desc);
}


int kclose(struct file_desc *desc) {

	if (NULL == desc) {
		errno = EBADF;
		return -1;
	}

	if (NULL == desc->ops->fclose) {
		errno = EBADF;
		LOG_ERROR("fop->fclose is NULL handler\n");
		return -1;
	}

	desc->ops->fclose(desc);
	file_desc_free(desc);

	return 0;
}

int kseek(struct file_desc *desc, long int offset, int origin) {

	if (NULL == desc) {
		errno = EBADF;
		return -1;
	}

	if (NULL == desc->ops->fseek) {
		errno = EBADF;
		LOG_ERROR("fop->fseec is NULL handler\n");
		return -1;
	}

	return desc->ops->fseek(desc, offset, origin);
}

int kstat(struct file_desc *desc, void *buff) {
	if (NULL == desc) {
		errno = EBADF;
		return -1;
	}

	if (NULL == desc->ops->fstat) {
		errno = EBADF;
		LOG_ERROR("fop->fstat is NULL handler\n");
		return -1;
	}

	return desc->ops->fstat(desc, buff);
}

int kioctl(struct file_desc *fp, int request, ...) {
	struct file_desc *desc = (struct file_desc *) fp;
	va_list args;
	va_start(args, request);

	if (NULL == fp) {
		errno = EBADF;
		return -1;
	}

	if (NULL == desc->ops->ioctl) {
		errno = EBADF;
		LOG_ERROR("fop->ioctl is NULL handler\n");
		return -1;
	}
	return desc->ops->ioctl(fp, request, args);
}
