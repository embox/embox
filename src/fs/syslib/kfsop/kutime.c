/*
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#include <stddef.h>
#include <utime.h>

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
