/**
 * @file
 *
 * @data 01.02.2016
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <unistd.h>

#include <fs/dvfs.h>

int chown(const char *path, uid_t owner, gid_t group) {
	struct lookup lookup = {};
	int res;

	if (!path) {
		return SET_ERRNO(EFAULT);
	}

	if ((res = dvfs_lookup(path, &lookup))) {
		return SET_ERRNO(-res);
	}
	assert(lookup.item);
	assert(lookup.item->d_inode);
	lookup.item->d_inode->i_owner_id = owner;
	lookup.item->d_inode->i_owner_id = group;

	return 0;
}
