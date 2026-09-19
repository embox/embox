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
	int err;

	if ((err = dvfs_lookup(path, &lu))) {
		return SET_ERRNO(-err);
	}
	if (lu.item == NULL) {
		return SET_ERRNO(ENOENT);
	}

	/* Kept in memory only: no DVFS driver stores a time it is handed. */
	if (lu.item->d_inode) {
		lu.item->d_inode->i_mtime = times ? times->modtime : time(NULL);
	}

	dentry_ref_dec(lu.item);

	return 0;
}
