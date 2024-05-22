/**
 * @file
 *
 * @date 15.01.2015
 * @author: Anton Bondarev
 */
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

#include <fs/dvfs.h>
#include <kernel/thread.h>
#include <kernel/task.h>


/* Allocates embox-specific requests for the FUSE */
#include <fs/fuse_req_alloc.h>

// Needed by fuse_common.h
#define _FILE_OFFSET_BITS 64

// it is from fuse-ext2fs.c
#define FUSE_USE_VERSION 25
#include <fuse_lowlevel.h>
#include <fuse_opt.h>
#include <fuse_kernel.h>

#define FUSE_MAX_NAMELEN 255

struct fuse_data {
	struct fuse_file_info fi;
	char name[FUSE_MAX_NAMELEN];
};

struct fuse_sb_priv_data {
	struct fuse_lowlevel_ops *fuse_lowlevel_ops;
	struct task *fuse_task;
	struct thread *stub_thread;
};

static struct task *fuse_in(struct fuse_sb_priv_data *data) {
	struct task *cur_task;
	struct thread *t = thread_self();

	sched_lock();

	cur_task = task_self();

	task_thread_register(task_self(), data->stub_thread);
	cur_task->tsk_main = data->stub_thread;
	task_thread_unregister(task_self(), t);
	t->task = NULL;
	task_thread_register(data->fuse_task, t);

	sched_unlock();

	return cur_task;
}

static void fuse_out(struct fuse_sb_priv_data *data, struct task *task) {
	struct thread *t = thread_self();

	sched_lock();

	task_thread_unregister(data->fuse_task, t);
	t->task = NULL;
	task_thread_register(task, t);
	task->tsk_main = t;
	task_thread_unregister(task, data->stub_thread);
	data->stub_thread->task = NULL;

	sched_unlock();
}

static void fuse_fill_req(struct fuse_req_embox *req, struct inode *node, void *buf) {
	struct fuse_data *data = node->i_data;

	req->node = node;
	req->fi = &data->fi;
	req->buf = buf;
}

static struct idesc *fuse_open(struct inode *node, struct idesc *desc, int __oflag) {
	struct fuse_data *data;
	struct task *task;
	struct fuse_sb_priv_data *sb_fuse_data;

	sb_fuse_data = node->i_sb->sb_data;

	if (NULL == (node->i_data = malloc(sizeof(struct fuse_data)))) {
		return NULL;
	}
	data = node->i_data;
	/* FIXME check this */
	data->fi.flags = node->i_mode;

	task = fuse_in(sb_fuse_data);
	sb_fuse_data->fuse_lowlevel_ops->open(NULL, node->i_no, &data->fi);
	fuse_out(sb_fuse_data, task);

	return desc;
}

static int fuse_close(struct file_desc *desc) {
	struct inode *inode;
	struct fuse_req_embox *req;
	struct task *task;
	struct fuse_sb_priv_data *sb_fuse_data;

	sb_fuse_data = desc->f_inode->i_sb->sb_data;

	if (NULL == (req = fuse_req_alloc())) {
		return -1;
	}
	inode = desc->f_inode;
	fuse_fill_req(req, inode, NULL);
	task = fuse_in(sb_fuse_data);
	sb_fuse_data->fuse_lowlevel_ops->flush((fuse_req_t) req, inode->i_no, req->fi);
	sb_fuse_data->fuse_lowlevel_ops->release((fuse_req_t) req, inode->i_no, req->fi);
	fuse_out(sb_fuse_data, task);
	fuse_req_free(req);

	free(inode->i_data);

	return 0;
}

static int fuse_remove(struct inode *dir, struct inode *inode) {
	struct task *task;
	struct inode *parent;
	struct fuse_req_embox *req;
	struct fuse_sb_priv_data *sb_fuse_data;

	sb_fuse_data = inode->i_sb->sb_data;

	if (NULL == (req = fuse_req_alloc())) {
		return -1;
	}

	parent = inode->i_dentry->parent->d_inode;

	fuse_fill_req(req, inode, NULL);
	task = fuse_in(sb_fuse_data);
	sb_fuse_data->fuse_lowlevel_ops->unlink((fuse_req_t) req, parent->i_no, inode_name(inode));
	fuse_out(sb_fuse_data, task);
	fuse_req_free(req);

	dvfs_destroy_inode(inode);

	return 0;
}

static size_t fuse_read(struct file_desc *desc, void *buf, size_t size) {
	struct inode *inode;
	struct fuse_req_embox *req;
	size_t ret;
	struct task *task;
	struct fuse_sb_priv_data *sb_fuse_data;

	sb_fuse_data = desc->f_inode->i_sb->sb_data;

	if (NULL == (req = fuse_req_alloc())) {
		return 0;
	}

	inode = desc->f_inode;
	if (size > inode->length - desc->pos) {
		size = inode->length - desc->pos;
	}
	fuse_fill_req(req, inode, buf);
	task = fuse_in(sb_fuse_data);
	sb_fuse_data->fuse_lowlevel_ops->read((fuse_req_t) req, inode->i_no, size, desc->pos, req->fi);
	fuse_out(sb_fuse_data, task);
	memcpy(buf, req->buf, req->buf_size);
	ret = req->buf_size;
	fuse_req_free(req);

	return ret;
}

static size_t fuse_write(struct file_desc *desc, void *buf, size_t size) {
	struct inode *inode;
	struct fuse_req_embox *req;
	size_t ret;
	struct task *task;
	struct fuse_sb_priv_data *sb_fuse_data;

	sb_fuse_data = desc->f_inode->i_sb->sb_data;

	if (NULL == (req = fuse_req_alloc())) {
		return 0;
	}

	inode = desc->f_inode;
	fuse_fill_req(req, inode, buf);
	task = fuse_in(sb_fuse_data);
	sb_fuse_data->fuse_lowlevel_ops->write((fuse_req_t) req, inode->i_no, buf, size, desc->pos, req->fi);
	fuse_out(sb_fuse_data, task);
	ret = req->buf_size;
	fuse_req_free(req);

	return ret;
}

static struct inode *fuse_lookup(struct inode *node, char const *name, struct inode const *dir) {
	struct fuse_req_embox *req;
	struct task *task;
	struct fuse_sb_priv_data *sb_fuse_data;

	sb_fuse_data = dir->i_sb->sb_data;

	if (NULL == (req = fuse_req_alloc())) {
		return NULL;
	}

	fuse_fill_req(req, node, NULL);
	task = fuse_in(sb_fuse_data);
	sb_fuse_data->fuse_lowlevel_ops->lookup((fuse_req_t) req, dir->i_no, name);
	fuse_out(sb_fuse_data, task);
	fuse_req_free(req);

	if (node->i_no == -1) {
		dvfs_destroy_inode(node);
		return NULL;
	}

	return node;
}

static int fuse_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *ctx) {
	char buf[512];
	int res = 0;
	struct fuse_req_embox *req;
	struct task *task;
	struct fuse_dirent *dirent;
	struct fuse_data *data;
	struct fuse_sb_priv_data *sb_fuse_data;

	int idx = (int) ctx->fs_ctx;
	size_t i = 0;
	size_t offset = 0;

	sb_fuse_data = parent->i_sb->sb_data;

	if (NULL == (req = fuse_req_alloc())) {
		return -1;
	}
	fuse_fill_req(req, parent, buf);
	task = fuse_in(sb_fuse_data);
	sb_fuse_data->fuse_lowlevel_ops->readdir((fuse_req_t) req, parent->i_no, 512, 0, req->fi);
	fuse_out(sb_fuse_data, task);

	for (i = 0; i < idx + 1; i++) {
		dirent = (struct fuse_dirent *)(buf  + offset);
		offset += fuse_dirent_size(dirent->namelen);
	}
	ctx->fs_ctx = (void *)++idx;

	if (NULL == (data = malloc(sizeof *data))) {
		res = -1;
		goto out;
	}
	next->i_no = -1;
	next->i_data = data;
	strcpy(data->name, dirent->name);
	strcpy(name, data->name);

	fuse_fill_req(req, next, NULL);
	task = fuse_in(sb_fuse_data);
	sb_fuse_data->fuse_lowlevel_ops->lookup((fuse_req_t) req, parent->i_no, dirent->name);
	fuse_out(sb_fuse_data, task);
	// If not found
	if (next->i_no < 0) {
		res = -1;
	}

out:
	fuse_req_free(req);
	return res;
}

static int fuse_create(struct inode *i_new, struct inode *i_dir, int mode) {
	struct fuse_req_embox *req;
	struct task *task;
	struct fuse_sb_priv_data *sb_fuse_data;

	sb_fuse_data = i_dir->i_sb->sb_data;

	if (NULL == (req = fuse_req_alloc())) {
		return -1;
	}

	fuse_fill_req(req, i_new, NULL);
	task = fuse_in(sb_fuse_data);
	if (mode & S_IFDIR) {
		sb_fuse_data->fuse_lowlevel_ops->mkdir(
				(fuse_req_t) req, i_dir->i_no, inode_name(i_new), i_new->i_mode);
	} else {
		sb_fuse_data->fuse_lowlevel_ops->create(
				(fuse_req_t) req, i_dir->i_no, inode_name(i_new), i_new->i_mode, req->fi);
	}
	fuse_out(sb_fuse_data, task);
	fuse_req_free(req);

	return 0;
}

static int ext2fuse_getxattr(struct inode *node, const char *name,
		char *value, size_t size) {
	struct fuse_req_embox *req;
	struct task *task;
	int res;
	struct fuse_sb_priv_data *sb_fuse_data;

	sb_fuse_data = node->i_sb->sb_data;

	if (NULL == (req = fuse_req_alloc())) {
		return -1;
	}

	fuse_fill_req(req, node, value);
	task = fuse_in(sb_fuse_data);

	assert(sb_fuse_data->fuse_lowlevel_ops->getxattr);
	sb_fuse_data->fuse_lowlevel_ops->getxattr((fuse_req_t) req, node->i_no, name, size);
	res = req->buf_size;

	fuse_out(sb_fuse_data, task);
	fuse_req_free(req);

	return res;
}

static int ext2fuse_setxattr(struct inode *node, const char *name,
		const char *value, size_t size, int flags) {
	struct fuse_req_embox *req;
	struct task *task;
	int res;
	struct fuse_sb_priv_data *sb_fuse_data;

	sb_fuse_data = node->i_sb->sb_data;

	if (NULL == (req = fuse_req_alloc())) {
		return -1;
	}

	fuse_fill_req(req, node, NULL);
	task = fuse_in(sb_fuse_data);

	assert(sb_fuse_data->fuse_lowlevel_ops->getxattr);
	sb_fuse_data->fuse_lowlevel_ops->setxattr((fuse_req_t) req, node->i_no, name, value, size, flags);
	res = req->buf_size;

	fuse_out(sb_fuse_data, task);
	fuse_req_free(req);

	return res;
}

static int fuse_destroy_inode(struct inode *inode) {
	return 0;
}

static int fuse_umount_begin(struct super_block *sb) {
	// TODO kill task and thread
	struct task *task;
	struct fuse_sb_priv_data *fuse_data;

	fuse_data = sb->sb_data;

	task = fuse_in(fuse_data);
	fuse_data->fuse_lowlevel_ops->destroy(NULL);
	fuse_out(fuse_data, task);

	return 0;
}

const struct super_block_operations fuse_sbops = {
	.open_idesc = dvfs_file_open_idesc,
	.destroy_inode = fuse_destroy_inode,
	.umount_begin = fuse_umount_begin
};

const struct inode_operations fuse_iops = {
	.ino_lookup   = fuse_lookup,
	.ino_iterate  = fuse_iterate,
	.ino_create = fuse_create,
	.ino_remove = fuse_remove,
	.ino_getxattr = ext2fuse_getxattr,
	.ino_setxattr = ext2fuse_setxattr
};

const struct file_operations fuse_fops = {
	.open = fuse_open,
	.close = fuse_close,
	.read  = fuse_read,
	.write  = fuse_write
};
