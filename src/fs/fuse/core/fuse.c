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
#include <fuse_kernel.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fs/dvfs.h>


#include "fuse_req_alloc.h"

#define EMBOX_FUSE_NIY() printf("EMBOX_FUSE_NIY: %s\n", __func__)


	/* EMBOX SPECIFIC */
struct fuse_req_embox *fuse_req_alloc(void) {
	return malloc(sizeof(struct fuse_req_embox));
}

void fuse_req_free(struct fuse_req_embox *req) {
	free(req);
}

	/* CTX */
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
	// XXX fi is currently unused
	fuse_reply_entry(req, e);
	return 0;
}

int fuse_reply_entry(fuse_req_t req, const struct fuse_entry_param *e) {
	struct fuse_req_embox *emreq = (struct fuse_req_embox *) req;
	struct inode *node = emreq->node;

	assert(node);

	node->i_no = e->ino;
	node->length = e->attr.st_size;
	node->i_mode = e->attr.st_mode;
	node->i_group_id = e->attr.st_gid;
	node->i_owner_id = e->attr.st_uid;

	return 0;
}

int fuse_reply_open(fuse_req_t req, const struct fuse_file_info *fi) {
	struct fuse_req_embox *emreq;
	struct fuse_file_info *node_fi;

	emreq = (struct fuse_req_embox *) req;
	node_fi = emreq->fi;
	node_fi->fh = fi->fh;

	return 0;
}

int fuse_reply_write(fuse_req_t req, size_t count) {
	struct fuse_req_embox *emreq = (struct fuse_req_embox *) req;
	emreq->buf_size = count;
	EMBOX_FUSE_NIY();
	return 0;
}

int fuse_reply_buf(fuse_req_t req, const char *buf, size_t size) {
	struct fuse_req_embox *emreq = (struct fuse_req_embox *) req;
	memcpy(emreq->buf, buf, size);
	emreq->buf_size = size;
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
	return FUSE_DIRENT_ALIGN(FUSE_NAME_OFFSET + namelen);
}

char *fuse_add_dirent(char *buf, const char *name, const struct stat *stbuf,
              off_t off) {

	unsigned namelen = strlen(name);
	unsigned entlen = FUSE_NAME_OFFSET + namelen;
	unsigned entsize = fuse_dirent_size(namelen);
	unsigned padlen = entsize - entlen;
	struct fuse_dirent *dirent = (struct fuse_dirent *) buf;

	dirent->ino = stbuf->st_ino;
	dirent->off = off;
	dirent->namelen = namelen;
	dirent->type = (stbuf->st_mode & 0170000) >> 12;
	strncpy(dirent->name, name, namelen);
	if (padlen)
	    memset(buf + entlen, 0, padlen);

	return buf + entsize;
}

