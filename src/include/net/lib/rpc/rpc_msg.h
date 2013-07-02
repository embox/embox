/**
 * @file
 * @brief
 * @date 07.06.12
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_RPC_RPC_MSG_H_
#define NET_LIB_RPC_RPC_MSG_H_

#include <stdint.h>
#include <net/lib/rpc/auth.h>

/* Prototypes */
struct xdr;

/* Type of RPC message */
enum msg_type {
	CALL = 0,
	REPLY = 1
};

/* Forms of a reply to a call message */
enum reply_stat {
	MSG_ACCEPTED = 0,
	MSG_DENIED = 1
};

/* Status of an attempt to call a remote procedure if a call message was accepted */
enum accept_stat {
	SUCCESS = 0,       /* RPC executed successfully */
	PROG_UNAVAIL = 1,  /* remote hasn't exported program */
	PROG_MISMATCH = 2, /* remote can't support version */
	PROC_UNAVAIL = 3,  /* program can't support procedure */
	GARBAGE_ARGS = 4   /* procedure can't decode parameters */
};

/* Reasons why a call message was rejected */
enum reject_stat {
	RPC_MISMATCH = 0, /* RPC version number != 2 */
	AUTH_ERROR = 1    /* remote can't authenticate caller */
};

struct mismatch_info {
	uint32_t low;
	uint32_t high;
};

/* Reply to a RPC request that was accepted by the server */
struct accepted_reply {
	struct opaque_auth verf;
	enum accept_stat stat;
	union {
		struct {
			void *decoder;
			void *param;
		} result;
		struct mismatch_info mminfo;
	} d; /* data */
};

/* Reply to a RPC request that was rejected by the server */
struct rejected_reply {
	enum reject_stat stat;
	union {
		enum auth_stat reason;
		struct mismatch_info mminfo;
	} d; /* data */
};

/* Body of a reply to a RPC request */
struct reply_body {
	enum reply_stat stat;
	union {
		struct accepted_reply accepted;
		struct rejected_reply rejected;
	} r; /* reply */
};

/* Body of a RPC request call */
struct call_body {
	uint32_t rpcvers;
	uint32_t prog;
	uint32_t vers;
	uint32_t proc;
	struct opaque_auth cred;
	struct opaque_auth verf;
};

/* RPC message */
struct rpc_msg {
	uint32_t xid;
	enum msg_type type;
	union {
		struct call_body call;
		struct reply_body reply;
	} b; /* body */
};

extern int xdr_mismatch_info(struct xdr *xs, struct mismatch_info *mi);
extern int xdr_accepted_reply(struct xdr *xs, struct accepted_reply *ar);
extern int xdr_rejected_reply(struct xdr *xs, struct rejected_reply *rr);
extern int xdr_call_body(struct xdr *xs, struct call_body *cb);
extern int xdr_reply_body(struct xdr *xs, struct reply_body *rb);
extern int xdr_rpc_msg(struct xdr *xs, struct rpc_msg *msg);

#endif /* NET_LIB_RPC_RPC_MSG_H_ */
