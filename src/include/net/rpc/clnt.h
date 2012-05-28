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
#include <net/rpc/xdr.h>


enum clnt_stat {
	RPC_SUCCESS = 0,
	RPC_CANTENCODEARGS = 1,
	RPC_CANTDECODERES = 2,
	RPC_CANTSEND = 3,
	RPC_CANTRECV = 4,
	RPC_TIMEDOUT = 5,
	RPC_UNKNOWNHOST = 13,
	RPC_UNKNOWNPROTO = 17,
	RPC_UNKNOWNADDR = 19
};

struct client;

struct clnt_ops {
	enum clnt_stat (*cl_call)(struct client *clnt, __u32 procnum, xdrproc_t inproc,
			char *in, xdrproc_t outproc, char *out, struct timeval wait);
	void (*cl_destroy)(struct client *clnt);
};

struct client {
//	struct auth *cl_auth;
	const struct clnt_ops *ops;
	int sock;
};

extern struct client * clnt_create(const char *host, __u32 prognum, __u32 versnum,
		const char *prot);
extern struct client * clntudp_create(struct sockaddr_in *addr, __u32 prognum,
		__u32 versnum, struct timeval wait, int *psock);

extern enum clnt_stat clnt_call(struct client *clnt, __u32 procnum, xdrproc_t inproc,
		char *in, xdrproc_t outproc, char *out, struct timeval wait);

extern void clnt_destroy(struct client *clnt);

#if 0
extern struct client * clnttcp_create(struct sockaddr_in *addr, __u32 prognum, 	__u32 versnum,
		int *psock, unsigned int sendsz, unsigned int __recvsz);
extern void clnt_pcreateerror(const char *s);
extern char * clnt_spcreateerror(const char *s);

extern void clnt_perrno(enum clnt_stat stat);
extern char * clnt_perrno(enum clnt_stat stat);

extern void clnt_perror(struct client *clnt, const char *s);
extern char * clnt_sperror(struct client *clnt, const char *s);
#endif

#endif /* NET_RPC_CLNT_H_ */
