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

/**
 * Simple util function to copy file in oldpath to newpath
 * @param Should be regular file to copy
 * @param Name of new copy
 * @return ENOERR if file successfully copied -1 and set errno in other way
 */
int copy_file(const char *oldpath, const char *newpath) {
	int oldfd, newfd, rc;
	char buf[BUFSIZ];

	oldfd = open(oldpath, O_RDONLY);
	if (-1 == oldfd) {
		return -1;
	}
	newfd = open(newpath, O_WRONLY);
	if (-1 == newfd) {
		return -1;
	}

	/* Copy bytes */
	while ((rc = read(oldfd, buf, sizeof(buf))) > 0) {
		if (write(newfd, buf, rc) <= 0) {
			SET_ERRNO(EIO);
			return -1;
		}
	}

	/* Close files and free memory*/
	rc = close(oldfd);
	if (0 != rc) {
		return -1;
	}
	rc = close(newfd);
	if (0 != rc) {
		return -1;
	}

	return ENOERR;
}

/**
 * Rename oldpath to newpath
 * @param Path to file or directory to rename
 * @param Destination path, could not be existent file
 * @return ENOERR if file successfully copied -1 and set errno in other way
 */
int krename(const char *oldpath, const char *newpath) {
	int rc, newpathlen, diritemlen;
	char *name, *newpathbuf = NULL;
	char *newpatharg, *oldpatharg;
	node_t *oldnode, *newnode, *diritem;
	/* We use custom tree traversal while I can't
	 * get success with tree_foreach_children */
	struct tree_link *link, *end_link;

	if (MAX_LENGTH_PATH_NAME < strlen(oldpath) ||
			MAX_LENGTH_PATH_NAME < strlen(newpath)) {
		SET_ERRNO(ENAMETOOLONG);
		return -1;
	}

	/* Check if source file exists */
	oldnode = vfs_find_node(oldpath, NULL);
	if (NULL == oldnode) {
		SET_ERRNO(EINVAL);
		return -1;
	}

	/* Check if destination file already exists or if directory were
	 * provided as destination path */
	newnode = vfs_find_node(newpath, NULL);
	if (NULL != newnode) {
		if (node_is_directory(newnode)) {
			/* Directory was passed as destination */
			name = strrchr(oldpath, '/') + 1;
			newpathlen = strlen(newpath) + strlen(name);
			if (newpathlen > MAX_LENGTH_PATH_NAME) {
				SET_ERRNO(ENAMETOOLONG);
				return -1;
			}
			newpathbuf = calloc(newpathlen + 2, sizeof(char));
			if (NULL == newpathbuf) {
				SET_ERRNO(ENOMEM);
				return -1;
			}
			strcat(newpathbuf, newpath);
			if (newpathbuf[strlen(newpathbuf) - 1] != '/') {
				strcat(newpathbuf, "/");
			}
			strcat(newpathbuf, name);
			newpath = newpathbuf;
		} else {
			SET_ERRNO(EINVAL);
			return -1;
		}
	}

	/**
	 * TODO:
	 * Here we should check if we move within one filesystem and don't copy
	 * data in such case. Instead of that just make new hardlink
	 * and remove old one.
	 */

	/* If oldpath is directory, copy it recursively */
	if (node_is_directory(oldnode)) {
		rc = mkdir(newpath, oldnode->mode);
		if (-1 == rc) {
			return -1;
		}

		/**
		 * Following line should be here:
		 *  tree_foreach_children(diritem, (&oldnode->tree_link), tree_link) {
		 * But it's not working with it.
		 */
		link = tree_children_begin(&oldnode->tree_link);
		end_link = tree_children_end(&oldnode->tree_link);

		while (link != end_link) {
			diritem = tree_element(link, typeof(*diritem), tree_link);
			link = tree_children_next(link);

			if (0 != strcmp(".", diritem->name) &&
					0 != strcmp("..", diritem->name)) {
				diritemlen = strlen(diritem->name);
				oldpatharg =
						calloc(strlen(oldpath) + diritemlen + 2, sizeof(char));
				newpatharg =
						calloc(strlen(newpath) + diritemlen + 2, sizeof(char));
				if (NULL == oldpatharg || NULL == newpatharg) {
					SET_ERRNO(ENOMEM);
					return -1;
				}

				strcat(oldpatharg, oldpath);
				if (oldpatharg[strlen(oldpatharg) - 1] != '/') {
					strcat(oldpatharg, "/");
				}
				strcat(oldpatharg, diritem->name);
				strcat(newpatharg, newpath);
				if (newpatharg[strlen(newpatharg) - 1] != '/') {
					strcat(newpatharg, "/");
				}
				strcat(newpatharg, diritem->name);

				/* Call itself recursively */
				if (-1 == krename(oldpatharg, newpatharg)) {
					return -1;
				}

				free(newpatharg);
				free(oldpatharg);
			}
		}
	/* Or copy file */
	} else {
		rc = copy_file(oldpath, newpath);
		if (-1 == rc) {
			return -1;
		}
	}

	if (NULL != newpathbuf) {
		free(newpathbuf);
	}

	/* Delete file in old path */
	rc = remove(oldpath);
	if (0 != rc) {
		return -1;
	}

	return ENOERR;
}
