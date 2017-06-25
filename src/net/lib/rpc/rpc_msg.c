/**
 * @file
 * @brief
 * @date 07.06.12
 * @author Ilia Vaprol
 */

#include <net/lib/rpc/rpc_msg.h>
#include <net/lib/rpc/auth.h>
#include <net/lib/rpc/xdr.h>
#include <assert.h>
#include <stdint.h>

int xdr_mismatch_info(struct xdr *xs, struct mismatch_info *mi) {
	assert(mi != NULL);

	return xdr_u_int(xs, &mi->low) && xdr_u_int(xs, &mi->high);
}

int xdr_accepted_reply(struct xdr *xs, struct accepted_reply *ar) {
	int32_t ar_stat;

	assert(ar != NULL);

	ar_stat = ar->stat;
	if (!xdr_opaque_auth(xs, &ar->verf)
			|| !xdr_enum(xs, &ar_stat)) {
		return XDR_FAILURE;
	}

	ar->stat = ar_stat;
	switch (ar->stat) {
	default:
		break;
	case SUCCESS:
		assert(ar->d.result.decoder != NULL);
		return (*(xdrproc_t)ar->d.result.decoder)(xs, ar->d.result.param, XDR_LAST_UINT32);
	case PROG_MISMATCH:
		return xdr_mismatch_info(xs, &ar->d.mminfo);
	}

	return XDR_SUCCESS;
}

int xdr_rejected_reply(struct xdr *xs, struct rejected_reply *rr) {
	static const struct xdr_discrim reject_dscrm[] = {
			{ RPC_MISMATCH, (xdrproc_t)xdr_mismatch_info },
			{ AUTH_ERROR, (xdrproc_t)xdr_enum },
			{ 0, NULL }
	};
	int32_t rr_stat;

	assert(rr != NULL);

	rr_stat = rr->stat;
	if (!xdr_union(xs, &rr_stat, &rr->d, reject_dscrm, NULL)) {
		return XDR_FAILURE;
	}

	rr->stat = rr_stat;

	return XDR_SUCCESS;
}

int xdr_call_body(struct xdr *xs, struct call_body *cb) {
	assert(cb != NULL);

	return xdr_u_int(xs, &cb->rpcvers) && xdr_u_int(xs, &cb->prog)
			&& xdr_u_int(xs, &cb->vers) && xdr_u_int(xs, &cb->proc)
			&& xdr_opaque_auth(xs, &cb->cred) && xdr_opaque_auth(xs, &cb->verf);
}

int xdr_reply_body(struct xdr *xs, struct reply_body *rb) {
	static const struct xdr_discrim reply_dscrm[] = {
			{ MSG_ACCEPTED, (xdrproc_t)xdr_accepted_reply },
			{ MSG_DENIED, (xdrproc_t)xdr_rejected_reply },
			{ 0, NULL }
	};
	int32_t rb_stat;

	assert(rb != NULL);

	rb_stat = rb->stat;
	if (!xdr_union(xs, &rb_stat, &rb->r, reply_dscrm, NULL)) {
		return XDR_FAILURE;
	}

	rb->stat = rb_stat;

	return XDR_SUCCESS;
}

int xdr_rpc_msg(struct xdr *xs, struct rpc_msg *msg) {
	static const struct xdr_discrim msg_dscrm[] = {
			{ CALL, (xdrproc_t)xdr_call_body },
			{ REPLY, (xdrproc_t)xdr_reply_body },
			{ 0, NULL }
	};
	int32_t msg_type;

	assert(msg != NULL);

	msg_type = msg->type;
	if (!xdr_u_int(xs, &msg->xid)
			|| !xdr_union(xs, &msg_type, &msg->b, msg_dscrm, NULL)) {
		return XDR_FAILURE;
	}

	msg->type = msg_type;

	return XDR_SUCCESS;
}
