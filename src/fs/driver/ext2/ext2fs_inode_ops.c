/**
 * @file
 * @brief ext file system
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */

#include <string.h>

#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/ext2.h>
#include <fs/hlpr_path.h>
#include <fs/dir_context.h>
#include <fs/inode_operation.h>


extern struct ext2_file_info *ext2_fi_alloc(void);


extern int ext2_buf_read_file(struct inode *inode, char **, size_t *);
extern int ext2_read_inode(struct inode *node, uint32_t);

extern int ext2_create(struct inode *i_new, struct inode *i_dir);
extern int ext2_mkdir(struct inode *i_new, struct inode *i_dir);
extern int ext2_unlink(struct inode *dir_node, struct inode *node);
extern int ext2_close(struct inode *inode);
extern int ext2_open(struct inode *inode);

static int ext2fs_create(struct inode *node, struct inode *parent_node, int mode) {
	int rc;

	if (S_ISDIR(node->i_mode)) {
		rc = ext2_mkdir(node, parent_node);
	} else {
		rc = ext2_create(node, parent_node);
	}
	return -rc;
}

static int ext2fs_delete(struct inode *dir, struct inode *node) {
	int rc;
	struct inode *parents;

	parents = vfs_subtree_get_parent(node);
	if (NULL == parents) {
		rc = ENOENT;
		return -rc;
	}

	rc = ext2_unlink(parents, node);
	if (0 != rc) {
		return -rc;
	}

	return 0;
}

int ext2_iterate(struct inode *next, char *next_name, struct inode *parent,
					struct dir_ctx *dir_ctx) {
	char name_buff[NAME_MAX];
	struct ext2_fs_info *fsi;
	struct ext2_file_info *dir_fi;
	struct ext2_file_info *fi;
	struct ext2fs_direct *dp, *edp;
	size_t buf_size;
	char *buf;
	int rc;
	char *name;
	int idx = 0;

	fsi = parent->i_sb->sb_data;

	if (0 != ext2_open(parent)) {
		return -1;
	}

	dir_fi = inode_priv(parent);

	dir_fi->f_pointer = 0;
	while (dir_fi->f_pointer < (long) dir_fi->f_di.i_size) {
		rc = ext2_buf_read_file(parent, &buf, &buf_size);
		if (0 != rc) {
			goto out;
		}
		if (buf_size != fsi->s_block_size || buf_size == 0) {
			rc = EIO;
			goto out;
		}

		dp = (struct ext2fs_direct *) buf;
		edp = (struct ext2fs_direct *) (buf + buf_size);
		for (; dp < edp; dp = (void *)((char *)dp + fs2h16(dp->e2d_reclen))) {
			if (fs2h16(dp->e2d_reclen) <= 0) {
				goto out;
			}
			if (fs2h32(dp->e2d_ino) == 0) {
				continue;
			}

			/* set null determine name */
			name = (char *) &dp->e2d_name;

			memcpy(name_buff, name, fs2h16(dp->e2d_namlen));
			name_buff[fs2h16(dp->e2d_namlen)] = '\0';

			if(0 != path_is_dotname(name_buff, dp->e2d_namlen)) {
				/* dont need create dot or dotdot node */
				continue;
			}
			if (idx++ < (int)(uintptr_t)dir_ctx->fs_ctx) {
				continue;
			}

			//mode = ext2_type_to_mode_fmt(dp->e2d_type);
			fi = ext2_fi_alloc();
			if (!fi) {
				rc = ENOSPC;
				goto out;
			}

			memset(fi, 0, sizeof(struct ext2_file_info));
			fi->f_num = fs2h32(dp->e2d_ino);

			next->i_sb = parent->i_sb;
			inode_size_set(next, 0);
			inode_priv_set(next, fi);

			/* Load permisiions and credentials. */
			fi->f_buf = ext2_buff_alloc(fsi, fsi->s_block_size);
			if (NULL == fi->f_buf) {
				rc = ENOSPC;
				goto out;
			}

			ext2_read_inode(next, fs2h32(dp->e2d_ino));
			ext2_buff_free(fsi, fi->f_buf);

			next->i_mode = fi->f_di.i_mode;

			next->i_owner_id = fi->f_di.i_uid;
			next->i_group_id = fi->f_di.i_gid;
			inode_size_set(next, fi->f_di.i_size);
			strncpy(next_name, name_buff, NAME_MAX - 1);
			next_name[NAME_MAX - 1] = '\0';

			dir_ctx->fs_ctx = (void *)(uintptr_t)idx;

			rc = 0;
			goto out;
		}
		dir_fi->f_pointer += buf_size;
	}

	rc = -1;
out:

	ext2_close(parent);

	return rc;
}

static int ext2fs_truncate (struct inode *node, off_t length) {

	inode_size_set(node, length);

	return 0;
}

extern struct inode *ext2fs_lookup(char const *name, struct inode const *dir);

struct inode_operations ext2_iops = {
	.ino_create  = ext2fs_create,
	.ino_remove  = ext2fs_delete,
	.ino_lookup  = ext2fs_lookup,
	.ino_iterate = ext2_iterate,
	.ino_truncate = ext2fs_truncate,

	.ino_getxattr     = ext2fs_getxattr,
	.ino_setxattr     = ext2fs_setxattr,
	.ino_listxattr    = ext2fs_listxattr,
};

