/**
 * @file
 * @brief  DVFS interface implementation
 * @author Denis Deryugin
 * @date   11 Mar 2014
 */

#include <util/log.h>

#include <assert.h>
#include <errno.h>

#include <fs/dentry.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/dvfs.h>

/**
 * @brief Delete file from storage
 * @param path Path to file
 *
 * @return Negative error code
 * @retval  0 Ok
 * @retval -1 File not found
 */
int dvfs_remove(const char *path) {
	struct lookup lookup = {};
	struct inode *i_no;
	int res;

	dvfs_lookup(path, &lookup);

	if (!lookup.item) {
		return -ENOENT;
	}

	dentry_ref_dec(lookup.item);
	i_no = lookup.item->d_inode;

	assert(i_no->i_ops);

	if (!i_no->i_ops->ino_remove)
		return -EPERM;

	if (lookup.item->usage_count > 0) {
		return -EBUSY;
	}

	res = i_no->i_ops->ino_remove(lookup.parent->d_inode, i_no);
	if (res != 0) {
		log_error("Failed to remove inode");
		return res;
	}
	
	res = dvfs_destroy_dentry(lookup.item);
	if (res != 0) {
		log_error("Failed to destroy dentry");
	}

	return res;
}
