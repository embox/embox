/**
 * @file
 *
 * @data 23.11.2015
 * @author: Anton Bondarev
 */

#ifndef SRC_FS_FUSE_FUSE_MODULE_H_
#define SRC_FS_FUSE_FUSE_MODULE_H_


struct fuse_module {
	const char *fuse_module_name;
	const char *fuse_module_cmd_mount;
};

extern struct fuse_module *fuse_module_lookup(char *fuse_type);

extern int fuse_module_mount(struct fuse_module *fm, char *dev, char *dest);

extern int fuse_module_umount(struct fuse_module *fm);

#include <util/array.h>
#define FUSE_MODULE_DEF(name, cmd) \
	static const struct fuse_module fm##__LINE__ = {name, cmd}; \
	ARRAY_SPREAD_DECLARE(const struct fuse_module *, fuse_module_repo); \
	ARRAY_SPREAD_ADD( fuse_module_repo, &fm##__LINE__)

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

#endif /* SRC_FS_FUSE_FUSE_MODULE_H_ */
