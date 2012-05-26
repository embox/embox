/**
 * @file
 * @brief Realization of the eXternal Data Representation
 * @date 21.05.12
 * @author Ilia Vaprol
 */

#include <net/rpc/xdr.h>
#include <net/in.h>
#include <string.h>
#include <assert.h>

static const struct xdr_ops xdrmem_ops = {
		.x_destroy = &xdrmem_destroy,
		.x_getint = &xdrmem_getint,
		.x_putint = &xdrmem_putint,
		.x_getbytes = &xdrmem_getbytes,
		.x_putbytes = &xdrmem_putbytes,
		.x_getpos = &xdrmem_getpos,
		.x_setpos = &xdrmem_setpos,
		.x_inline = &xdrmem_inline
};

static inline long encode_int(int i) { return (int)htonl((uint32_t)i); }
static inline long decode_int(int i) { return (int)ntohl((uint32_t)i); }

void xdrmem_create(struct xdr *xs, char *addr,
		unsigned int size, enum xdr_op oper) {
	assert((xs != NULL) && (addr != NULL));

	xs->oper = oper;
	xs->ops = &xdrmem_ops;
	xs->curr = xs->buff = addr;
	xs->left = size;
}

static void xdrmem_destroy(struct xdr *xs) {
	assert(xs != NULL);
}

static int xdrmem_getint(struct xdr *xs, int *to) {
	assert((xs != NULL) && (to != NULL));

	if (xs->left < sizeof(int)) {
		return XDR_FAILURE;
	}

	*to = decode_int(*(int *)xs->curr);
	xs->left -= sizeof(int);
	xs->curr += sizeof(int);

	return XDR_SUCCESS;
}

static int xdrmem_putint(struct xdr *xs, const int *from) {
	assert((xs != NULL) && (from != NULL));

	if (xs->left < sizeof(int)) {
		return XDR_FAILURE;
	}

	*(int *)xs->curr = encode_int(*from);
	xs->left -= sizeof(int);
	xs->curr += sizeof(int);

	return XDR_SUCCESS;
}

static int xdrmem_getbytes(struct xdr *xs, char *to, unsigned int size) {
	assert((xs != NULL) && (to != NULL));

	if (xs->left < size) {
		return XDR_FAILURE;
	}

	memcpy(to, xs->curr, size);
	xs->left -= size;
	xs->curr += size;

	return XDR_SUCCESS;
}

static int xdrmem_putbytes(struct xdr *xs, const char *from, unsigned int size) {
	assert((xs != NULL) && (from != NULL));

	if (xs->left < size) {
		return XDR_FAILURE;
	}

	memcpy(xs->curr, from, size);
	xs->left -= size;
	xs->curr += size;

	return XDR_SUCCESS;
}

static unsigned int xdrmem_getpos(struct xdr *xs) {
	assert(xs != NULL);

	return (unsigned int)(xs->curr - xs->buff);
}

static int xdrmem_setpos(struct xdr *xs, unsigned int pos) {
	assert(xs != NULL);

	if (xs->buff + pos >= xs->curr + xs->left) {
		xs->left = 0;
	}
	else {
		xs->left = xs->curr + xs->left - (xs->buff + pos);
		xs->curr = xs->buff + pos;
	}

	return XDR_SUCCESS;
}

static long * xdrmem_inline(struct xdr *xs, unsigned int len) {
	long *ptr;

	assert(xs != NULL);

	if (xs->left < len) {
		return NULL;
	}

	ptr = xs->curr;
	xs->left -= len;
	xs->curr += len;

	return ptr;
}
