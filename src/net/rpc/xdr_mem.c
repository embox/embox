/**
 * @file
 * @brief Realization of the eXternal Data Representation
 * @date 21.05.12
 * @author Ilia Vaprol
 */

#include <net/rpc/xdr.h>
#include <lib/bits/byteswap.h>
#include <string.h>
#include <assert.h>

static const struct xdr_ops xdrmem_ops;

static xdr_unit_t encode_unit(xdr_unit_t u) { return __bswap_32(u); }
static xdr_unit_t decode_unit(xdr_unit_t u) { return __bswap_32(u); }

void xdrmem_create(struct xdr *xs, char *addr,
		size_t size, enum xdr_op oper) {
	assert((xs != NULL) && (addr != NULL));

	xs->oper = oper;
	xs->ops = &xdrmem_ops;
	xs->curr = xs->buff = addr;
	xs->left = size;
}

static void xdrmem_destroy(struct xdr *xs) {
	assert(xs != NULL);
}

static int xdrmem_getunit(struct xdr *xs, xdr_unit_t *to) {
	assert((xs != NULL) && (to != NULL));

	if (xs->left < BYTES_PER_XDR_UNIT) {
		return XDR_FAILURE;
	}

	*to = decode_unit(*(int *)xs->curr);
	xs->left -= BYTES_PER_XDR_UNIT;
	xs->curr += BYTES_PER_XDR_UNIT;

	return XDR_SUCCESS;
}

static int xdrmem_putunit(struct xdr *xs, const xdr_unit_t *from) {
	assert((xs != NULL) && (from != NULL));

	if (xs->left < BYTES_PER_XDR_UNIT) {
		return XDR_FAILURE;
	}

	*(int *)xs->curr = encode_unit(*from);
	xs->left -= BYTES_PER_XDR_UNIT;
	xs->curr += BYTES_PER_XDR_UNIT;

	return XDR_SUCCESS;
}

static int xdrmem_getbytes(struct xdr *xs, char *to, size_t size) {
	assert((xs != NULL) && (to != NULL));

	if (xs->left < size) {
		return XDR_FAILURE;
	}

	memcpy(to, xs->curr, size);
	xs->left -= size;
	xs->curr += size;

	return XDR_SUCCESS;
}

static int xdrmem_putbytes(struct xdr *xs, const char *from, size_t size) {
	assert((xs != NULL) && (from != NULL));

	if (xs->left < size) {
		return XDR_FAILURE;
	}

	memcpy(xs->curr, from, size);
	xs->left -= size;
	xs->curr += size;

	return XDR_SUCCESS;
}

static size_t xdrmem_getpos(struct xdr *xs) {
	assert(xs != NULL);

	return (size_t)(xs->curr - xs->buff);
}

static int xdrmem_setpos(struct xdr *xs, size_t pos) {
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

static char * xdrmem_inline(struct xdr *xs, size_t len) {
	char *ptr;

	assert(xs != NULL);

	if (xs->left < len) {
		return NULL;
	}

	ptr = xs->curr;
	xs->left -= len;
	xs->curr += len;

	return ptr;
}

static const struct xdr_ops xdrmem_ops = {
		.x_destroy = &xdrmem_destroy,
		.x_getunit = &xdrmem_getunit,
		.x_putunit = &xdrmem_putunit,
		.x_getbytes = &xdrmem_getbytes,
		.x_putbytes = &xdrmem_putbytes,
		.x_getpos = &xdrmem_getpos,
		.x_setpos = &xdrmem_setpos,
		.x_inline = &xdrmem_inline
};
