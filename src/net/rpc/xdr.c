/**
 * @file
 * @brief Realization of the eXternal Data Representation
 * @details XDR: External Data Representation Standard, RFC 1832
 * @date 21.05.12
 * @author Ilia Vaprol
 */

#include <stdlib.h>
#include <assert.h>
#include <net/rpc/rpc.h>
#include <net/rpc/xdr.h>
#include <string.h>
#include <stddef.h>

#define XDR_LAST_UINT32 ((__u32)-1)
#define XDR_SAVE(xs, s)            \
	s = xdr_getpos(xs)
#define XDR_RESTORE(xs, s)         \
	s = (size_t)xdr_setpos(xs, s); \
	assert(s == (size_t)XDR_SUCCESS)

/*
 * Useful routines
 */
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

int xdr_enum(struct xdr *xs, __s32 *pe) {
	/* TODO fix typeof pe */
	/* According to standard enum is interpreted as int */
	return xdr_int(xs, pe);
}

int xdr_bool(struct xdr *xs, __s32 *pb) {
	/* According to standard bool is interpreted as enum */
	return xdr_enum(xs, pb);
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
	return xdr_string(xs, pstr, XDR_LAST_UINT32);
}

int xdr_union(struct xdr *xs, __s32 *pdiscriminant, void *punion,
		const struct xdr_discrim *choices, xdrproc_t dfault) {
	size_t s;

	XDR_SAVE(xs, s);

	if (xdr_enum(xs, pdiscriminant)) {
		while (choices->proc != NULL) {
			if (choices->value == *pdiscriminant) {
				if ((*choices->proc)(xs, punion, XDR_LAST_UINT32)) {
					return XDR_SUCCESS;
				}
				break;
			}
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


/*
 * Other functions for RPC
 */
static int xdr_opaque_auth(struct xdr *xs, struct opaque_auth *oa) {
	// FIXME must used all fields of opaque_auth
	return xdr_enum(xs, (__s32 *)&oa->flavor) && xdr_u_int(xs, &oa->len)
			&& (oa->len == 0); // temporary check
}

static int xdr_accepted_reply(struct xdr *xs, struct accepted_reply *ar) {
	if (xdr_opaque_auth(xs, &ar->verf) && xdr_enum(xs, (__s32 *)&ar->stat)) {
//		switch (ar->stat) {
//		case SUCCESS:
//			;
//		}
;
	}

	return XDR_FAILURE;
}

static int xdr_rejected_reply(struct xdr *xs, struct rejected_reply *rr) {
	if (xdr_enum(xs, (__s32 *)&rr->stat)) {
		switch (rr->stat) {
		case RPC_MISMATCH:
			return xdr_u_int(xs, &rr->d.mismatch_info.low)
					&& xdr_u_int(xs, &rr->d.mismatch_info.high);
		case AUTH_ERROR:
			return xdr_enum(xs, (__s32 *)&rr->d.stat);
		}
	}

	return XDR_FAILURE;
}

static int xdr_call_body(struct xdr *xs, struct call_body *cb) {
	assert(cb->rpcvers == RPC_VERSION);

	return xdr_u_int(xs, &cb->rpcvers) && xdr_u_int(xs, &cb->prog)
			&& xdr_u_int(xs, &cb->vers) && xdr_u_int(xs, &cb->proc)
			&& xdr_opaque_auth(xs, &cb->cred) && xdr_opaque_auth(xs, &cb->verf);
}

static int xdr_reply_body(struct xdr *xs, struct reply_body *rb) {
	const struct xdr_discrim reply_dscrm[] = {
			{MSG_ACCEPTED, (xdrproc_t)xdr_accepted_reply},
			{MSG_DENIED, (xdrproc_t)xdr_rejected_reply},
			{0, NULL_xdrproc_t}
	};

	return xdr_union(xs, (__s32 *)&rb->stat, &rb->r, reply_dscrm, NULL);
//	if (xdr_enum(xs, (__s32 *)&rb->stat)) {
//		switch (rb->stat) {
//		case MSG_ACCEPTED:
//			;
//		case MSG_DENIED:
//		}
//	}
}

int xdr_rpc_msg(struct xdr *xs, struct rpc_msg *msg) {
	size_t s;
	const struct xdr_discrim msg_dscrm[] = {
			{CALL, (xdrproc_t)xdr_call_body},
			{REPLY, (xdrproc_t)xdr_reply_body},
			{0, NULL_xdrproc_t}
	};

	XDR_SAVE(xs, s);

	if (xdr_u_int(xs, &msg->xid) && xdr_enum(xs, (__s32 *)msg->type)) {
#if 1
		if (xdr_union(xs, (__s32 *)&msg->type, &msg->b, msg_dscrm, NULL)) {
			return XDR_SUCCESS;
		}
#else
		switch (msg->type) {
		case CALL:
			if (xdr_call_body(xs, &msg->b.call)) {
				return XDR_SUCCESS;
			}
			break;
		case REPLY:
			if (xdr_reply_body(xs, &msg->b.reply)) {
				return XDR_SUCCESS;
			}
			break;
		}
#endif
	}

	XDR_RESTORE(xs, s);

	return XDR_FAILURE;
}
