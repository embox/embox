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

static int create_new_node(struct node *parent, char *name, mode_t mode) {
	struct node *node;
	fs_drv_t *drv;
	int retval = 0;

	node = vfs_create(parent, name, mode);
	if (!node) {
		return -ENOMEM;
	}

	/* check drv of parents */
	drv = parent->nas->fs->drv;
	if (!drv || !drv->fsop->create_node) {
		retval = -1;
		goto out;
	}

	retval = drv->fsop->create_node(parent, node);
	if (retval) {
		goto out;
	}

	return 0;

out:
	vfs_del_leaf(node);
	return retval;
}

int kcreat(struct node *root_node, const char *pathname, mode_t mode) {
	char tpath[MAX_LENGTH_PATH_NAME];
	char path[MAX_LENGTH_PATH_NAME];
	char node_name[MAX_LENGTH_FILE_NAME];
	size_t path_offset, path_len, name_len;
	struct node *node;

	/* if node already exist return error */
	if (NULL != (node = vfs_lookup(root_node, pathname))) {
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
	if (NULL == node) {
		return -1;
	}

	mode &= S_IRWXU | S_IRWXG | S_IRWXO; /* leave only permission bits */

	path_len = strlen(path);
	path_offset = strlen(tpath);

#if 1
	while (1) {
		path_get_next_name(path + path_offset, node_name, sizeof(node_name));
		name_len = strlen(node_name);

		if (path_offset + name_len + 1 < path_len) {
			path_offset += name_len + 1;
		} else {
			break;
		}

		if (-1 == kmkdir(node, node_name, mode)) {
			return -1;
		}

		node = vfs_lookup_child(node, node_name);
		assert (node);
	}

	if (0 == name_len) {
		path_get_next_name(path + path_offset, node_name, sizeof(node_name));
	}

	return create_new_node(node, node_name, mode | S_IFREG);
	// return create_new_node(node, node_name, NODE_TYPE_FILE);
#else

	do {
		path_get_next_name(&path[path_offset], node_name, sizeof(node_name));
		if ((path_offset + strlen(node_name) + 1) >= strlen(path)) {
			if (0 == strlen(node_name)) {
				path_get_next_name(&path[path_offset], node_name, sizeof(node_name));
			}
			/* this is a file */
			return create_new_node(node, node_name, NODE_TYPE_FILE);
		} else {
			if (-1 == kmkdir(node, node_name, mode)) {
				return -1;
			}

			node = vfs_lookup_child(node, node_name);
			path_offset += (strlen(node_name) + 1);
		}
	} while (NULL != node);


	/* set permission */

	return 0;

#endif
}

int kmkdir(struct node *root_node, const char *pathname, mode_t mode) {
	int rc;
	char tpath[MAX_LENGTH_PATH_NAME];
	char path[MAX_LENGTH_PATH_NAME];
	int path_offset;
	char node_name[MAX_LENGTH_FILE_NAME];
	struct node *node;

	if (NULL != (node = vfs_lookup(root_node, pathname))) {
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
	if (NULL == node) {
		return -1;
	}

	path_offset = strlen(tpath);

	do {
		path_get_next_name(&path[path_offset], node_name, sizeof(tpath));
		if (0 == strlen(node_name)) {
			return 0; /* we create all directory */
		}
		if (0 != (rc = create_new_node(node, node_name, NODE_TYPE_DIRECTORY))) {
//		if (0 != (rc = create_new_node(node, node_name, S_IFDIR | S_IRUSR))) {
			return -rc;
		}
		path_offset += (strlen(node_name) + 1);

		node = vfs_lookup_child(node, node_name);
	} while (NULL != node);

	return 0;
}

int kremove(const char *pathname) {
	node_t *node;
	struct nas *nas;
	fs_drv_t *drv;

	if (NULL == (node = vfs_lookup(NULL, pathname))) {
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

	node = vfs_lookup(NULL, pathname);
	/*
	if (0 == (node->type & S_IWRITE)) {
		return -EPERM;
	}
	*/
	nas = node->nas;
	drv = nas->fs->drv;
	if (NULL == drv->fsop->delete_node) {
		errno = EPERM;
		return -1;
	}

	return drv->fsop->delete_node (node);
}

int krmdir(const char *pathname) {
	node_t *node;
	fs_drv_t *drv;
	struct nas *nas;

	node = vfs_lookup(NULL, pathname);
	nas = node->nas;
	drv = nas->fs->drv;

	if (NULL == drv->fsop->delete_node) {
		return -1;
	}

	return drv->fsop->delete_node(node);
}


int klstat(const char *path, struct stat *buf) {
	node_t *node;

	if (NULL == (node = vfs_lookup(NULL, path))) {
		return -1;
	}

	kfile_fill_stat(node, buf);

	return 0;
}

int kformat(const char *pathname, const char *fs_type) {
	node_t *node;
	fs_drv_t *drv;

	if (0 != fs_type) {
		drv = fs_driver_find_drv((const char *) fs_type);
		if (NULL == drv) {
			return -EINVAL;
		}
		if (NULL == drv->fsop->format) {
			return  -ENOSYS;
		}
	}
	else {
		return -EINVAL;
	}

	node = vfs_lookup(NULL, pathname);
	if (NULL == node) {
		return -ENODEV;
	}
	/*
	if (0 == (node->type & S_IWRITE)) {
		return -EPERM;
	}
	*/

	return drv->fsop->format (node);
}

int kmount(const char *dev, const char *dir, const char *fs_type) {
	struct node *dev_node, *dir_node;
	fs_drv_t *drv;

	if (!fs_type) {
		return -EINVAL;
	}

	drv = fs_driver_find_drv(fs_type);
	if (!drv) {
		return -EINVAL;
	}
	if (!drv->fsop->mount) {
		return  -ENOSYS;
	}

		/* find device */
	if (NULL == (dev_node = vfs_lookup(NULL, dev))) {
		if (0 != strcmp(fs_type, "nfs")) {
			printf("mount: no such device\n");
			return -ENODEV;
		}
		else {
			dev_node = (node_t *) dev;
		}
	}
	/* find directory */
	dir_node = vfs_lookup(NULL, dir);
	if (!dir_node) {
#if 0
		return -ENOENT;
#else
		/*FIXME: usually mount doesn't create a directory*/
		dir_node = vfs_create(NULL, dir, S_IFDIR);
		if (!dir_node) {
			return -ENOENT; /*device not found*/
		}
		dir_node->type = NODE_TYPE_DIRECTORY;
#endif
	}

	return drv->fsop->mount(dev_node, dir_node);
}
