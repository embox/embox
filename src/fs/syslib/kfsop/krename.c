/*
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <lib/libds/tree.h>

#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/kfsop.h>
#include <fs/perm.h>

#include <fs/path.h>

/**
 * Simple util function to copy file in oldpath to newpath
 * @param Should be regular file to copy
 * @param Name of new copy
 * @return ENOERR if file successfully copied -1 and set errno in other way
 */
static int copy_file(const char *oldpath, const char *newpath) {
	int oldfd, newfd, rc;
	char buf[BUFSIZ];
	struct stat old_st;

	oldfd = open(oldpath, O_RDONLY);
	if (-1 == oldfd) {
		return -1;
	}

	if (-1 == fstat(oldfd, &old_st)) {
		return -1;
	}

	newfd = open(newpath, O_CREAT|O_WRONLY|O_TRUNC, old_st.st_mode & 0777);
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
	char *oldpathcopy, *newpathcopy;
	char *opc_free, *npc_free;
	char *name, *newpathbuf = NULL;
	char *newpatharg, *oldpatharg;
	struct path oldnode, newnode;
	struct inode *diritem;
	/* We use custom tree traversal while I can't
	 * get success with tree_foreach_children */
	struct tree_link *link, *end_link;

	if (PATH_MAX < strlen(oldpath) ||
			PATH_MAX < strlen(newpath)) {
		SET_ERRNO(ENAMETOOLONG);
		return -1;
	}

	/* Check if source file exists */
	oldpathcopy = strdup(oldpath);
	opc_free = oldpathcopy;
	rc = fs_perm_lookup((const char *) oldpathcopy,
			(const char **) &oldpathcopy, &oldnode);
	free(opc_free);
	if (0 != rc) {
		SET_ERRNO(-rc);
		return -1;
	}

	/* Check if destination file already exists or if directory were
	 * provided as destination path */
	newpathcopy = strdup(newpath);
	npc_free = newpathcopy;
	rc = fs_perm_lookup((const char *) newpathcopy,
			(const char **) &newpathcopy, &newnode);
	free(npc_free);
	if (0 == rc) {
		if (S_ISDIR(newnode.node->i_mode)) {
			/* Directory was passed as destination */
			name = strrchr(oldpath, '/') + 1;
			newpathlen = strlen(newpath) + strlen(name);
			if (newpathlen > PATH_MAX) {
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
	if (S_ISDIR(oldnode.node->i_mode)) {
		rc = kmkdir(newpath, oldnode.node->i_mode);
		if (-1 == rc) {
			return -1;
		}

		/**
		 * Following line should be here:
		 *  tree_foreach_children(diritem, (&oldnode->tree_link), tree_link) {
		 * But it's not working with it.
		 */
		link = tree_children_begin(&oldnode.node->tree_link);
		end_link = tree_children_end(&oldnode.node->tree_link);

		while (link != end_link) {
			char *node_name;

			diritem = tree_element(link, typeof(*diritem), tree_link);
			link = tree_children_next(link);

			node_name = inode_name(diritem);
			if (0 != strcmp(".", node_name) &&
					0 != strcmp("..", node_name)) {
				diritemlen = strlen(node_name);
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
				strcat(oldpatharg, node_name);
				strcat(newpatharg, newpath);
				if (newpatharg[strlen(newpatharg) - 1] != '/') {
					strcat(newpatharg, "/");
				}
				strcat(newpatharg, node_name);

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
	rc = kremove(oldpath);
	if (0 != rc) {
		return -1;
	}

	return ENOERR;
}
