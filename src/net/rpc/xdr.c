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

#define XDR_SAVE(xs, s)            \
	s = xdr_getpos(xs)
#define XDR_RESTORE(xs, s)         \
	s = (size_t)xdr_setpos(xs, s); \
	assert(s == (size_t)XDR_SUCCESS)

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

static int xdr_align(struct xdr *xs, size_t size) {
	xdr_unit_t trash;
	size_t round_up;

	assert(xs != NULL);

	round_up = size % BYTES_PER_XDR_UNIT;
	if (round_up == 0) {
		return XDR_SUCCESS;
	}
	round_up = BYTES_PER_XDR_UNIT - round_up;

	if (xs->oper == XDR_DECODE) {
		return xdr_getbytes(xs, (char *)&trash, round_up);
	}

	assert(xs->oper == XDR_ENCODE);

	memset(&trash, 0, round_up);

	return xdr_putbytes(xs, (char *)&trash, round_up);
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

int xdr_enum(struct xdr *xs, __s32 *pe) {
	return xdr_int(xs, pe);
}

int xdr_bool(struct xdr *xs, __s32 *pb) {
	return xdr_int(xs, pb);
}

int xdr_opaque(struct xdr *xs, char *pc, size_t size) {
	size_t s;

	assert((xs != NULL) && (pc != NULL));

	XDR_SAVE(xs, s);

	switch (xs->oper) {
	case XDR_DECODE:
		if (xdr_getbytes(xs, pc, size) && xdr_align(xs, size)) {
			return XDR_SUCCESS;
		}
		break;
	case XDR_ENCODE:
		if (xdr_putbytes(xs, pc, size) && xdr_align(xs, size)) {
			return XDR_SUCCESS;
		}
		break;
	case XDR_FREE:
		return XDR_SUCCESS;
	}

	XDR_RESTORE(xs, s);

	return XDR_FAILURE;
}

int xdr_string(struct xdr *xs, char **pstr, __u32 maxsize) {
	size_t s;
	__u32 size;
	char need_free;

	assert((xs != NULL) && (pstr != NULL));

	XDR_SAVE(xs, s);

	switch (xs->oper) {
	case XDR_DECODE:
		if (!xdr_u_int(xs, &size) || (size > maxsize)) {
			break;
		}
		if (*pstr == NULL) {
			*pstr = malloc(size + 1); /* plus one for \0 at the end of string */
			if (*pstr == NULL) {
				break;
			}
			need_free = 1;
		}
		else {
			need_free = 0;
		}
		*pstr[size] = '\0';
		if (xdr_getbytes(xs, *pstr, size) && xdr_align(xs, size)) {
			return XDR_SUCCESS;
		}
		if (need_free) {
			free(*pstr);
		}
		break;
	case XDR_ENCODE:
		if (*pstr == NULL) {
			break;
		}
		size = strlen(*pstr);
		if (size > maxsize) {
			break;
		}
		if (xdr_u_int(xs, &size) && xdr_putbytes(xs, *pstr, size)
				&& xdr_align(xs, size)) {
			return XDR_SUCCESS;
		}
		break;
	case XDR_FREE:
		if (*pstr != NULL) {
			free(*pstr);
			*pstr = NULL;
		}
		return XDR_SUCCESS;
	}

	XDR_RESTORE(xs, s);

	return XDR_FAILURE;
}

int xdr_wrapstring(struct xdr *xs, char **pstr) {
	return xdr_string(xs, pstr, (__u32)-1);
}

size_t xdr_getpos(struct xdr *xs) {
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
