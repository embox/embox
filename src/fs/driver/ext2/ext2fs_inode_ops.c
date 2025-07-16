/**
 * @file
 * @brief ext file system
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/ext2.h>
#include <fs/hlpr_path.h>
#include <fs/dir_context.h>
#include <fs/inode_operation.h>

extern struct ext2_file_info *ext2_fi_alloc(void);

extern int ext2_buf_read_file(struct inode *inode, char **, size_t *);
extern int ext2_read_inode(struct inode *node, uint32_t);
extern int ext2_search_directory(struct inode *node, const char *, int, uint32_t *);

extern int ext2_create(struct inode *i_new, struct inode *i_dir);
extern int ext2_mkdir(struct inode *i_new, struct inode *i_dir);
extern int ext2_unlink(struct inode *dir_node, struct inode *node);

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

	rc = ext2_unlink(dir, node);
	if (0 != rc) {
		return -rc;
	}

	return 0;
}

extern int ext2_set_inode_priv(struct inode *node);
extern mode_t ext2_type_to_mode_fmt(uint8_t e2d_type);
int ext2_iterate(struct inode *next, char *next_name, struct inode *parent,
					struct dir_ctx *dir_ctx) {
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
			int name_len;

			if (fs2h16(dp->e2d_reclen) <= 0) {
				goto out;
			}
			if (fs2h32(dp->e2d_ino) == 0) {
				continue;
			}
			
			name_len = fs2h16(dp->e2d_namlen);
			assert (name_len < NAME_MAX);
			/* set null determine name */
			name = (char *) &dp->e2d_name;

			if(0 != path_is_dotname(name, name_len)) {
				/* dont need create dot or dotdot node */
				continue;
			}

			if (idx++ < (int)(uintptr_t)dir_ctx->fs_ctx) {
				continue;
			}

			rc = ext2_set_inode_priv(next);
			if (rc) {
				goto out;
			}
			fi = inode_priv(next);

			next->i_sb = parent->i_sb;

			fi->f_num = fs2h32(dp->e2d_ino);

			ext2_read_inode(next, fi->f_num);

			//mode = ext2_type_to_mode_fmt(dp->e2d_type);
			next->i_mode = fi->f_di.i_mode; //ext2_type_to_mode_fmt(fi->f_di.i_mode);
			next->i_owner_id = fi->f_di.i_uid;
			next->i_group_id = fi->f_di.i_gid;

			inode_size_set(next, fi->f_di.i_size);

			//ext2_buff_free(fsi, fi->f_buf);

			strncpy(next_name, name, NAME_MAX - 1);
			next_name[NAME_MAX - 1] = '\0';

			dir_ctx->fs_ctx = (void *)(uintptr_t)idx;

			rc = 0;
			goto out;
		}
		dir_fi->f_pointer += buf_size;
	}

	rc = -1;
out:
	return rc;
}

static int ext2fs_truncate(struct inode *node, off_t length) {

	inode_size_set(node, length);

	return 0;
}

/* libext2fs from 'third_party.e2fsprogs.core' have function ext2fs_lookup() */
struct inode *_ext2fs_lookup(struct inode *node, char const *name, struct inode const *dir) {
	uint32_t i_no;
	int rc;
	struct ext2_file_info *fi;

	if (dir == NULL) {
		dir = node->i_sb->sb_root;
	}

	rc = ext2_search_directory((struct inode *)dir, name, strlen(name), &i_no);
	if (0 != rc) {
		return NULL;
	}

	rc = ext2_set_inode_priv(node);
	if (rc) {
		return NULL;
	}

	fi = inode_priv(node);

	node->i_sb = dir->i_sb;

	fi->f_num = i_no;

	ext2_read_inode(node, fi->f_num);

	//mode = ext2_type_to_mode_fmt(dp->e2d_type);
	node->i_mode = fi->f_di.i_mode; //ext2_type_to_mode_fmt(fi->f_di.i_mode);
	node->i_owner_id = fi->f_di.i_uid;
	node->i_group_id = fi->f_di.i_gid;

	inode_size_set(node, fi->f_di.i_size);

	return node;
}

struct inode_operations ext2_iops = {
	.ino_create  = ext2fs_create,
	.ino_remove  = ext2fs_delete,
	.ino_lookup  = _ext2fs_lookup,
	.ino_iterate = ext2_iterate,
	.ino_truncate = ext2fs_truncate,

	.ino_getxattr     = ext2fs_getxattr,
	.ino_setxattr     = ext2fs_setxattr,
	.ino_listxattr    = ext2fs_listxattr,
};

