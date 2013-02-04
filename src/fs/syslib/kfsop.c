/*
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <fcntl.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>

#include <fs/vfs.h>
#include <fs/path.h>
#include <fs/fs_drv.h>
#include <fs/kfsop.h>

static int create_new_node(struct node * parent, char *node_name, uint8_t node_type) {
	struct node *new_node;
	struct nas *nas;
	fs_drv_t *drv;

	if( NULL == (new_node = vfs_add_path(node_name, parent))) {
		return -1;
	}
	new_node->type = node_type;
	/* check drv of parents */
	nas = parent->nas;
	drv = nas->fs->drv;
	if ((NULL == drv) || (NULL == drv->fsop->create_node)) {
		return -1;
	}

	if(0 > drv->fsop->create_node(parent, new_node)) {
		vfs_del_leaf(new_node);
		return -1;
	}
	return 0;
}

int kcreat(struct node *root_node, const char *pathname, mode_t mode) {
	char tpath[MAX_LENGTH_PATH_NAME];
	char path[MAX_LENGTH_PATH_NAME];
	int path_offset;
	char node_name[MAX_LENGTH_FILE_NAME];
	struct node *node;

	/* if node already exist return error */
	if (NULL != (node = vfs_find_node(pathname, root_node))) {
		errno = EBUSY;
		return -1;
	}

	if(NULL != root_node) {
		vfs_get_path_by_node(root_node, path);
		strncat(path, "/", sizeof(path));
	} else {
		path[0] = '\0';
	}

	strncat(path, pathname, sizeof(path));

	/* get last exist node */
	node = vfs_get_exist_path(path, tpath, sizeof(tpath));
	if(NULL == node) {
		return -1;
	}

	path_offset = strlen(tpath);

	do {
		path_get_next_name(&path[path_offset], node_name, sizeof(node_name));
		if((path_offset + strlen(node_name) + 1) >= strlen(path)) {
			if(0 == strlen(node_name)) {
				path_get_next_name(&path[path_offset], node_name, sizeof(node_name));
			}
			/* this is a file */
			return create_new_node(node, node_name, NODE_TYPE_FILE);
		} else {
			if(-1 == kmkdir(node, node_name, mode)) {
				return -1;
			}

			node = vfs_get_child(node_name, node);
			path_offset += (strlen(node_name) + 1);
		}
	} while (NULL != node);


	/* set permission */

	return 0;
}

int kmkdir(struct node *root_node, const char *pathname, mode_t mode) {
	int rc;
	char tpath[MAX_LENGTH_PATH_NAME];
	char path[MAX_LENGTH_PATH_NAME];
	int path_offset;
	char node_name[MAX_LENGTH_FILE_NAME];
	struct node *node;

	if (NULL != (node = vfs_find_node(pathname, root_node))) {
		errno = EBUSY;
		return -1;
	}

	if (NULL != root_node) {
		vfs_get_path_by_node(root_node, path);
		strncat(path, "/", sizeof(path));
	} else {
		path[0] = '\0';
	}


	strncat(path, pathname, sizeof(path));

	/* get last exist node */
	node = vfs_get_exist_path(path, tpath, sizeof(tpath));
	if(NULL == node) {
		return -1;
	}

	path_offset = strlen(tpath);

	do {
		path_get_next_name(&path[path_offset], node_name, sizeof(tpath));
		if(0 == strlen(node_name)) {
			return 0; /* we create all directory */
		}
		if(0 != (rc = create_new_node(node, node_name, NODE_TYPE_DIRECTORY))) {
			return -rc;
		}
		path_offset += (strlen(node_name) + 1);

		node = vfs_get_child(node_name, node);
	} while (NULL != node);

	return 0;
}

int kremove(const char *pathname) {
	node_t *node;
	struct nas *nas;
	fs_drv_t *drv;

	if (NULL == (node = vfs_find_node(pathname, NULL))) {
		errno = ENOENT;
		return -1;
	}

	nas = node->nas;
	drv = nas->fs->drv;
	if (NULL == drv->fsop->delete_node) {
		errno = EPERM;
		return -1;
	}

	if (node_is_directory(node)) {
		return rmdir(pathname);
	}
	else {
		return unlink(pathname);
	}
}

int kunlink(const char *pathname) {
	node_t *node;
	fs_drv_t *drv;
	struct nas *nas;

	node = vfs_find_node(pathname, NULL);
	/*
	if(0 == (node->type & S_IWRITE)) {
		return -EPERM;
	}
	*/
	nas = node->nas;
	drv = nas->fs->drv;
	if(NULL == drv->fsop->delete_node) {
		errno = EPERM;
		return -1;
	}

	return drv->fsop->delete_node (node);
}

int krmdir(const char *pathname) {
	node_t *node;
	fs_drv_t *drv;
	struct nas *nas;

	node = vfs_find_node(pathname, NULL);
	nas = node->nas;
	drv = nas->fs->drv;

	if(NULL == drv->fsop->delete_node) {
		return -1;
	}

	return drv->fsop->delete_node(node);
}


int klstat(const char *path, struct stat *buf) {
	node_t *node;

	if(NULL == (node = vfs_find_node(path, NULL))) {
		return -1;
	}

	kfile_fill_stat(node, buf);

	return 0;
}

int kformat(const char *pathname, const char *fs_type) {
	node_t *node;
	fs_drv_t *drv;

	if(0 != fs_type) {
		drv = fs_driver_find_drv((const char *) fs_type);
		if(NULL == drv) {
			return -EINVAL;
		}
		if (NULL == drv->fsop->format) {
			return  -ENOSYS;
		}
	}
	else {
		return -EINVAL;
	}

	node = vfs_find_node(pathname, NULL);
	if(NULL == node) {
		return -ENODEV;
	}
	/*
	if(0 == (node->type & S_IWRITE)) {
		return -EPERM;
	}
	*/

	return drv->fsop->format (node);
}

int kmount(char *dev, char *dir, char *fs_type) {
	struct node *dev_node, *dir_node;
	fs_drv_t *drv;

	if(0 != fs_type) {
		drv = fs_driver_find_drv((const char *) fs_type);
		if(NULL == drv) {
			return -EINVAL;
		}
		if (NULL == drv->fsop->mount) {
			return  -ENOSYS;
		}
	}
	else {
		return -EINVAL;
	}

	/* find device */
	if(NULL == (dev_node = vfs_find_node((const char *) dev, NULL))) {
		if(0 != strcmp((const char *) fs_type, "nfs")) {
			printf("mount: no such device\n");
			return -ENODEV;
		}
		else {
			dev_node = (node_t *) dev;
		}
	}
	/* find directory */
	if (NULL == (dir_node = vfs_find_node(dir, NULL))) {
		/*FIXME: usually mount doesn't create a directory*/
		if (NULL == (dir_node = vfs_add_path (dir, NULL))) {
			return -ENODEV;/*device not found*/
		}
		dir_node->type = NODE_TYPE_DIRECTORY;
	}

	return drv->fsop->mount(dev_node, dir_node);
}
