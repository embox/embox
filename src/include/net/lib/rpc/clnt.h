/**
 * @file
 * @brief
 * @date 19.05.12
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_RPC_CLNT_H_
#define NET_LIB_RPC_CLNT_H_

#include <stdint.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <net/lib/rpc/xdr.h>
#include <net/lib/rpc/rpc_msg.h>
#include <netinet/in.h>

/* Prototypes */
struct auth;
struct client;
struct sockaddr_in;
struct timeval;

enum clnt_stat {
	RPC_SUCCESS,           /* call succeeded */
	RPC_CANTENCODEARGS,    /* can't encode arguments */
	RPC_CANTDECODERES,     /* can't decode results */
	RPC_CANTSEND,          /* failure in sending call */
	RPC_CANTRECV,          /* failure in receiving result */
	RPC_TIMEDOUT,          /* call timed out */
	RPC_VERSMISMATCH,      /* RPC versions not compatible */
	RPC_AUTHERROR,         /* authentication error */
	RPC_PROGUNAVAIL,       /* program not available */
	RPC_PROGVERSMISMATCH,  /* program version mismatched */
	RPC_PROCUNAVAIL,       /* procedure unavailable */
	RPC_CANTDECODEARGS,    /* decode arguments error */
	RPC_SYSTEMERROR,       /* generic "other problem" */
	RPC_UNKNOWNHOST,       /* unknown host name */
	RPC_PMAPFAILURE,       /* portmapper failed in its call */
	RPC_PROGNOTREGISTERED, /* remote program is not registered */
	RPC_FAILED,
	RPC_UNKNOWNPROTO,      /* unknown protocol */
	RPC_INTR,
	RPC_UNKNOWNADDR,       /* remote address unknown */
	RPC_TLIERROR,
	RPC_NOBROADCAST,       /* broadcasting not supported */
	RPC_N2AXLATEFAILURE,   /* name to addr translation failed */
	RPC_UDERROR,
	RPC_INPROGRESS,
	RPC_STALERACHANDLE,
	RPC_MAX
};

struct rpc_err {
	enum clnt_stat status;
	union {
		int error;
		enum auth_stat reason;
		struct mismatch_info mminfo;
	} extra;
};


struct clnt_ops {
	enum clnt_stat (*call)(struct client *clnt, uint32_t procnum, xdrproc_t inproc,
			char *in, xdrproc_t outproc, char *out, struct timeval wait);
	void (*geterr)(struct client *clnt, struct rpc_err *perr);
	void (*destroy)(struct client *clnt);
};

struct client {
	struct auth *ath;
	const struct clnt_ops *ops;
	int sock;
	struct rpc_err err;
	uint32_t prognum;
	uint32_t versnum;
	/* Protocol specific options */
	union {
		struct {
			struct sockaddr_in sin;
			struct timeval resend;
		} udp;
		struct {
			unsigned int sendsz;
			unsigned int recvsz;
		} tcp;
	} extra;
};

struct rpc_createerr {
	enum clnt_stat stat;
	struct rpc_err err;
};

extern struct rpc_createerr rpc_create_error;

/* Client factory */
extern struct client * clnt_alloc(void);
extern void clnt_free(struct client *clnt);

extern struct client * clnt_create(const char *host, uint32_t prognum, uint32_t versnum,
		const char *prot);
extern struct client * clntudp_create(struct sockaddr_in *addr, uint32_t prognum,
		uint32_t versnum, struct timeval wait, int *psock);

extern struct client * clnttcp_create(struct sockaddr_in *addr, uint32_t prognum,
		uint32_t versnum, int *psock, unsigned int sendsz, unsigned int recvsz);

extern enum clnt_stat clnt_call(struct client *clnt, uint32_t procnum, xdrproc_t inproc,
		char *in, xdrproc_t outproc, char *out, struct timeval wait);

extern void clnt_geterr(struct client * clnt, struct rpc_err *perr);

extern void clnt_destroy(struct client *clnt);

extern struct client * clnttcp_create(struct sockaddr_in *addr, uint32_t prognum, 	uint32_t versnum,
		int *psock, unsigned int sendsz, unsigned int __recvsz);

extern char * clnt_sperrno(enum clnt_stat stat);
extern void clnt_perrno(enum clnt_stat stat);

extern char * clnt_sperror(struct client *clnt, const char *s);
extern void clnt_perror(struct client *clnt, const char *s);

extern char * clnt_spcreateerror(const char *s);
extern void clnt_pcreateerror(const char *s);

#endif /* NET_LIB_RPC_CLNT_H_ */
