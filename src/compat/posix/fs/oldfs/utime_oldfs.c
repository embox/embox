/**
 * @file
 * @brief utime function
 *
 * @author  Filipp Chubukov
 * @date    16.04.2014
 */

#include <utime.h>
#include <errno.h>

#include <fs/kfsop.h>
#include <fs/perm.h>
#include <fs/path.h>

int kutime(const char *path,const struct utimbuf *times) {
	struct path node;
	int res;

	if (0 != (res = fs_perm_lookup(path, NULL, &node))) {
		return res;
	}

	return kfile_change_stat(node.node, times);
}

int utime(const char *path, const struct utimbuf *times) {
	int res;
	if (0 != (res = kutime(path, times))) {
		errno = -res;
		return -1;
	}
	return 0;	
}
