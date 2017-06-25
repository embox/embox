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

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>

//#include <fs/dvfs.h>


#define EMBOX_FUSE_NIY() printf("EMBOX_FUSE_NIY: %s\n", __func__)


struct fuse_session {
	struct fuse_session_ops op;

	int (*receive_buf)(struct fuse_session *se, struct fuse_buf *buf,
			   struct fuse_chan **chp);

	void (*process_buf)(void *data, const struct fuse_buf *buf,
			    struct fuse_chan *ch);

	void *data;

	volatile int exited;

	struct fuse_chan *ch;
};


struct fuse_chan {
	struct fuse_chan_ops op;
	struct fuse_session *se;
	int fd;
	size_t bufsize;
	void *data;
	int compat;
};

static struct fuse_chan *fuse_chan_new_common(struct fuse_chan_ops *op, int fd,
		size_t bufsize, void *data, int compat) {
	struct fuse_chan *ch = (struct fuse_chan *) malloc(sizeof(*ch));
	if (ch == NULL) {
		fprintf(stderr, "fuse: failed to allocate channel\n");
		return NULL;
	}

	memset(ch, 0, sizeof(*ch));
	ch->op = *op;
	ch->fd = fd;
	ch->bufsize = bufsize;
	ch->data = data;
	ch->compat = compat;

	return ch;
}


struct fuse_chan *fuse_chan_new_compat24(struct fuse_chan_ops_compat24 *op,
		int fd, size_t bufsize, void *data) {
	return fuse_chan_new_common((struct fuse_chan_ops *) op, fd, bufsize, data,
			24);
}

static int fuse_kern_chan_receive(struct fuse_chan *chp, char *buf,
		size_t size) {
	struct fuse_chan *ch = chp;
	int err;
	ssize_t res;
	struct fuse_session *se = fuse_chan_session(ch);
	assert(se != NULL);

	restart: res = read(fuse_chan_fd(ch), buf, size);
	err = errno;

	if (fuse_session_exited(se))
		return 0;
	if (res == -1) {
		/* ENOENT means the operation was interrupted, it's safe
		 to restart */
		if (err == ENOENT)
			goto restart;

		if (err == ENODEV) {
			//fuse_session_exit(se);
			return 0;
		}
		/* Errors occurring during normal operation: EINTR (read
		 interrupted), EAGAIN (nonblocking I/O), ENODEV (filesystem
		 umounted) */
		if (err != EINTR && err != EAGAIN)
			perror("fuse: reading device");
		return -err;
	}

	return res;
}


struct fuse_chan *fuse_kern_chan_new(int fd) {
	const struct fuse_chan_ops fuse_chan_ops = {
		.receive = fuse_kern_chan_receive,
	};

	return fuse_chan_new((struct fuse_chan_ops *)&fuse_chan_ops, fd, 0x1000, NULL);
}


struct fuse_session *fuse_session_new(struct fuse_session_ops *op, void *data) {
	struct fuse_session *se;

	se = (struct fuse_session *) malloc(sizeof(*se));
	if (!se) {
		return NULL;
	}

	memset(se, 0, sizeof(*se));
	se->op = *op;
	se->data = data;

	return se;
}

static void session_process(void *data, const char *buf, size_t len,
			 struct fuse_chan *ch) {
}

static struct fuse_session_ops fuse_session_ops = {
	.process = session_process,

};

struct fuse_session *fuse_lowlevel_new_compat25(struct fuse_args *args,
                const struct fuse_lowlevel_ops_compat25 *op,
                size_t op_size, void *userdata) {
	struct fuse_session *fuse_session;

	fuse_session = fuse_session_new(&fuse_session_ops, userdata);
	if (!fuse_session) {
		return NULL;
	}
	op->init(userdata);

	return fuse_session;
}

/**/
void fuse_session_add_chan(struct fuse_session *se, struct fuse_chan *ch) {
	assert(se->ch == NULL);
	assert(ch->se == NULL);
	se->ch = ch;
	ch->se = se;
}
#if 0
void fuse_session_exit(struct fuse_session *se) {
	if (se->op.exit)
		se->op.exit(se->data, 1);
	se->exited = 1;
}
#endif
void fuse_session_reset(struct fuse_session *se) {
	if (se->op.exit)
		se->op.exit(se->data, 0);
	se->exited = 0;
}

int fuse_session_exited(struct fuse_session *se) {
	if (se->op.exited)
		return se->op.exited(se->data);
	else
		return se->exited;
}

int fuse_session_loop_mt(struct fuse_session *se) {
	EMBOX_FUSE_NIY();
	return 0;
}

int fuse_chan_fd(struct fuse_chan *ch) {
	return ch->fd;
}

size_t fuse_chan_bufsize(struct fuse_chan *ch) {
	return ch->bufsize;
}

void *fuse_chan_data(struct fuse_chan *ch) {
	return ch->data;
}

struct fuse_session *fuse_chan_session(struct fuse_chan *ch) {
	return ch->se;
}

struct fuse_chan *fuse_session_next_chan(struct fuse_session *se,
		struct fuse_chan *ch) {
	assert(ch == NULL || ch == se->ch);
	if (ch == NULL)
		return se->ch;
	else
		return NULL;
}

void fuse_session_process(struct fuse_session *se, const char *buf, size_t len,
		struct fuse_chan *ch) {
	se->op.process(se->data, buf, len, ch);
}

void fuse_session_process_buf(struct fuse_session *se,
		const struct fuse_buf *buf, struct fuse_chan *ch) {
	if (se->process_buf) {
		se->process_buf(se->data, buf, ch);
	} else {
		assert(!(buf->flags & FUSE_BUF_IS_FD));
		fuse_session_process(se->data, buf->mem, buf->size, ch);
	}
}


int fuse_chan_recv(struct fuse_chan **chp, char *buf, size_t size) {
	struct fuse_chan *ch = *chp;

	return ((struct fuse_chan_ops_compat24 *) &ch->op)->receive(ch, buf,
				size);
}

int fuse_session_receive_buf(struct fuse_session *se, struct fuse_buf *buf,
		struct fuse_chan **chp) {
	int res;

	if (se->receive_buf) {
		res = se->receive_buf(se, buf, chp);
	} else {
		res = fuse_chan_recv(chp, buf->mem, buf->size);
		if (res > 0)
			buf->size = res;
	}

	return res;
}

int fuse_session_loop(struct fuse_session *se) {
	int res = 0;
	struct fuse_chan *ch = fuse_session_next_chan(se, NULL);
	size_t bufsize = fuse_chan_bufsize(ch);
	char *buf = (char *) malloc(bufsize);
	if (!buf) {
		fprintf(stderr, "fuse: failed to allocate read buffer\n");
		return -1;
	}

	while (!fuse_session_exited(se)) {
		struct fuse_chan *tmpch = ch;
		struct fuse_buf fbuf = { .mem = buf, .size = bufsize, };

		res = fuse_session_receive_buf(se, &fbuf, &tmpch);

		if (res == -EINTR)
			continue;
		if (res <= 0)
			break;

		fuse_session_process_buf(se, &fbuf, tmpch);
	}

	free(buf);
	fuse_session_reset(se);
	return res < 0 ? -1 : 0;
}

void fuse_session_destroy(struct fuse_session *se) {
	if (se->op.destroy)
		se->op.destroy(se->data);
	if (se->ch != NULL)
		fuse_chan_destroy(se->ch);

	assert(0);
}

void fuse_chan_destroy(struct fuse_chan *ch) {
	fuse_session_remove_chan(ch);
	if (ch->op.destroy)
		ch->op.destroy(ch);
	assert(0);
}

void fuse_session_remove_chan(struct fuse_chan *ch) {
	struct fuse_session *se = ch->se;
	if (se) {
		assert(se->ch == ch);
		se->ch = NULL;
		ch->se = NULL;
	}
}
