/**
 * @file
 * @brief
 *
 * @date 04.06.2012
 * @author Andrey Gazukin
 */


#include <net/rpc/rpc.h>
#include <net/rpc/clnt.h>
#include <stdlib.h>
#include <assert.h>
#include <net/socket.h>
#include <net/net.h>
#include <net/ip.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include <types.h>
#include <net/ip.h>

#define TCP_MSG_MAX_SZ 1024

static const struct clnt_ops clnttcp_ops;

struct client * clnttcp_create(struct sockaddr_in *raddr, __u32 prognum,
		__u32 versnum, struct timeval wait, int *psock) {
	struct client *clnt;
	int sock;
	__u16 port;

	assert((raddr != NULL) && (psock != NULL));

	clnt = (struct client *)malloc(sizeof *clnt);
	if (clnt == NULL) {
		rpc_create_error.stat = RPC_SYSTEMERROR;
		rpc_create_error.err.extra.error = ENOMEM;
		LOG_ERROR("no memory\n");
		goto error;
	}

	port = ntohs(raddr->sin_port);
	if (port == 0) {
		port = pmap_getport(raddr, prognum, versnum, IPPROTO_TCP);
		if (port == 0) {
			goto error;
		}
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		rpc_create_error.stat = RPC_SYSTEMERROR;
		rpc_create_error.err.extra.error = errno;
		LOG_ERROR("can't create socket\n");
		goto error;
	}
	*psock = sock;

	raddr->sin_port = htons(port);
	if (0 > connect(sock, (struct sockaddr *)raddr,  sizeof(*raddr))) {
		printf("Error.. connect() failed. errno=%d\n", errno);
		goto error;
	}

	/* Fill rpc_msg structure */
	clnt->msg.xid = 0x02020242;
	clnt->msg.type = CALL;
	clnt->msg.b.call.rpcvers = RPC_VERSION;
	clnt->msg.b.call.prog = prognum;
	clnt->msg.b.call.vers = versnum;
	/* TODO move to a separate function */
	clnt->msg.b.call.cred.flavor = AUTH_NULL;
	clnt->msg.b.call.cred.len = 0;
	clnt->msg.b.call.verf.flavor = AUTH_NULL;
	clnt->msg.b.call.verf.len = 0;

	/* Fill other filed */
	clnt->ops = &clnttcp_ops;
	clnt->sock = sock;
	memcpy(&clnt->sin, raddr, sizeof *raddr);
	clnt->sin.sin_port = htons(port);

	return clnt;
error:
	free(clnt);
	return NULL;
}

int usleep(unsigned long);// TODO REMOVE THIS
static enum clnt_stat clnttcp_call(struct client *clnt, __u32 procnum,
		xdrproc_t inproc, char *in, xdrproc_t outproc, char *out,
		struct timeval wait) {
	int res;
	char buff[TCP_MSG_MAX_SZ];
	struct xdr xstream;
	struct rpc_msg msg_reply, *mcall, *mreply;
	struct sockaddr addr;
	socklen_t addr_len;

	assert((clnt != NULL) && (inproc != NULL) && (outproc != NULL));

	mcall = &clnt->msg;
	mreply = &msg_reply;

	mcall->b.call.proc = procnum;
	clnt->msg.xid++;

	xdrmem_create(&xstream, buff, sizeof buff, XDR_ENCODE);
	if (!xdr_rpc_msg(&xstream, mcall)
			|| !(*inproc)(&xstream, in)) {
		clnt->err.status = RPC_CANTENCODEARGS;
		return clnt->err.status;
	}
	xdr_destroy(&xstream);

	res = sendto(clnt->sock, buff, xdr_getpos(&xstream), 0,
			(struct sockaddr *)&clnt->sin, sizeof clnt->sin);
	if (res < 0) {
		clnt->err.status = RPC_CANTSEND;
		clnt->err.extra.error = errno;
		return clnt->err.status;
	}

	usleep(10);
	res = recvfrom(clnt->sock, buff, sizeof buff, 0, &addr, &addr_len);
	if (res < 0) {
		clnt->err.status = RPC_CANTRECV;
		clnt->err.extra.error = errno;
		return clnt->err.status;
	}

	xdrmem_create(&xstream, buff, res, XDR_DECODE);
	mreply->b.reply.r.accepted.d.result.decoder = outproc;
	mreply->b.reply.r.accepted.d.result.param = out;
	if (!xdr_rpc_msg(&xstream, mreply)) {
		clnt->err.status = RPC_CANTDECODERES;
		return clnt->err.status;
	}
	xdr_destroy(&xstream);

	clnt->err.status = RPC_SUCCESS;
	return clnt->err.status;
}

static void clnttcp_geterr(struct client *clnt, struct rpc_err *perr) {
	assert((clnt != NULL) && (perr != NULL));

	memcpy(perr, &clnt->err, sizeof clnt->err);
}

static void clnttcp_destroy(struct client *clnt) {
	assert(clnt != NULL);

	close(clnt->sock);
	free(clnt);
}

static const struct clnt_ops clnttcp_ops = {
		.cl_call = clnttcp_call,
		.cl_geterr = clnttcp_geterr,
		.cl_destroy = clnttcp_destroy,
};

