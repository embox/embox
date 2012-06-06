/**
 * @file
 * @brief Realization of the eXternal Data Representation
 * @details XDR: External Data Representation Standard, RFC 1832
 * @date 21.05.12
 * @author Ilia Vaprol
 */

#include <stdlib.h>
#include <assert.h>
#include <fs/nfs.h>
#include <net/rpc/rpc.h>
#include <net/rpc/xdr.h>
#include <string.h>
#include <stddef.h>

#define XDR_LAST_UINT32 ((__u32)-1)
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

int xdr_short(struct xdr *xs, __s16 *ps16) {
	__s32 s32;

	assert(ps16 != NULL);

	s32 = *ps16;
	if (!xdr_int(xs, &s32)) {
		return XDR_FAILURE;
	}

	*ps16 = (__s16)s32;

	return XDR_SUCCESS;
}

int xdr_u_short(struct xdr *xs, __u16 *pu16) {
	__u32 u32;

	assert(pu16 != NULL);

	u32 = *pu16;
	if (!xdr_u_int(xs, &u32)) {
		return XDR_FAILURE;
	}

	*pu16 = (__u16)u32;

	return XDR_SUCCESS;
}

int xdr_enum(struct xdr *xs, __s32 *pe) {
	/* According to standard enum is interpreted as int */
	return xdr_int(xs, pe);
}

int xdr_bool(struct xdr *xs, __s32 *pb) {
	/* According to standard bool is interpreted as enum */
	return xdr_enum(xs, pb);
}

int xdr_bytes(struct xdr *xs, char **ppc, __u32 *psize, __u32 maxsize) {
	size_t s;
	__u32 size;
	char need_free;

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
		}
		break;
		return XDR_SUCCESS;
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

int xdr_union(struct xdr *xs, __s32 *pdiscriminant, void *punion,
		const struct xdr_discrim *choices, xdrproc_t dfault) {
	size_t s;

	assert((pdiscriminant != NULL) && (choices != NULL));

	XDR_SAVE(xs, s);

	if (xdr_enum(xs, pdiscriminant)) {
		while (choices->proc != NULL) {
			if (choices->value == *pdiscriminant) {
				assert(choices->proc != NULL);
				if ((*choices->proc)(xs, punion, XDR_LAST_UINT32)) {
					return XDR_SUCCESS;
				}
				break;
			}
			choices++;
		}
	}

	if (dfault != NULL) {
		if ((*dfault)(xs, punion, XDR_LAST_UINT32)) {
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


/*
 * Other functions for RPC
 */
static int xdr_opaque_auth(struct xdr *xs, struct opaque_auth *oa) {
	assert(oa != NULL);

	return xdr_enum(xs, (__s32 *)&oa->flavor)
			&& xdr_bytes(xs, &oa->data, &oa->len, MAX_AUTH_BYTES);
}

static int xdr_mismatch_info(struct xdr *xs, struct mismatch_info *mi) {
	assert(mi != NULL);

	return xdr_u_int(xs, &mi->low) && xdr_u_int(xs, &mi->high);
}

static int xdr_accepted_reply(struct xdr *xs, struct accepted_reply *ar) {
	assert(ar != NULL);

	if (xdr_opaque_auth(xs, &ar->verf) && xdr_enum(xs, (__s32 *)&ar->stat)) {
		switch (ar->stat) {
		default:
			return XDR_SUCCESS;
		case SUCCESS:
			assert(ar->d.result.decoder != NULL);
			return (*(xdrproc_t)ar->d.result.decoder)(xs, ar->d.result.param, XDR_LAST_UINT32);
		case PROG_MISMATCH:
			return xdr_mismatch_info(xs, &ar->d.mminfo);
		}
	}

	return XDR_FAILURE;
}

static int xdr_rejected_reply(struct xdr *xs, struct rejected_reply *rr) {
	const struct xdr_discrim reject_dscrm[] = {
			{ RPC_MISMATCH, (xdrproc_t)xdr_mismatch_info },
			{ AUTH_ERROR, (xdrproc_t)xdr_enum },
			{ 0, NULL }
	};

	assert(rr != NULL);

	return xdr_union(xs, (__s32 *)&rr->stat, &rr->d, reject_dscrm, NULL);
}

static int xdr_call_body(struct xdr *xs, struct call_body *cb) {
	assert(cb != NULL);

	return xdr_u_int(xs, &cb->rpcvers) && xdr_u_int(xs, &cb->prog)
			&& xdr_u_int(xs, &cb->vers) && xdr_u_int(xs, &cb->proc)
			&& xdr_opaque_auth(xs, &cb->cred) && xdr_opaque_auth(xs, &cb->verf);
}

static int xdr_reply_body(struct xdr *xs, struct reply_body *rb) {
	const struct xdr_discrim reply_dscrm[] = {
			{ MSG_ACCEPTED, (xdrproc_t)xdr_accepted_reply },
			{ MSG_DENIED, (xdrproc_t)xdr_rejected_reply },
			{ 0, NULL }
	};

	assert(rb != NULL);

	return xdr_union(xs, (__s32 *)&rb->stat, &rb->r, reply_dscrm, NULL);
}

int xdr_rpc_msg(struct xdr *xs, struct rpc_msg *msg) {
	const struct xdr_discrim msg_dscrm[] = {
			{ CALL, (xdrproc_t)xdr_call_body },
			{ REPLY, (xdrproc_t)xdr_reply_body },
			{ 0, NULL }
	};
	size_t s;

	assert(msg != NULL);

	XDR_SAVE(xs, s);

	if (xdr_u_int(xs, &msg->xid)
			&& xdr_union(xs, (__s32 *)&msg->type, &msg->b, msg_dscrm, NULL)) {
		return XDR_SUCCESS;
	}

	XDR_RESTORE(xs, s);

	return XDR_FAILURE;
}

int xdr_pmap(struct xdr *xs, struct pmap *pmp) {
	size_t s;

	assert(pmp != NULL);

	XDR_SAVE(xs, s);

	if (xdr_u_int(xs, &pmp->prog) && xdr_u_int(xs, &pmp->vers)
			&& xdr_u_int(xs, &pmp->prot) && xdr_u_int(xs, &pmp->port)) {
		return XDR_SUCCESS;
	}

	XDR_RESTORE(xs, s);

	return XDR_FAILURE;
}
