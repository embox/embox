/**
 * @file
 * @brief Realization of the eXternal Data Representation
 * @details XDR: External Data Representation Standard, RFC 1832
 * @date 21.05.12
 * @author Ilia Vaprol
 */

#include <stdlib.h>
#include <assert.h>
#include <net/rpc/xdr.h>
#include <string.h>
#include <stddef.h>

/* Useful routines */
static int xdr_getunit(struct xdr *xs, xdr_unit_t *to) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_getunit != NULL);

	return (*xs->ops->x_getunit)(xs, to);
}

static int xdr_putunit(struct xdr *xs, const xdr_unit_t *from) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_putunit != NULL);

	return (*xs->ops->x_putunit)(xs, from);
}

static int xdr_getbytes(struct xdr *xs, char *to, size_t size) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_getbytes != NULL);

	return (*xs->ops->x_getbytes)(xs, to, size);
}

static int xdr_putbytes(struct xdr *xs, const char *from, size_t size) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_putbytes != NULL);

	return (*xs->ops->x_putbytes)(xs, from, size);
}

void xdr_free(xdrproc_t proc, char *obj) {
	struct xdr tmp;

	assert(proc != NULL);

	tmp.oper = XDR_FREE;
	(*proc)(&tmp, obj);
}

int xdr_void(void) {
	return XDR_SUCCESS;
}

int xdr_int(struct xdr *xs, __s32 *ps32) {
	assert(xs != NULL);
	assert(sizeof *ps32 == BYTES_PER_XDR_UNIT);

	switch (xs->oper) {
	case XDR_DECODE:
		return xdr_getunit(xs, (xdr_unit_t *)ps32);
	case XDR_ENCODE:
		return xdr_putunit(xs, (xdr_unit_t *)ps32);
	case XDR_FREE:
		return XDR_SUCCESS;
	}

	return XDR_FAILURE; /* unknown operation */
}

int xdr_u_int(struct xdr *xs, __u32 *pu32) {
	assert(xs != NULL);
	assert(sizeof *pu32 == BYTES_PER_XDR_UNIT);

	switch (xs->oper) {
	case XDR_DECODE:
		return xdr_getunit(xs, (xdr_unit_t *)pu32);
	case XDR_ENCODE:
		return xdr_putunit(xs, (xdr_unit_t *)pu32);
	case XDR_FREE:
		return XDR_SUCCESS;
	}

	return XDR_FAILURE; /* unknown operation */
}

int xdr_opaque(struct xdr *xs, char *pc, size_t size) {
	size_t round_up;
	char trash[BYTES_PER_XDR_UNIT];

	assert((xs != NULL) && (pc != NULL));

	switch (xs->oper) {
	case XDR_DECODE:
		if (!xdr_getbytes(xs, pc, size)) {
			return XDR_FAILURE;
		}
		round_up = BYTES_PER_XDR_UNIT - size % BYTES_PER_XDR_UNIT;
		return xdr_getbytes(xs, trash, round_up); /* TODO remove trash from stream if error */
	case XDR_ENCODE:
		if (!xdr_putbytes(xs, pc, size)) {
			return XDR_FAILURE;
		}
		round_up = BYTES_PER_XDR_UNIT - size % BYTES_PER_XDR_UNIT;
		memset(trash, 0, round_up);
		return xdr_putbytes(xs, trash, round_up);
	case XDR_FREE:
		return XDR_SUCCESS;
	}

	return XDR_FAILURE;  /* unknown operation */
}

int xdr_string(struct xdr *xs, char **pstr, __u32 maxsize) {
	__u32 size;

	assert((xs != NULL) && (pstr != NULL));

	switch (xs->oper) {
	case XDR_DECODE:
		if (!xdr_u_int(xs, &size)) {
			return XDR_FAILURE;
		}
		if (size > maxsize) { /* TODO should remove some trash from stream */
			return XDR_FAILURE;
		}
		if (*pstr == NULL) {
			*pstr = malloc(size + 1); /* plus one for \0 at the end of string */
			if (*pstr == NULL) {
				return XDR_FAILURE; /* TODO and there */
			}
		}
		*pstr[size] = '\0';
		if (!xdr_opaque(xs, *pstr, size)) {
			free(*pstr);
			return XDR_FAILURE;
		}
		return XDR_SUCCESS;
	case XDR_ENCODE:
		if (*pstr == NULL) {
			return XDR_FAILURE;
		}
		size = strlen(*pstr);
		if (size > maxsize) {
			return XDR_FAILURE;
		}
		/* TODO should remove uint from stream if xdr_opaque failed */
		return xdr_u_int(xs, (xdr_unit_t *)&size)
				&& xdr_opaque(xs, *pstr, size);
	case XDR_FREE:
		if (*pstr != NULL) {
			free(*pstr);
			*pstr = NULL;
		}
		return XDR_SUCCESS;
	}

	return XDR_FAILURE;  /* unknown operation */
}

int xdr_wrapstring(struct xdr *xs, char **pstr) {
	return xdr_string(xs, pstr, (__u32)-1);
}

unsigned int xdr_getpos(struct xdr *xs) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_getpos != NULL);

	return (*xs->ops->x_getpos)(xs);
}

int xdr_setpos(struct xdr *xs, size_t pos) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_setpos != NULL);

	return (*xs->ops->x_setpos)(xs, pos);
}

char * xdr_inline(struct xdr *xs, size_t len) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_inline != NULL);

	return (*xs->ops->x_inline)(xs, len);
}

void xdr_destroy(struct xdr *xs) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_destroy != NULL);

	if (xs->ops->x_destroy) {
		(*xs->ops->x_destroy)(xs);
	}
}
