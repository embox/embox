/**
 * @file
 * @brief Realization of the eXternal Data Representation
 * @details XDR: External Data Representation Standard, RFC 1832
 *
 * @date 05.06.12
 * @author Ilia Vaprol
 */

#include <net/lib/rpc/xdr.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/math.h>

static const struct xdr_ops xdrrec_ops;

static xdr_unit_t encode_unit(xdr_unit_t u) { return htonl(u); }
static xdr_unit_t decode_unit(xdr_unit_t u) { return ntohl(u); }

#define LAST_DATA     1
#define NOT_LAST_DATA 0
#define BUFF_SEND_SZ  1024
#define BUFF_RECV_SZ  1024

static int flush_data(struct xdr *xs, char is_last);
static int prepare_data(struct xdr *xs, uint32_t necessary);

void xdrrec_create(struct xdr *xs, size_t sendsz, size_t recvsz,
		char *handle, xdrrec_hnd_t readit, xdrrec_hnd_t writeit) {
	char *buff;

	assert((xs != NULL) && (handle != NULL)
			&& (readit != NULL) && (writeit != NULL));

	if (sendsz == 0) { sendsz = BUFF_SEND_SZ; }
	if (recvsz == 0) { recvsz = BUFF_RECV_SZ; }

	sendsz += BYTES_PER_XDR_UNIT + (BYTES_PER_XDR_UNIT
			- sendsz % BYTES_PER_XDR_UNIT) % BYTES_PER_XDR_UNIT;
	recvsz += BYTES_PER_XDR_UNIT + (BYTES_PER_XDR_UNIT
			- sendsz % BYTES_PER_XDR_UNIT) % BYTES_PER_XDR_UNIT;
	assert(sendsz % BYTES_PER_XDR_UNIT == 0); // TODO debug
	assert(recvsz % BYTES_PER_XDR_UNIT == 0); // TODO debug

	buff = (char *)malloc(sendsz + recvsz);
	assert(buff != NULL);

	xs->ops = &xdrrec_ops;
	xs->extra.rec.handle = handle;

	xs->extra.rec.in_hnd = readit;
	xs->extra.rec.in_base = xs->extra.rec.in_curr = buff;
	xs->extra.rec.in_boundry = xs->extra.rec.in_base + recvsz;
	xs->extra.rec.in_prep = xs->extra.rec.in_left = 0;
	xs->extra.rec.in_last = LAST_DATA;

	xs->extra.rec.out_hnd = writeit;
	xs->extra.rec.out_base = xs->extra.rec.out_hdr = buff + recvsz;
	xs->extra.rec.out_curr = xs->extra.rec.out_hdr + sizeof(union xdrrec_hdr);
	xs->extra.rec.out_boundry = xs->extra.rec.out_base + sendsz;
}

static void xdrrec_destroy(struct xdr *xs) {
	assert(xs != NULL);

	free(xs->extra.rec.in_base);
}

static int xdrrec_getunit(struct xdr *xs, xdr_unit_t *to) {
	assert((xs != NULL) && (to != NULL));

	if (!prepare_data(xs, BYTES_PER_XDR_UNIT)) {
		return XDR_FAILURE;
	}

	*to = decode_unit(*(xdr_unit_t *)xs->extra.rec.in_curr);
	xs->extra.rec.in_curr += BYTES_PER_XDR_UNIT;
	xs->extra.rec.in_prep -= BYTES_PER_XDR_UNIT;

	return XDR_SUCCESS;
}

static int xdrrec_putunit(struct xdr *xs, const xdr_unit_t *from) {
	assert((xs != NULL) && (from != NULL));

	if ((xs->extra.rec.out_boundry - xs->extra.rec.out_curr < BYTES_PER_XDR_UNIT)
			&& !flush_data(xs, NOT_LAST_DATA)) {
		return XDR_FAILURE;
	}

	*(xdr_unit_t *)xs->extra.rec.out_curr = encode_unit(*from);
	xs->extra.rec.out_curr += BYTES_PER_XDR_UNIT;

	return XDR_SUCCESS;
}

static int xdrrec_getbytes(struct xdr *xs, char *to, size_t size) {
	assert((xs != NULL) && ((to != NULL) || (size == 0)));

	if (!prepare_data(xs, size)) {
		return XDR_FAILURE;
	}

	assert(size <= xs->extra.rec.in_prep);
	memcpy(to, xs->extra.rec.in_curr, size);
	xs->extra.rec.in_curr += size;
	xs->extra.rec.in_prep -= size;

	return XDR_SUCCESS;
}

static int xdrrec_putbytes(struct xdr *xs, const char *from, size_t size) {
	size_t bytes;

	assert((xs != NULL) && ((from != NULL) || (size == 0)));

	while (size > 0) {
		assert(xs->extra.rec.out_curr <= xs->extra.rec.out_boundry);
		if ((xs->extra.rec.out_boundry == xs->extra.rec.out_curr) &&
				!flush_data(xs, NOT_LAST_DATA)) {
			return XDR_FAILURE;
		}
		bytes = min(size, xs->extra.rec.out_boundry - xs->extra.rec.out_curr);
		memcpy(xs->extra.rec.out_curr, from, bytes);
		xs->extra.rec.out_curr += bytes;
		size -= bytes;
		from += bytes;
	}

	return XDR_SUCCESS;
}

static size_t xdrrec_getpos(struct xdr *xs) {
	assert(xs != NULL);

	return 0;
}

static int xdrrec_setpos(struct xdr *xs, size_t pos) {
	assert(xs != NULL);

	return XDR_FAILURE;
}

int xdrrec_endofrecord(struct xdr *xs, int sendnow) {
	union xdrrec_hdr hdr;

	if (sendnow) {
		return flush_data(xs, LAST_DATA);
	}

	hdr.h.len = xs->extra.rec.out_curr - xs->extra.rec.out_hdr - sizeof(union xdrrec_hdr);
	hdr.h.is_last = LAST_DATA;
	*(xdr_unit_t *)xs->extra.rec.out_hdr = encode_unit(hdr.unit);

	xs->extra.rec.out_hdr = xs->extra.rec.out_curr;
	xs->extra.rec.out_curr += sizeof(union xdrrec_hdr);

	return XDR_SUCCESS;
}

static int flush_data(struct xdr *xs, char is_last) {
	int bytes;
	union xdrrec_hdr hdr;

	hdr.h.len = xs->extra.rec.out_curr - xs->extra.rec.out_hdr - sizeof hdr;
	hdr.h.is_last = is_last ? 1 : 0;
	*(xdr_unit_t *)xs->extra.rec.out_hdr = encode_unit(hdr.unit);

	bytes = xs->extra.rec.out_curr - xs->extra.rec.out_base;
	if ((*xs->extra.rec.out_hnd)(xs->extra.rec.handle,
			xs->extra.rec.out_hdr, bytes) != bytes) {
		return XDR_FAILURE;
	}

	xs->extra.rec.out_hdr = xs->extra.rec.out_base;
	xs->extra.rec.out_curr = xs->extra.rec.out_hdr + sizeof hdr;

	return XDR_SUCCESS;
}

static int prepare_data(struct xdr *xs, uint32_t necessary) {
	int res, bytes;
	uint32_t len;

	/* Can we give required amount of data? */
	if (necessary <= xs->extra.rec.in_prep) {
		return XDR_SUCCESS;
	}

	/* How much bytes left in current message? */
	if (xs->extra.rec.in_left != 0) {
		len = xs->extra.rec.in_left;
	}
	else {
		union xdrrec_hdr hdr;
		if ((xs->extra.rec.in_prep != 0)
				&& xs->extra.rec.in_last) {
			return XDR_FAILURE;
		}
		if ((*xs->extra.rec.in_hnd)(xs->extra.rec.handle,
				(char *)&hdr, sizeof hdr) != sizeof hdr) {
			return XDR_FAILURE;
		}
		hdr.unit = decode_unit(hdr.unit);
		xs->extra.rec.in_last = hdr.h.is_last;
		len = hdr.h.len;
	}

	/* Prepare memory for in-coming bytes */
	assert(xs->extra.rec.in_curr + xs->extra.rec.in_prep <= xs->extra.rec.in_boundry);
	if (xs->extra.rec.in_boundry - xs->extra.rec.in_curr - xs->extra.rec.in_prep < len) {
		memmove(xs->extra.rec.in_base, xs->extra.rec.in_curr, xs->extra.rec.in_prep);
		xs->extra.rec.in_curr = xs->extra.rec.in_base;
	}

	/* How much bytes we will try receive ? */
	bytes = min(len, xs->extra.rec.in_boundry - xs->extra.rec.in_curr
			- xs->extra.rec.in_prep);

	/* Receiving of data */
	res = (*xs->extra.rec.in_hnd)(xs->extra.rec.handle,
			xs->extra.rec.in_curr + xs->extra.rec.in_prep, bytes);
	if (res <= 0) {
		return XDR_FAILURE;
	}

	xs->extra.rec.in_prep += res;
	assert(res <= len);
	xs->extra.rec.in_left = len - res;

	return necessary <= xs->extra.rec.in_prep ? XDR_SUCCESS : XDR_FAILURE;
}

static const struct xdr_ops xdrrec_ops = {
		.destroy = &xdrrec_destroy,
		.getunit = &xdrrec_getunit,
		.putunit = &xdrrec_putunit,
		.getbytes = &xdrrec_getbytes,
		.putbytes = &xdrrec_putbytes,
		.getpos = &xdrrec_getpos,
		.setpos = &xdrrec_setpos,
};
