/**
 * @file
 * @brief Embox specific implementation of FUSE
 *
 * @date 16.11.2015
 * @author Alexander Kalmuk
 */

// Needed by fuse_common.h
#define _FILE_OFFSET_BITS 64

// it is from fuse-ext2fs.c
#define FUSE_USE_VERSION 25
#include <fuse_lowlevel.h>
#include <fuse_opt.h>

#include <stdio.h>

#define EMBOX_FUSE_NIY() printf("EMBOX_FUSE_NIY: %s\n", __func__)

const struct fuse_ctx *fuse_req_ctx(fuse_req_t req) {
	EMBOX_FUSE_NIY();
	return NULL;
}

	/* REPLY */
int fuse_reply_err(fuse_req_t req, int err) {
	EMBOX_FUSE_NIY();
	return 0;
}

int fuse_reply_attr(fuse_req_t req, const struct stat *attr,
            double attr_timeout) {
	EMBOX_FUSE_NIY();
	return 0;
}

int fuse_reply_create(fuse_req_t req, const struct fuse_entry_param *e,
              const struct fuse_file_info *fi) {
	EMBOX_FUSE_NIY();
	return 0;
}

int fuse_reply_entry(fuse_req_t req, const struct fuse_entry_param *e) {
	EMBOX_FUSE_NIY();
	return 0;
}

int fuse_reply_open(fuse_req_t req, const struct fuse_file_info *fi) {
	EMBOX_FUSE_NIY();
	return 0;
}

int fuse_reply_write(fuse_req_t req, size_t count) {
	EMBOX_FUSE_NIY();
	return 0;
}

int fuse_reply_buf(fuse_req_t req, const char *buf, size_t size) {
	EMBOX_FUSE_NIY();
	return 0;
}

int fuse_reply_statfs(fuse_req_t req, const struct statvfs *stbuf) {
	EMBOX_FUSE_NIY();
	return 0;
}

int fuse_reply_readlink(fuse_req_t req, const char *linkname) {
	EMBOX_FUSE_NIY();
	return 0;
}


	/* FUSE_OPT */
int fuse_opt_add_arg(struct fuse_args *args, const char *arg) {
	EMBOX_FUSE_NIY();
	return 0;
}

void fuse_opt_free_args(struct fuse_args *args) {
	EMBOX_FUSE_NIY();
}


	/* SIGNAL */
int fuse_set_signal_handlers(struct fuse_session *se) {
	EMBOX_FUSE_NIY();
	return 0;
}

void fuse_remove_signal_handlers(struct fuse_session *se) {
	EMBOX_FUSE_NIY();
}


	/* SESSION */
void fuse_session_add_chan(struct fuse_session *se, struct fuse_chan *ch) {
	EMBOX_FUSE_NIY();
}

int fuse_session_loop_mt(struct fuse_session *se) {
	EMBOX_FUSE_NIY();
	return 0;
}

void fuse_session_destroy(struct fuse_session *se) {
	EMBOX_FUSE_NIY();
}

int fuse_session_loop(struct fuse_session *se) {
	EMBOX_FUSE_NIY();
	return 0;
}

	/* MOUNT/UMOUNT  */
int fuse_mount_compat25(const char *mountpoint, struct fuse_args *args) {
	EMBOX_FUSE_NIY();
	return 0;
}

void fuse_unmount_compat22(const char *mountpoint) {
	EMBOX_FUSE_NIY();
}


	/* DIRENT */
size_t fuse_dirent_size(size_t namelen) {
	EMBOX_FUSE_NIY();
	return 0;
}

char *fuse_add_dirent(char *buf, const char *name, const struct stat *stbuf,
              off_t off) {
	EMBOX_FUSE_NIY();
	return NULL;
}

struct fuse_chan *fuse_kern_chan_new(int fd) {
	EMBOX_FUSE_NIY();
	return NULL;
}

struct fuse_session *fuse_lowlevel_new_compat25(struct fuse_args *args,
                const struct fuse_lowlevel_ops_compat25 *op,
                size_t op_size, void *userdata) {
	EMBOX_FUSE_NIY();
	return NULL;
}
