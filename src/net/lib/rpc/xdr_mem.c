/**
 * @file
 * @brief Realization of the eXternal Data Representation
 * @details XDR: External Data Representation Standard, RFC 1832
 *
 * @date 21.05.12
 * @author Ilia Vaprol
 */

#include <net/lib/rpc/xdr.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <util/binalign.h>

static const struct xdr_ops xdrmem_ops;

static xdr_unit_t encode_unit(xdr_unit_t u) { return htonl(u); }
static xdr_unit_t decode_unit(xdr_unit_t u) { return ntohl(u); }

void xdrmem_create(struct xdr *xs, char *addr,
		size_t size, enum xdr_op oper) {
	assert((xs != NULL) && (addr != NULL));

	xs->oper = oper;
	xs->ops = &xdrmem_ops;
	assert(binalign_check_bound((uintptr_t)addr, sizeof(xdr_unit_t)));
	xs->extra.mem.curr = xs->extra.mem.buff = addr;
	xs->extra.mem.left = size;
}

static void xdrmem_destroy(struct xdr *xs) {
	assert(xs != NULL);
}

static int xdrmem_getunit(struct xdr *xs, xdr_unit_t *to) {
	assert((xs != NULL) && (to != NULL));

	if (xs->extra.mem.left < BYTES_PER_XDR_UNIT) {
		return XDR_FAILURE;
	}

	*to = decode_unit(*(xdr_unit_t *)xs->extra.mem.curr);
	xs->extra.mem.left -= BYTES_PER_XDR_UNIT;
	xs->extra.mem.curr += BYTES_PER_XDR_UNIT;

	return XDR_SUCCESS;
}

static int xdrmem_putunit(struct xdr *xs, const xdr_unit_t *from) {
	assert((xs != NULL) && (from != NULL));

	if (xs->extra.mem.left < BYTES_PER_XDR_UNIT) {
		return XDR_FAILURE;
	}

	*(xdr_unit_t *)xs->extra.mem.curr = encode_unit(*from);
	xs->extra.mem.left -= BYTES_PER_XDR_UNIT;
	xs->extra.mem.curr += BYTES_PER_XDR_UNIT;

	return XDR_SUCCESS;
}

static int xdrmem_getbytes(struct xdr *xs, char *to, size_t size) {
	assert((xs != NULL) && ((to != NULL) || (size == 0)));

	if (xs->extra.mem.left < size) {
		return XDR_FAILURE;
	}

	memcpy(to, xs->extra.mem.curr, size);
	xs->extra.mem.left -= size;
	xs->extra.mem.curr += size;

	return XDR_SUCCESS;
}

static int xdrmem_putbytes(struct xdr *xs, const char *from, size_t size) {
	assert((xs != NULL) && ((from != NULL) || (size == 0)));

	if (xs->extra.mem.left < size) {
		return XDR_FAILURE;
	}

	memcpy(xs->extra.mem.curr, from, size);
	xs->extra.mem.left -= size;
	xs->extra.mem.curr += size;

	return XDR_SUCCESS;
}

static size_t xdrmem_getpos(struct xdr *xs) {
	assert(xs != NULL);

	return (size_t)xs->extra.mem.curr - (size_t)xs->extra.mem.buff;
}

static int xdrmem_setpos(struct xdr *xs, size_t pos) {
	assert(xs != NULL);

	if (xs->extra.mem.buff + pos > xs->extra.mem.curr + xs->extra.mem.left) {
		return XDR_FAILURE;
	}

	xs->extra.mem.left = xs->extra.mem.curr + xs->extra.mem.left - (xs->extra.mem.buff + pos);
	xs->extra.mem.curr = xs->extra.mem.buff + pos;

	return XDR_SUCCESS;
}

static const struct xdr_ops xdrmem_ops = {
		.destroy = &xdrmem_destroy,
		.getunit = &xdrmem_getunit,
		.putunit = &xdrmem_putunit,
		.getbytes = &xdrmem_getbytes,
		.putbytes = &xdrmem_putbytes,
		.getpos = &xdrmem_getpos,
		.setpos = &xdrmem_setpos,
};
