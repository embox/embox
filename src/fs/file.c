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
#include <fs/file.h>

#include <fs/file_desc.h>
#include <kernel/prom_printf.h>

#define LAST  0x01

int nip_tail (char *head, char *tail) {
	char *p_tail;
	char *p;

	p = p_tail = head + strlen(head);
	strcat(head, tail);

	do {
		p_tail--;
		if(head >= p_tail) {
			*p = '\0';
			return -1;
		}
	} while ('/' != *p_tail);

	strcpy (tail, p_tail);
	*p_tail = '\0';

	return 0;
}

int increase_tail (char *head, char *tail) {
	char *p_tail;

		p_tail = head + strlen(head);
		strcat(head, tail);

		do {
			if('\0' == *p_tail) {
				break;
			}
			p_tail++;
		} while ('/' != *p_tail);

		strcpy (tail, p_tail);
		*p_tail = '\0';

		return 0;
}

int create_filechain (const char *path){
	int count_dir;
	file_create_param_t param;
	fs_drv_t *drv;
	node_t *node, *new_node;
	char tail[CONFIG_MAX_LENGTH_FILE_NAME];

	count_dir = 0;
	tail[0] = '\0';
	strcpy (param.path, path);

	do {
		if (nip_tail (param.path, tail)) {
			errno = -EINVAL;
			return -1;
		}
		count_dir ++;
	} while (NULL == (node = vfs_find_node(param.path, NULL)));


	do {
		increase_tail (param.path, tail);

		if (NULL == (new_node = vfs_add_path (param.path, NULL))) {
			errno = -EINVAL;
			return -1;
		}

		drv = new_node->fs_type = node->fs_type;
		new_node->file_info = node->file_info;
		new_node->properties  = IS_DIRECTORY;
		/* believe that the latter in path is always a file */
		if (LAST == count_dir) {
			new_node->properties &= ~IS_DIRECTORY;
		}

		param.node = (void *) new_node;
		param.parents_node = (void *) node;

		if (NULL == drv->fsop->create_file) {
			errno = -EINVAL;
			LOG_ERROR("fsop->create_file is NULL handler\n");
			return -1;
		}

		drv->fsop->create_file ((void *)&param);

		node = new_node;

	} while (count_dir --);

	return 0;
}

FILE *fopen(const char *path, const char *mode) {
	node_t *nod;
	fs_drv_t *drv;
	FILE *file;
	struct file_desc *desc;

	if (NULL == (nod = vfs_find_node(path, NULL))) {
		if ('w' != *mode) {
			errno = -EINVAL;
			return NULL;
		}

		if (create_filechain(path)) {
			errno = -EINVAL;
			return NULL;
		}

	}
	/* check permissions */

	/* allocate new descriptor */
	if (NULL == (desc = file_desc_alloc())) {
		errno = -EINVAL;
		return NULL;
	}

	desc->cursor = 0;
	desc->node = nod;

	drv = nod->fs_type;
	if(NULL != nod->file_info) {
		desc->ops = (struct file_operations *)nod->file_info;
	} else {
		desc->ops = (struct file_operations *) drv->file_op;
	}

	if (NULL == drv->file_op->fopen) {
		errno = -EINVAL;
		LOG_ERROR("fop->fopen is NULL handler\n");
		return NULL;
	}
	file = drv->file_op->fopen(desc, mode);

	return file;
}

FILE *freopen(const char *path, const char *mode, FILE *file) {
	//TODO:
	return NULL;
}

int feof(FILE *file) {
	//TODO:
	return 0;
}

int ferror(FILE *file) {
	//TODO:
	return 0;
}

size_t fwrite(const void *buf, size_t size, size_t count, FILE *file) {
	struct file_desc *desc;

	if(NULL == file) {
		return -EBADF;
	}

	desc = (struct file_desc *)file;

	return desc->ops->fwrite(buf, size, count, file);
}

size_t fread(void *buf, size_t size, size_t count, FILE *file) {
	struct file_desc *desc;

	if(NULL == file) {
		return -EBADF;
	}

	desc = (struct file_desc *)file;

	return desc->ops->fread(buf, size, count, file);
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
