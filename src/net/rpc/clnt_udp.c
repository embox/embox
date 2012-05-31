/**
 * @file
 * @brief
 * @date 28.05.12
 * @author Ilia Vaprol
 */

#include <net/rpc/rpc.h>
#include <net/rpc/clnt.h>
#include <stdlib.h>
#include <assert.h>
#include <net/socket.h>
#include <net/net.h>
#include <net/ip.h>
#include <string.h>

static const struct clnt_ops clntudp_ops;

struct client * clntudp_create(struct sockaddr_in *raddr, __u32 prognum,
		__u32 versnum, struct timeval wait, int *psock) {
	struct client *clnt;
	int sock;

	assert((raddr != NULL) && (psock != NULL));

	clnt = (struct client *)malloc(sizeof *clnt);
	if (clnt == NULL) {
		return NULL;
	}

	assert(raddr->sin_port != 0); // else call port-mapper

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		/* TODO insert error handling there */
		free(clnt);
		return NULL;
	}
	*psock = sock;

	/* Fill rpc_msg structure */
	clnt->msg.xid = 0x01010101;
	clnt->msg.type = CALL;
	clnt->msg.b.call.rpcvers = RPC_VERSION;
	clnt->msg.b.call.prog = prognum;
	clnt->msg.b.call.vers = versnum;
	/* Fill other filed */
	clnt->ops = &clntudp_ops;
	clnt->sock = sock;
	memcpy(&clnt->sin, raddr, sizeof *raddr);

	return clnt;
}

enum clnt_stat clntudp_call(struct client *clnt, __u32 procnum, xdrproc_t inproc,
		char *in, xdrproc_t outproc, char *out, struct timeval wait) {
	char buff[1024];
	char zero[16]; // TODO remove this
	struct xdr xstream;

	clnt->msg.b.call.proc = procnum;

	xdrmem_create(&xstream, buff, sizeof buff, XDR_ENCODE);

	memset(zero, 0, sizeof zero);
	if (xdr_u_int(&xstream, &clnt->msg.xid) && xdr_enum(&xstream, (__s32 *)&clnt->msg.type)
			&& xdr_u_int(&xstream, &clnt->msg.b.call.rpcvers)
			&& xdr_u_int(&xstream, &clnt->msg.b.call.prog)
			&& xdr_u_int(&xstream, &clnt->msg.b.call.vers)
			&& xdr_u_int(&xstream, &clnt->msg.b.call.proc)
			&& xdr_opaque(&xstream, zero, sizeof zero)
			&& (*inproc)(&xstream, in)) {
		return sendto(clnt->sock, buff, xdr_getpos(&xstream), 0, (struct sockaddr *)&clnt->sin, sizeof clnt->sin);
	}

	return -1;
}

void clntudp_destroy(struct client *clnt) {
	close(clnt->sock);
	free(clnt);
}

static const struct clnt_ops clntudp_ops = {
		.cl_call = clntudp_call,
		.cl_destroy = clntudp_destroy,
};
