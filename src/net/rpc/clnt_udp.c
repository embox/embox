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
	clnt->msg_call.xid = 0x01010101;
	clnt->msg_call.type = CALL;
	clnt->msg_call.b.call.rpcvers = RPC_VERSION;
	clnt->msg_call.b.call.prog = prognum;
	clnt->msg_call.b.call.vers = versnum;
	/* TODO move to a separate function */
	clnt->msg_call.b.call.cred.flavor = AUTH_NULL;
	clnt->msg_call.b.call.cred.len = 0;
	clnt->msg_call.b.call.verf.flavor = AUTH_NULL;
	clnt->msg_call.b.call.verf.len = 0;

	/* Fill other filed */
	clnt->ops = &clntudp_ops;
	clnt->sock = sock;
	memcpy(&clnt->sin, raddr, sizeof *raddr);

	return clnt;
}

static enum clnt_stat clntudp_call(struct client *clnt, __u32 procnum,
		xdrproc_t inproc, char *in, xdrproc_t outproc, char *out,
		struct timeval wait) {
	int res;
	char buff[1024];
	struct xdr xstream;
	struct rpc_msg msg_reply;
	struct sockaddr addr;
	socklen_t addr_len;

	clnt->msg_call.b.call.proc = procnum;

	xdrmem_create(&xstream, buff, sizeof buff, XDR_ENCODE);
	if (!xdr_rpc_msg(&xstream, &clnt->msg_call)
			|| !(*inproc)(&xstream, in)) {
		return RPC_CANTENCODEARGS;
	}
	xdr_destroy(&xstream);

	res = sendto(clnt->sock, buff, xdr_getpos(&xstream), 0,
			(struct sockaddr *)&clnt->sin, sizeof clnt->sin);
	if (res < 0) {
		return RPC_CANTSEND;
	}

	res = recvfrom(clnt->sock, buff, sizeof buff, 0, &addr, &addr_len);
	if (res < 0) {
		return RPC_CANTRECV;
	}

	xdrmem_create(&xstream, buff, sizeof buff, XDR_DECODE);
	if (!xdr_rpc_msg(&xstream, &msg_reply)
			|| !(*outproc)(&xstream, out)) {
		return RPC_CANTENCODEARGS;
	}
	xdr_destroy(&xstream);

	return RPC_SUCCESS;
}

static void clntudp_destroy(struct client *clnt) {
	close(clnt->sock);
	free(clnt);
}

static const struct clnt_ops clntudp_ops = {
		.cl_call = clntudp_call,
		.cl_destroy = clntudp_destroy,
};
