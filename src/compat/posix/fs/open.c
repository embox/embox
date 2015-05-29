/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    04.10.2012
 */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h>

#include <util/array.h>
#include <fs/vfs.h>

#include <fs/kfile.h>
#include <kernel/task/idesc_table.h>
#include <kernel/task/resource/idesc_table.h>

#include <dirent_impl.h>
#include "getumask.h"

struct node *find_node(DIR *dir, char * node_name) {
	struct dirent * dent;

	while (NULL != (dent = readdir(dir))) {
		if (0 == strcmp(dent->d_name, node_name)) {
			return (struct node *)dent->d_ino;
		}
	}

	return NULL;
}
extern int kcreat(struct path *dir, const char *path, mode_t mode, struct path *child);

int open(const char *path, int __oflag, ...) {
	char path_buf[PATH_MAX];
	char name[NAME_MAX];
	struct file_desc *kfile;
	va_list args;
	mode_t mode;
	int rc;
	char *parent_path;
	char *bname;
	DIR *dir;
	struct node *node;
	struct path node_path;
	struct idesc_table*it;

	assert(~__oflag & O_DIRECTORY);

	if (strlen(path) >= PATH_MAX) {
		return SET_ERRNO(ENAMETOOLONG);
	}

	va_start(args, __oflag);
	mode = va_arg(args, mode_t);
	mode = umask_modify(mode);
	va_end(args);

	strncpy(path_buf, path, PATH_MAX);
	bname = basename(path_buf);

	if (strlen(bname) >= NAME_MAX)
		return SET_ERRNO(ENAMETOOLONG);

	strncpy(name, bname, NAME_MAX);

	if (0 == strcmp(name, "/")) {
		return SET_ERRNO(EISDIR);
	}

	parent_path = dirname(strcpy(path_buf, path));
	if (NULL == (dir = opendir(parent_path))) {
		return SET_ERRNO(errno);
	}

	node = find_node(dir, name);

	node_path.node = node;
	if_mounted_follow_down(&dir->path);
	node_path.mnt_desc = dir->path.mnt_desc;

	if (node == NULL) {
		if (__oflag & O_CREAT) {
			if(0 > kcreat(&dir->path, name, mode, &node_path)) {
				rc = -errno;
				goto out;
			}
		} else {
			rc = -ENOENT;
			goto out;
		}
	} else {
		/* When used with O_CREAT, if the file already exists it is an error
		 * and the open() will fail. */
		if (__oflag & O_EXCL && __oflag & O_CREAT) {
			rc = -EEXIST;
			goto out;
		}

		if (node_is_directory(node)) {
			rc = -EISDIR;
			goto out;
		}
		else if (path[strlen(path) - 1] == '/') {
			rc = -ENOTDIR;
			goto out;
		}

		if (__oflag & O_TRUNC) {
			if (-1 == ktruncate(node, 0)){
				//errno already setup
				return -1;
			}
		}
	}

	__oflag &= ~(O_CREAT | O_EXCL);

	kfile = kopen(node_path.node, __oflag);
	if (NULL == kfile) {
		rc = -errno;
		goto out;
	}

	it = task_resource_idesc_table(task_self());
	rc = idesc_table_add(it, (struct idesc *)kfile, 0);

out:
	closedir(dir);

	return rc >= 0 ? rc : SET_ERRNO(-rc);
}
