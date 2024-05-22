/**
 * @file
 *
 * @date 21.01.2015
 * @author: Anton Bondarev
 */

#ifndef SRC_FS_DRIVER_FUSE_FUSE_DRIVER_H_
#define SRC_FS_DRIVER_FUSE_FUSE_DRIVER_H_

struct fuse_lowlevel_ops;
struct task;
struct thread;

struct fuse_sb_priv_data {
	struct fuse_lowlevel_ops *fuse_lowlevel_ops;
	struct task *fuse_task;
	struct thread *stub_thread;
};

extern const struct super_block_operations fuse_sbops;
extern const struct inode_operations fuse_iops;
extern const struct file_operations fuse_fops;

#endif /* SRC_FS_DRIVER_FUSE_FUSE_DRIVER_H_ */
