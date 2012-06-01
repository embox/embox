/**
 * @file
 * @brief
 * @date 19.05.12
 * @author Ilia Vaprol
 */

#ifndef NET_RPC_CLNT_H_
#define NET_RPC_CLNT_H_

#include <stdint.h>
#include <net/socket.h>
#include <sys/time.h>
#include <net/rpc/rpc.h>
#include <net/rpc/xdr.h>
#include <net/rpc/auth.h>

/* Prototypes */
struct client;
struct rpc_err;
struct rpc_msg;

enum clnt_stat {
	RPC_SUCCESS = 0,            /* call succeeded */
	RPC_CANTENCODEARGS = 1,     /* can't encode arguments */
	RPC_CANTDECODERES = 2,      /* can't decode results */
	RPC_CANTSEND = 3,           /* failure in sending call */
	RPC_CANTRECV = 4,           /* failure in receiving result */
	RPC_TIMEDOUT = 5,           /* call timed out */
	RPC_VERSMISMATCH = 6,       /* RPC versions not compatible */
	RPC_AUTHERROR = 7,          /* authentication error */
	RPC_PROGUNAVAIL = 8,        /* program not available */
	RPC_PROGVERSMISMATCH = 9,   /* program version mismatched */
	RPC_PROCUNAVAIL = 10,       /* procedure unavailable */
	RPC_CANTDECODEARGS = 11,    /* decode arguments error */
	RPC_SYSTEMERROR = 12,       /* generic "other problem" */
	RPC_UNKNOWNHOST = 13,       /* unknown host name */
	RPC_PMAPFAILURE = 14,       /* portmapper failed in its call */
	RPC_PROGNOTREGISTERED = 15, /* remote program is not registered */
	RPC_FAILED = 16,
	RPC_UNKNOWNPROTO = 17,      /* unknown protocol */
	RPC_INTR = 18,
	RPC_UNKNOWNADDR = 19,       /* remote address unknown */
	RPC_TLIERROR = 20,
	RPC_NOBROADCAST = 21,       /* broadcasting not supported */
	RPC_N2AXLATEFAILURE = 22,   /* name to addr translation failed */
	RPC_UDERROR = 23,
	RPC_INPROGRESS = 24,
	RPC_STALERACHANDLE = 25
};

struct rpc_err {
	enum clnt_stat status;
	union {
		int code;
		enum auth_stat reason;
		struct mismatch_info vers;
	} extra;
};

struct clnt_ops {
	enum clnt_stat (*cl_call)(struct client *clnt, __u32 procnum, xdrproc_t inproc,
			char *in, xdrproc_t outproc, char *out, struct timeval wait);
	void (*cl_geterr)(struct client *clnt, struct rpc_err *perr);
	void (*cl_destroy)(struct client *clnt);
};

struct client {
//	struct auth *cl_auth;
	const struct clnt_ops *ops;
	int sock;
	struct rpc_msg msg;
	struct rpc_err err;
	struct sockaddr_in sin;
};

extern struct client * clnt_create(const char *host, __u32 prognum, __u32 versnum,
		const char *prot);
extern struct client * clntudp_create(struct sockaddr_in *addr, __u32 prognum,
		__u32 versnum, struct timeval wait, int *psock);

extern enum clnt_stat clnt_call(struct client *clnt, __u32 procnum, xdrproc_t inproc,
		char *in, xdrproc_t outproc, char *out, struct timeval wait);

extern void clnt_geterr(struct client * clnt, struct rpc_err *perr);

extern void clnt_destroy(struct client *clnt);

#if 0
extern struct client * clnttcp_create(struct sockaddr_in *addr, __u32 prognum, 	__u32 versnum,
		int *psock, unsigned int sendsz, unsigned int __recvsz);
#endif

extern char * clnt_spcreateerror(const char *s);
extern void clnt_pcreateerror(const char *s);

extern char * clnt_sperrno(enum clnt_stat stat);
extern void clnt_perrno(enum clnt_stat stat);

//extern char * clnt_sperror(struct client *clnt, const char *s);
extern void clnt_perror(struct client *clnt, const char *s);

#endif /* NET_RPC_CLNT_H_ */
