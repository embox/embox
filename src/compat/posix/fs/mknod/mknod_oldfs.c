/**
 * @file
 *
 * @date 14.04.2017
 * @author Anton Bondarev
 */
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fs/vfs.h>

int mknod(const char *pathname, mode_t mode, dev_t dev) {
	struct path root, child;

	vfs_get_root_path(&root);
	vfs_create(&root, pathname, mode, &child);

	SET_ERRNO(ENOSYS);

	return -1;
}
