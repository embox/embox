/**
 * @file
 * @brief ext2fuse support
 *
 * @date 16.11.2015
 * @author Alexander Kalmuk
 */

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

#include <fs/dvfs.h>
#include <kernel/printk.h>
#include <embox/unit.h>
#include <kernel/task.h>

// Allocates embox-specific requests for the FUSE
#include <fuse_req_alloc.h>

// Needed by fuse_common.h
#define _FILE_OFFSET_BITS 64

// it is from fuse-ext2fs.c
#define FUSE_USE_VERSION 25
#include <fuse_lowlevel.h>
#include <fuse_opt.h>
#include <fuse_kernel.h>

#define EXT2_MAX_NAMELEN 255

extern struct fuse_lowlevel_ops *ext2fs_register(void);

static struct fuse_lowlevel_ops *ext2fuse_ops;

struct ext2fuse_data {
	struct fuse_file_info fi;
	char name[EXT2_MAX_NAMELEN];
};

static struct task *mount_task;
static struct thread *stub_thread;


static struct task *get_mount_task(void) {
	struct task *t;

	task_foreach(t) {
		if (!strcmp(task_get_name(t), "ext2fs_mount")) {
			return t;
		}
	}
	return NULL;
}

static struct task *fuse_in(void) {
	struct task *cur_task;
	struct thread *t = thread_self();

	sched_lock();

	cur_task = task_self();

	task_thread_register(task_self(), stub_thread);
	cur_task->tsk_main = stub_thread;
	task_thread_unregister(task_self(), t);
	t->task = NULL;
	task_thread_register(mount_task, t);

	sched_unlock();

	return cur_task;
}

static void fuse_out(struct task *task) {
	struct thread *t = thread_self();

	sched_lock();

	task_thread_unregister(mount_task, t);
	t->task = NULL;
	task_thread_register(task, t);
	task->tsk_main = t;
	task_thread_unregister(task, stub_thread);
	stub_thread->task = NULL;

	sched_unlock();
}

static void ext2fuse_fill_req(struct fuse_req_embox *req, struct inode *node, void *buf) {
	struct ext2fuse_data *data = node->i_data;

	req->node = node;
	req->fi = &data->fi;
	req->buf = buf;
}

static struct idesc *ext2fuse_open(struct inode *node, struct idesc *desc) {
	struct ext2fuse_data *data;
	struct task *task;

	if (NULL == (node->i_data = malloc(sizeof(struct ext2fuse_data)))) {
		return NULL;
	}
	data = node->i_data;
	/* FIXME check this */
	data->fi.flags = node->flags;

	task = fuse_in();
	ext2fuse_ops->open(NULL, node->i_no, &data->fi);
	fuse_out(task);

	return desc;
}

static int ext2fuse_close(struct file *desc) {
	struct inode *inode;
	struct fuse_req_embox *req;
	struct task *task;

	if (NULL == (req = fuse_req_alloc())) {
		return -1;
	}
	inode = desc->f_inode;
	ext2fuse_fill_req(req, inode, NULL);
	task = fuse_in();
	ext2fuse_ops->flush((fuse_req_t) req, inode->i_no, req->fi);
	ext2fuse_ops->release((fuse_req_t) req, inode->i_no, req->fi);
	fuse_out(task);
	fuse_req_free(req);

	free(inode->i_data);

	return 0;
}

static size_t ext2fuse_read(struct file *desc, void *buf, size_t size) {
	struct inode *inode;
	struct fuse_req_embox *req;
	size_t ret;
	struct task *task;

	if (NULL == (req = fuse_req_alloc())) {
		return 0;
	}

	inode = desc->f_inode;
	if (size > inode->length - desc->pos) {
		size = inode->length - desc->pos;
	}
	ext2fuse_fill_req(req, inode, buf);
	task = fuse_in();
	ext2fuse_ops->read((fuse_req_t) req, inode->i_no, size, desc->pos, req->fi);
	fuse_out(task);
	memcpy(buf, req->buf, req->buf_size);
	ret = req->buf_size;
	fuse_req_free(req);

	return ret;
}

static size_t ext2fuse_write(struct file *desc, void *buf, size_t size) {
	struct inode *inode;
	struct fuse_req_embox *req;
	size_t ret;
	struct task *task;

	if (NULL == (req = fuse_req_alloc())) {
		return 0;
	}

	inode = desc->f_inode;
	ext2fuse_fill_req(req, inode, buf);
	task = fuse_in();
	ext2fuse_ops->write((fuse_req_t) req, inode->i_no, buf, size, desc->pos, req->fi);
	fuse_out(task);
	ret = req->buf_size;
	fuse_req_free(req);

	return ret;
}

static struct inode *ext2fuse_lookup(char const *name, struct dentry const *dir) {
	struct inode *node;
	struct fuse_req_embox *req;
	struct task *task;

	if (NULL == (node = dvfs_alloc_inode(dir->d_sb))) {
		return NULL;
	}
	if (NULL == (req = fuse_req_alloc())) {
		return NULL;
	}

	ext2fuse_fill_req(req, node, NULL);
	task = fuse_in();
	ext2fuse_ops->lookup((fuse_req_t) req, dir->d_inode->i_no, name);
	fuse_out(task);
	fuse_req_free(req);

	if (node->i_no == -1) {
		dvfs_destroy_inode(node);
		return NULL;
	}

	return node;
}

static int ext2fuse_iterate(struct inode *next, struct inode *parent, struct dir_ctx *ctx) {
	char buf[512];
	int res = 0;
	struct fuse_req_embox *req;
	struct task *task;
	struct fuse_dirent *dirent;
	struct ext2fuse_data *data;
	int idx = (int) ctx->fs_ctx;
	size_t i = 0;
	size_t offset = 0;

	if (NULL == (req = fuse_req_alloc())) {
		return -1;
	}
	ext2fuse_fill_req(req, parent, buf);
	task = fuse_in();
	ext2fuse_ops->readdir((fuse_req_t) req, parent->i_no, 512, 0, req->fi);
	fuse_out(task);

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

	ext2fuse_fill_req(req, next, NULL);
	task = fuse_in();
	ext2fuse_ops->lookup((fuse_req_t) req, parent->i_no, dirent->name);
	fuse_out(task);
	// If not found
	if (next->i_no < 0) {
		res = -1;
	}

out:
	fuse_req_free(req);
	return res;
}

static int ext2fuse_create(struct inode *i_new, struct inode *i_dir, int mode) {
	struct fuse_req_embox *req;
	struct task *task;

	if (NULL == (req = fuse_req_alloc())) {
		return 0;
	}

	ext2fuse_fill_req(req, i_new, NULL);
	task = fuse_in();
	ext2fuse_ops->create((fuse_req_t) req, i_dir->i_no, i_new->i_dentry->name, mode, req->fi);
	fuse_out(task);
	fuse_req_free(req);

	return 0;
}

static int ext2fuse_pathname(struct inode *inode, char *buf, int flags) {
	struct ext2fuse_data *data = inode->i_data;

	strcpy(buf, data->name);

	return 0;
}

static int ext2fuse_mount_end(struct super_block *sb) {
	return 0;
}

static int ext2fuse_destroy_inode(struct inode *inode) {
	return 0;
}

int ext2fuse_umount_begin(struct super_block *sb) {
	// TODO kill task and thread
	struct task *task;

	task = fuse_in();
	ext2fuse_ops->destroy(NULL);
	fuse_out(task);

	return 0;
}

struct super_block_operations ext2fuse_sbops = {
	.open_idesc = dvfs_file_open_idesc,
	.destroy_inode = ext2fuse_destroy_inode,
	.umount_begin = ext2fuse_umount_begin
};

struct inode_operations ext2fuse_iops = {
	.lookup   = ext2fuse_lookup,
	.iterate  = ext2fuse_iterate,
	.pathname = ext2fuse_pathname,
	.create = ext2fuse_create
};

struct file_operations ext2fuse_fops = {
	.open = ext2fuse_open,
	.close = ext2fuse_close,
	.read  = ext2fuse_read,
	.write  = ext2fuse_write
};

extern void init_ext2_stuff();

static void *ext2fuse_mount_task(void *arg) {
	init_ext2_stuff();
	ext2fuse_ops = ext2fs_register();
	// SHOULD BE ext2fuse_ops->init(bdev_file->f_dentry->name);
	ext2fuse_ops->init("/dev/hda");

	*(int*) arg = 0;
	while(1) {
		sleep(0);
	}

	/* UNREACHABLE */
	return NULL;
}

static void * stub_run(void *arg) {
	while (1) {
		sleep(0);
	}
	return NULL;
}

static int ext2fuse_fill_sb(struct super_block *sb, struct file *bdev_file) {
	int res;
	int flag = 1;

	sb->sb_iops = &ext2fuse_iops;
	sb->sb_fops = &ext2fuse_fops;
	sb->sb_ops  = &ext2fuse_sbops;
	sb->bdev = NULL;

	res = new_task("ext2fs_mount", ext2fuse_mount_task, &flag);
	mount_task = get_mount_task();
	stub_thread = thread_create(THREAD_FLAG_NOTASK | THREAD_FLAG_SUSPENDED, stub_run, NULL);

	if (res) {
		return res;
	}
	while (flag) {
		sleep(0);
	}
	if (bdev_file) {
		return -1;
	}

	return 0;
}

static struct dumb_fs_driver ext2fuse_dumb_driver = {
	.name      = "ext2fuse",
	.fill_sb   = ext2fuse_fill_sb,
	.mount_end = ext2fuse_mount_end,
};

ARRAY_SPREAD_DECLARE(struct dumb_fs_driver *, dumb_drv_tab);
ARRAY_SPREAD_ADD(dumb_drv_tab, &ext2fuse_dumb_driver);
