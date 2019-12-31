/**
 * @file
 * @brief
 *
 * @date 2.02.2016
 * @author Alex Kalmuk
 */

#include <sys/stat.h>
#include <assert.h>

#include <fs/vfs.h>
#include <fs/inode.h>

int chmod(const char *path, mode_t mode) {
	struct path p;
	int res;

	res = vfs_lookup(path, &p);
	if (res < 0) {
		return res;
	}
	assert(p.node);
	p.node->i_mode = mode;

	return 0;
}
