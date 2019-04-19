/**
 * @file
 *
 * @date Oct 24, 2013
 * @author: Anton Bondarev
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <fs/hlpr_path.h>
#include <fs/perm.h>
#include <security/security.h>
#include <fs/vfs.h>
#include <fs/path.h>
#include <fs/fs_driver.h>
#include <fs/file_operation.h>
#include <sys/time.h>
#include <utime.h>

int ktruncate(struct node *node, off_t length) {
	int ret;
	struct nas *nas;
	struct fs_driver *drv;

	if (node_is_directory(node)) {
		SET_ERRNO(EISDIR);
		return -1;
	}

	if (0 > (ret = fs_perm_check(node, S_IWOTH))) {
		SET_ERRNO(-ret);
		return -1;
	}

	nas = node->nas;
	drv = nas->fs->drv;

	if (NULL == drv || NULL == drv->fsop || NULL == drv->fsop->truncate) {
		//errno = EPERM;
		return 0;
	}

	if (0 > (ret = drv->fsop->truncate(node, length))) {
		SET_ERRNO(-ret);
		return -1;
	}

	return ret;
}

int kfile_fill_stat(struct node *node, struct stat *stat_buff) {
	struct nas *nas;
	struct node_info *ni;

	memset(stat_buff, 0 , sizeof(struct stat));

	nas = node->nas;
	ni = &nas->fi->ni;

	stat_buff->st_size = ni->size;
	stat_buff->st_mode = node->mode;
	stat_buff->st_uid = node->uid;
	stat_buff->st_gid = node->gid;
	stat_buff->st_ctime = ni->ctime;
	stat_buff->st_mtime = ni->mtime;
	return 0;
}

int kfile_change_stat(struct node *node, const struct utimbuf *times) {
	struct nas *nas;
	struct node_info *ni;
	struct timeval now;

	if (node == NULL) {
		return -ENOENT;
	}

	nas = node->nas;
	ni = &nas->fi->ni;

	if (times == NULL) {
		if (gettimeofday(&now, NULL) == -1) {
			return -EINVAL;
		}
		ni->ctime = now.tv_sec;
		return 0;
	}

	ni->ctime = times->actime;
	ni->mtime = times->modtime;

	return 0;
}
