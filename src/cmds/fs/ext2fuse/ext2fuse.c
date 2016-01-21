/**
 * @file ext2fuse.c
 * @brief ext2fuse support
 * @author Alexander Kalmuk
 * @version 0.1
 * @date 2016-01-20
 */

#include <unistd.h>

#include <kernel/task.h>
#include <kernel/thread.h>

#define _FILE_OFFSET_BITS 64
#define FUSE_USE_VERSION 25

#include <fuse_lowlevel.h>

#include <fs/fuse_module.h>
#include <fs/fuse_driver.h>

extern void init_ext2_stuff();
extern struct fuse_lowlevel_ops *ext2fs_register(void);

static struct fuse_lowlevel_ops *ext2fuse_ops;

extern struct fuse_sb_priv_data ext2fuse_sb_priv_data;

static void * stub_run(void *arg) {
	while (1) {
		sleep(0);
	}
	return NULL;
}

extern void fuse_ext2_getxattr(fuse_req_t req, fuse_ino_t ino, const char *name,
            size_t size);
extern void fuse_ext2_setxattr(fuse_req_t req, fuse_ino_t ino, const char *name,
              const char *value, size_t size, int flags);

int main(int argc, char *argv[]) {
	init_ext2_stuff();
	ext2fuse_ops = ext2fs_register();
	ext2fuse_ops->getxattr = fuse_ext2_getxattr;
	ext2fuse_ops->setxattr = fuse_ext2_setxattr;

	ext2fuse_sb_priv_data.fuse_lowlevel_ops = ext2fuse_ops;
	ext2fuse_sb_priv_data.fuse_task = task_self();
	ext2fuse_sb_priv_data.stub_thread = thread_create(THREAD_FLAG_NOTASK | THREAD_FLAG_SUSPENDED, stub_run, NULL);

	ext2fuse_ops->init(argv[1]);


	while(1) {
		sleep(0);
	}

	/* UNREACHABLE */
	return 0;
}

FUSE_MODULE_DEF("ext2fuse", "ext2fuse");
