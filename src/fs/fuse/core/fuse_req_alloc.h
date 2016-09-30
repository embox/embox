/*
 * @file
 *
 * @date 24.11.2015
 * @author Alexander Kalmuk
 */

#ifndef FUSE_REQ_ALLOC_H_
#define FUSE_REQ_ALLOC_H_

#include <stddef.h>

struct inode;
struct fuse_file_info;

struct fuse_req_embox {
	struct inode *node;
	struct fuse_file_info *fi;
	void *buf;
	size_t buf_size;
};

extern struct fuse_req_embox *fuse_req_alloc(void);
extern void fuse_req_free(struct fuse_req_embox *req);

#endif /* FUSE_REQ_ALLOC_H_ */
