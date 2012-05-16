/**
 * @file
 *
 * @brief
 *
 * @date 16.09.2011
 * @author Anton Bondarev
 */

#include <fs/fs_drv.h>
#include <unistd.h>
#include <fcntl.h>
#include <kernel/file.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <lib/list.h>
#include <sys/types.h>
//#include <sys/stat.h>
//#include <fs/rootfs.h>
#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <fs/file_desc.h>
#include <util/array.h>

#include <kernel/prom_printf.h>

node_t *create_filechain(const char *path, uint8_t node_type){
	int count_dir;
	file_create_param_t param;
	fs_drv_t *drv;
	node_t *node, *new_node;
	char tail[CONFIG_MAX_LENGTH_FILE_NAME];

	count_dir = 0;
	tail[0] = '\0';
	strcpy (param.path, path);

	/* find last node in the path */
	do {
		if (nip_tail (param.path, tail)) {
			return NULL;
		}
		count_dir ++;
	} while (NULL == (node = vfs_find_node(param.path, NULL)));
	/* check drv of parents */
	drv = node->fs_type;
	if (NULL == drv->fsop->create_file) {
		LOG_ERROR("fsop->create_file is NULL handler\n");
		return NULL;
	}

	/* add one directory and assign the parameters of the parent */
	do {
		increase_tail (param.path, tail);

		if (NULL == (new_node = vfs_add_path (param.path, NULL))) {
			return NULL;
		}

		new_node->properties = DIRECTORY_NODE_TYPE;
		if ((LAST_IN_PATH == count_dir) && (FILE_NODE_TYPE == node_type)) {
			new_node->properties &= ~DIRECTORY_NODE_TYPE;
		}

		param.node = (void *) new_node;
		param.parents_node = (void *) node;
		drv->fsop->create_file ((void *)&param);

		node = new_node;
		count_dir--;

	} while (0 < count_dir);

	return node;
}

int creat(const char *pathname, mode_t mode) {
	node_t *nod;

	if (NULL != (nod = vfs_find_node(pathname, NULL))) {
		errno = -EBUSY;
		return -1;
	}

	/* set permission */


	if (NULL == (nod = create_filechain(pathname, FILE_NODE_TYPE))) {
		errno = -EINVAL;
		return -1;
	}

	return 0;
}

int mkdir(const char *pathname, mode_t mode) {
	node_t *nod;

	if (NULL != (nod = vfs_find_node(pathname, NULL))) {
		errno = -EBUSY;
		return -1;
	}

	/* set permission */


	if (NULL == (nod = create_filechain(pathname, DIRECTORY_NODE_TYPE))) {
		errno = -EINVAL;
		return -1;
	}

	return 0;
}

int remove(const char *pathname) {
	node_t *node;
	fs_drv_t *drv;

	if (NULL == (node = vfs_find_node(pathname, NULL))) {
		errno = -ENOENT;
		return -1;
	}

	drv = node->fs_type;
	if (NULL == drv->fsop->delete_file) {
		errno = -EINVAL;
		LOG_ERROR("fsop->delete_file is NULL handler\n");
		return -1;
	}

	if (DIRECTORY_NODE_TYPE == (node->properties & DIRECTORY_NODE_TYPE)) {
		return rmdir(pathname);
	}
	else {
		return unlink(pathname);
	}
}

/* TODO */
int unlink(const char *pathname) {
	node_t *node;
	fs_drv_t *drv;

	node = vfs_find_node(pathname, NULL);
	drv = node->fs_type;

	return drv->fsop->delete_file (pathname);
}

int rmdir(const char *pathname) {
	node_t *node;
	fs_drv_t *drv;

	node = vfs_find_node(pathname, NULL);
	drv = node->fs_type;

	return drv->fsop->delete_file (pathname);
}

int fstat(const char *path, stat_t *buf) {
	/*node_t *nod;
	fs_drv_t *drv;
	FILE *file;
	struct file_desc *desc;*/
	return 0;
}

