/**
 * @file
 * @brief
 * @date 19.05.12
 * @author Ilia Vaprol
 */

#ifndef NET_RPC_RPC_H_
#define NET_RPC_RPC_H_

#include <stdint.h>
//#include <net/rpc/clnt.h> /* Client side (mostly) remote procedure call */
//#include <net/rpc/xdr.h> /* External data representation interfaces */

#define RPC_VERSION 2


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

/* Errors of a authentication */
enum auth_stat {
	AUTH_BADCRED = 1,      /* bad credentials (seal broken) */
	AUTH_REJECTEDCRED = 2, /* client must begin new session */
	AUTH_BADVERF = 3,      /* bad verifier (seal broken)    */
	AUTH_REJECTEDVERF = 4, /* verifier expired or replayed  */
	AUTH_TOOWEAK = 5       /* rejected for security reasons */
};

typedef int opaque; // TODO

enum auth_flavor {
	AUTH_NULL = 0,
	AUTH_UNIX = 1,
	AUTH_SHORT = 2,
	AUTH_DES = 3
};

struct opaque_auth {
	enum auth_flavor flavor;
};

/* Reply to a RPC request that was accepted by the server */
struct accepted_reply {
	struct opaque_auth verf;
	enum accept_stat stat;
	union {
		opaque results;
		struct {
			__u32 low;
			__u32 high;
		} mismatch_info;
	} d; /* data */
};

/* Reply to a RPC request that was rejected by the server */
struct rejected_reply {
	enum reject_stat stat;
	union {
		enum auth_stat stat;
		struct {
			__u32 low;
			__u32 high;
		} mismatch_info;
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
	__u32 rpcvers;
	__u32 prog;
	__u32 vers;
	__u32 proc;
	struct opaque_auth cred;
	struct opaque_auth verf;
};

/* RPC message */
struct rpc_msg {
	__u32 xid;
	enum msg_type type;
	union {
		struct call_body call;
		struct reply_body reply;
	} b; /* body */
};

#endif /* NET_RPC_RPC_H_ */
