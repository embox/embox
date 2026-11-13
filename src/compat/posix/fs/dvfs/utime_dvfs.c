/**
 * @brief
 *
 * @date 06.07.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <time.h>
#include <utime.h>

#include <fs/dvfs.h>

int utime(const char *path, const struct utimbuf *times) {
	struct lookup lu = {};
	struct inode *node;
	time_t mtime;
	int err = 0;

	if ((err = dvfs_lookup(path, &lu))) {
		return SET_ERRNO(-err);
	}
	if (lu.item == NULL) {
		return SET_ERRNO(ENOENT);
	}

	mtime = times ? times->modtime : time(NULL);

	/* A driver keeping times on the medium sets i_mtime to what it stored;
	 * otherwise the time is in memory only, until the inode leaves the cache. */
	node = lu.item->d_inode;
	if (node) {
		if (node->i_ops && node->i_ops->ino_utime) {
			err = node->i_ops->ino_utime(node, mtime);
		} else {
			node->i_mtime = mtime;
		}
	}

	dentry_ref_dec(lu.item);

	return err ? SET_ERRNO(-err) : 0;
}
