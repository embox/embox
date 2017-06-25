/**
 * @file
 * @brief Realization of the eXternal Data Representation
 * @details XDR: External Data Representation Standard, RFC 1832
 * @date 21.05.12
 * @author Ilia Vaprol
 */

#include <net/lib/rpc/xdr.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#define XDR_SAVE(xs, s)            \
	s = xdr_getpos(xs)
#define XDR_RESTORE(xs, s)         \
	xdr_setpos(xs, s)

/*
 * Useful routines
 */
static int xdr_getunit(struct xdr *xs, xdr_unit_t *to) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->getunit != NULL);

	return (*xs->ops->getunit)(xs, to);
}

static int xdr_putunit(struct xdr *xs, const xdr_unit_t *from) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->putunit != NULL);

	return (*xs->ops->putunit)(xs, from);
}

static int xdr_getbytes(struct xdr *xs, char *to, size_t size) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->getbytes != NULL);

	return (*xs->ops->getbytes)(xs, to, size);
}

static int xdr_putbytes(struct xdr *xs, const char *from, size_t size) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->putbytes != NULL);

	return (*xs->ops->putbytes)(xs, from, size);
}

static int xdr_align(struct xdr *xs, size_t size) {
	xdr_unit_t trash;
	size_t round_up;

	assert(xs != NULL);

	round_up = (BYTES_PER_XDR_UNIT - size % BYTES_PER_XDR_UNIT) % BYTES_PER_XDR_UNIT;

	if (xs->oper == XDR_DECODE) {
		return xdr_getbytes(xs, (char *)&trash, round_up);
	}

	assert(xs->oper == XDR_ENCODE);

	memset(&trash, 0, round_up);

	return xdr_putbytes(xs, (char *)&trash, round_up);
}


/*
 * Standard specification functions
 */
void xdr_free(xdrproc_t proc, char *obj) {
	struct xdr tmp;

	assert(proc != NULL);

	tmp.oper = XDR_FREE;
	(*proc)(&tmp, obj);
}

int xdr_void(void) {
	return XDR_SUCCESS;
}

int xdr_int(struct xdr *xs, int32_t *ps32) {
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

int xdr_u_int(struct xdr *xs, uint32_t *pu32) {
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

int xdr_short(struct xdr *xs, int16_t *ps16) {
	int32_t s32;

	assert(ps16 != NULL);

	s32 = *ps16;
	if (!xdr_int(xs, &s32)) {
		return XDR_FAILURE;
	}

	*ps16 = (int16_t)s32;

	return XDR_SUCCESS;
}

int xdr_u_short(struct xdr *xs, uint16_t *pu16) {
	uint32_t u32;

	assert(pu16 != NULL);

	u32 = *pu16;
	if (!xdr_u_int(xs, &u32)) {
		return XDR_FAILURE;
	}

	*pu16 = (uint16_t)u32;

	return XDR_SUCCESS;
}

int xdr_u_hyper(struct xdr *xs, uint64_t *pu64) {
	size_t s;
	xdr_unit_t unit1, unit2;

	assert(xs != NULL);

	XDR_SAVE(xs, s);

	switch (xs->oper) {
	case XDR_DECODE:
		if (xdr_getunit(xs, &unit1) && xdr_getunit(xs, &unit2)) {
			*pu64 = ((uint64_t)unit1) << 32;
			*pu64 |= (uint32_t)unit2;
			return XDR_SUCCESS;
		}
		break;
	case XDR_ENCODE:
		unit1 = (xdr_unit_t)((*pu64) >> 32);
		unit2 = (xdr_unit_t)(*pu64);
		if (xdr_putunit(xs, &unit1) && xdr_putunit(xs, &unit2)) {
			return XDR_SUCCESS;
		}
		break;
	case XDR_FREE:
		return XDR_SUCCESS;
	}

	XDR_RESTORE(xs, s);

	return XDR_FAILURE;
}

int xdr_hyper(struct xdr *xs, int64_t *ps64) {
	return xdr_u_hyper(xs, (uint64_t *)ps64);
}

int xdr_enum(struct xdr *xs, int32_t *pe) {
	/* According to standard enum is interpreted as int */
	return xdr_int(xs, pe);
}

int xdr_bool(struct xdr *xs, int32_t *pb) {
	/* According to standard bool is interpreted as enum */
	return xdr_enum(xs, pb);
}

int xdr_array(struct xdr *xs, char **parr, uint32_t *psize, uint32_t maxsize,
		uint32_t elem_size, xdrproc_t elem_proc) {
	size_t s;
	uint32_t i, size;
	uint8_t need_free;
	char *pelem;

	assert((xs != NULL) && (parr != NULL) && (psize != NULL)
			&& (elem_size != 0) && (elem_proc != NULL));

	XDR_SAVE(xs, s);

	switch (xs->oper) {
	case XDR_DECODE:
		if (!xdr_u_int(xs, &size) || (size > maxsize)) {
			break;
		}
		if (size == 0) {
			return XDR_SUCCESS;
		}
		if (*parr == NULL) {
			*parr = (char *)malloc(size * elem_size);
			if (*parr == NULL) {
				break;
			}
			need_free = 1;
		}
		else {
			need_free = 0;
		}
		for (i = 0, pelem = *parr;
				(*elem_proc)(xs, pelem, XDR_LAST_UINT32);
				++i, pelem += elem_size) {
			if (i == size) {
				*psize = size;
				return XDR_SUCCESS;
			}
		}
		if (need_free) {
			free(*parr);
			*parr = NULL;
		}
		break;
	case XDR_ENCODE:
		if ((*psize <= maxsize) && xdr_u_int(xs, psize)) {
			for (i = 0, pelem = *parr;
					(i < *psize) && (*elem_proc)(xs, pelem, XDR_LAST_UINT32);
					++i, pelem += elem_size);
			if (i == *psize) {
				return XDR_SUCCESS;
			}
		}
		break;
	case XDR_FREE:
		if (*parr != NULL) {
			free(*parr);
			*parr = NULL;
		}
		return XDR_SUCCESS;
	}

	XDR_RESTORE(xs, s);

	return XDR_FAILURE;
}

int xdr_bytes(struct xdr *xs, char **ppc, uint32_t *psize, uint32_t maxsize) {
	size_t s;
	uint32_t size;
	uint8_t need_free;

	assert((xs != NULL) && (ppc != NULL) && (psize != NULL));

	XDR_SAVE(xs, s);

	switch (xs->oper) {
	case XDR_DECODE:
		if (!xdr_u_int(xs, &size) || (size > maxsize)) {
			break;
		}
		if (size == 0) {
			return XDR_SUCCESS;
		}
		if (*ppc == NULL) {
			*ppc = (char *)malloc(size);
			if (*ppc == NULL) {
				break;
			}
			need_free = 1;
		}
		else {
			need_free = 0;
		}
		if (xdr_getbytes(xs, *ppc, size) && xdr_align(xs, size)) {
			*psize = size;
			return XDR_SUCCESS;
		}
		if (need_free) {
			free(*ppc);
			*ppc = NULL;
		}
		break;
	case XDR_ENCODE:
		if (*psize > maxsize) {
			break;
		}
		if (xdr_u_int(xs, psize) && xdr_putbytes(xs, *ppc, *psize)
				&& xdr_align(xs, *psize)) {
			return XDR_SUCCESS;
		}
		break;
	case XDR_FREE:
		if (*ppc != NULL) {
			free(*ppc);
			*ppc = NULL;
		}
		return XDR_SUCCESS;
	}

	XDR_RESTORE(xs, s);

	return XDR_FAILURE;
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

int xdr_string(struct xdr *xs, char **pstr, uint32_t maxsize) {
	size_t s;
	uint32_t size;
	uint8_t need_free;

	assert((xs != NULL) && (pstr != NULL));

	XDR_SAVE(xs, s);

	switch (xs->oper) {
	case XDR_DECODE:
		if (!xdr_u_int(xs, &size) || (size > maxsize)) {
			break;
		}
		if (*pstr == NULL) {
			*pstr = (char *)malloc(size + 1); /* plus one for \0 at the end of string */
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
			*pstr = NULL;
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
	return xdr_string(xs, pstr, XDR_LAST_UINT32);
}

int xdr_union(struct xdr *xs, int32_t *pdscm, void *pun,
		const struct xdr_discrim *choices, xdrproc_t dfault) {
	size_t s;

	assert((pdscm != NULL) && (choices != NULL));

	XDR_SAVE(xs, s);

	if (xdr_enum(xs, pdscm)) {
		while (choices->proc != NULL) {
			if (choices->value == *pdscm) {
				assert(choices->proc != NULL);
				if ((*choices->proc)(xs, pun, XDR_LAST_UINT32)) {
					return XDR_SUCCESS;
				}
				break;
			}
			choices++;
		}
	}

	if (dfault != NULL) {
		if ((*dfault)(xs, pun, XDR_LAST_UINT32)) {
			return XDR_SUCCESS;
		}
	}

	XDR_RESTORE(xs, s);

	return XDR_FAILURE;
}

size_t xdr_getpos(struct xdr *xs) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->getpos != NULL);

	return (*xs->ops->getpos)(xs);
}

int xdr_setpos(struct xdr *xs, size_t pos) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->setpos != NULL);

	return (*xs->ops->setpos)(xs, pos);
}

void xdr_destroy(struct xdr *xs) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->destroy != NULL);

	(*xs->ops->destroy)(xs);
}
