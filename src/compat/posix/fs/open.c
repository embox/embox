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
#include <fs/posix.h>

#include <fs/kfile.h>

struct node *find_node(DIR *dir, char * node_name) {
	struct dirent * dent;

	while (NULL != (dent = readdir(dir))) {
		if (0 == strcmp(dent->d_name, node_name)) {
			return (struct node *)dent->d_ino;
		}
	}

	return NULL;
}
extern struct node *kcreat(struct node *dir, const char *path, mode_t mode);

int open(const char *path, int __oflag, ...) {
	struct file_desc *kfile;
	va_list args;
	mode_t mode;
	int rc;
	char *parent_path;
	DIR *dir;
	char *name;
	struct node *node;
	struct idesc_table*it;

	if (strlen(path) > PATH_MAX) {
		return -ENAMETOOLONG;
	}

	va_start(args, __oflag);
	mode = va_arg(args, mode_t);
	va_end(args);

	parent_path = dirname((char *)path);
	if (NULL == (dir = opendir(parent_path))) {
		return -1;
	}
	name = basename((char *)path);
	node = find_node(dir, name);

	if (__oflag & O_DIRECTORY) {
		assert(0);
		opendir(path);
	}

	if (node == NULL) {
		if (__oflag & O_CREAT) {
			if(NULL == (node = kcreat(dir->node, name, mode))) {
				rc =  -1;
				goto out;
			}
		} else {
			SET_ERRNO(ENOENT);
			rc = -1;
			goto out;
		}
	} else {
		/* When used with O_CREAT, if the file already exists it is an error
		 * and the open() will fail. */
		if (__oflag & O_EXCL && __oflag & O_CREAT) {
			SET_ERRNO(EEXIST);
			rc = -1;
			goto out;
		}

		if (__oflag & O_TRUNC) {
			ktruncate(node, 0);
		}
	}

	__oflag &= ~(O_CREAT | O_EXCL);

	kfile = kopen(node, __oflag);
	if (NULL == kfile) {
		rc = -1;
		goto out;
	}

	it = task_get_idesc_table(task_self());
	rc = idesc_table_add(it, (struct idesc *)kfile, 0);

out:
	closedir(dir);

	return rc;
}
