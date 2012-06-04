/**
 * @file
 * @brief
 * @date 28.05.12
 * @author Ilia Vaprol
 */

#include <net/rpc/rpc.h>
#include <net/rpc/clnt.h>
#include <net/rpc/auth.h>
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
#include <kernel/time.h>
#include <kernel/ktime.h>

#define UDP_MSG_MAX_SZ 1024

static const struct clnt_ops clntudp_ops;

/*
 * TODO remove this and use get_timeval instead
 * after Issue 432 will be fixed
 */
static void tmp_get_timeval(struct ktimeval *kt) {
	kt->tv_sec = 0;
	kt->tv_usec = 0;
}
extern int usleep(unsigned long usec);

struct client * clntudp_create(struct sockaddr_in *raddr, __u32 prognum,
		__u32 versnum, struct timeval resend, int *psock) {
	struct client *clnt;
	struct auth *ath;
	int sock;
	__u16 port;

	assert((raddr != NULL) && (psock != NULL));

	clnt = (struct client *)malloc(sizeof *clnt);
	if (clnt == NULL) {
		rpc_create_error.stat = RPC_SYSTEMERROR;
		rpc_create_error.err.extra.error = ENOMEM;
		LOG_ERROR("no memory\n");
		goto exit_with_error;
	}

	ath = authnone_create();
	if (ath == NULL) {
		rpc_create_error.stat = RPC_SYSTEMERROR;
		rpc_create_error.err.extra.error = ENOMEM;
		LOG_ERROR("no memory\n");
		goto exit_with_error;
	}

	port = ntohs(raddr->sin_port);
	if (port == 0) {
		port = pmap_getport(raddr, prognum, versnum, IPPROTO_UDP);
		if (port == 0) {
			goto exit_with_error;
		}
	}

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		rpc_create_error.stat = RPC_SYSTEMERROR;
		rpc_create_error.err.extra.error = errno;
		LOG_ERROR("can't create socket\n");
		goto exit_with_error;
	}
	*psock = sock;

	/* Fill rpc_msg structure */
	clnt->msg.xid = 0x01010101;
	clnt->msg.type = CALL;
	clnt->msg.b.call.rpcvers = RPC_VERSION;
	clnt->msg.b.call.prog = prognum;
	clnt->msg.b.call.vers = versnum;
	memcpy(&clnt->msg.b.call.cred, &ath->cred, sizeof ath->cred);
	memcpy(&clnt->msg.b.call.verf, &ath->verf, sizeof ath->verf);

	/* Fill other filed */
	clnt->ops = &clntudp_ops;
	clnt->sock = sock;
	clnt->ath = ath;
	memcpy(&clnt->resend, &resend, sizeof resend);
	memcpy(&clnt->sin, raddr, sizeof *raddr);
	clnt->sin.sin_port = htons(port);
	return clnt;
exit_with_error:
	auth_destroy(ath);
	free(clnt);
	return NULL;
}

static enum clnt_stat clntudp_call(struct client *clnt, __u32 procnum,
		xdrproc_t inproc, char *in, xdrproc_t outproc, char *out,
		struct timeval timeout) {
	int res;
	char buff[UDP_MSG_MAX_SZ];
	struct xdr xstream;
	struct rpc_msg msg_reply, *mcall, *mreply;
	struct sockaddr addr;
	socklen_t addr_len;
	struct ktimeval was_sended, now, elapsed;
	struct timeval *resend;
	int iter = 0; // TODO remove this

	assert((clnt != NULL) && (inproc != NULL));

	mcall = &clnt->msg;
	mreply = &msg_reply;
	resend = &clnt->resend;

	mcall->b.call.proc = procnum;

	xdrmem_create(&xstream, buff, sizeof buff, XDR_ENCODE);
	if (!xdr_rpc_msg(&xstream, mcall)
			|| !(*inproc)(&xstream, in)) {
		clnt->err.status = RPC_CANTENCODEARGS;
		goto exit_with_status;
	}
	xdr_destroy(&xstream);

send_again:
	tmp_get_timeval(&was_sended);

	res = sendto(clnt->sock, buff, xdr_getpos(&xstream), 0,
			(struct sockaddr *)&clnt->sin, sizeof clnt->sin);
	if (res < 0) {
		clnt->err.status = RPC_CANTSEND;
		clnt->err.extra.error = errno;
		goto exit_with_status;
	}

	usleep(20);
recv_again:
	iter++;
	res = recvfrom(clnt->sock, buff, sizeof buff, 0, &addr, &addr_len);
	if (res < 0) {
		clnt->err.status = RPC_CANTRECV;
		clnt->err.extra.error = errno;
		goto exit_with_status;
	}

	if (res == 0) { /* Reply was not received, resend request or exit with error */

		/* Calculate elapsed time */
		tmp_get_timeval(&now);
		elapsed.tv_sec = now.tv_sec - was_sended.tv_sec;
		elapsed.tv_usec = now.tv_usec - was_sended.tv_usec;
		if (elapsed.tv_usec < 0) {
			elapsed.tv_usec += USEC_PER_SEC;
			elapsed.tv_sec--;
		}

		/* how much time we can wait yet */
		timeout.tv_sec -= elapsed.tv_sec;
		if (timeout.tv_usec >= elapsed.tv_usec) {
			timeout.tv_usec -= elapsed.tv_usec;
		}
		else {
			timeout.tv_sec--;
			timeout.tv_usec += USEC_PER_SEC - elapsed.tv_usec;
		}

		/* check timeout */
		if ((iter > 20) || (timeout.tv_sec < 0)
				|| ((timeout.tv_sec == 0) && (timeout.tv_usec == 0))) {
			clnt->err.status = RPC_TIMEDOUT;
			goto exit_with_status;
		}

		/* check resend time */
		if ((elapsed.tv_sec > resend->tv_sec)
				|| ((elapsed.tv_sec == resend->tv_sec)
						&& (elapsed.tv_usec >= resend->tv_usec))) {
			goto send_again;
		}

		goto recv_again;
	}

	xdrmem_create(&xstream, buff, res, XDR_DECODE);
	mreply->b.reply.r.accepted.d.result.decoder = outproc;
	mreply->b.reply.r.accepted.d.result.param = out;
	if (!xdr_rpc_msg(&xstream, mreply)) {
		clnt->err.status = RPC_CANTDECODERES;
		goto exit_with_status;
	}
	xdr_destroy(&xstream);

	clnt->err.status = RPC_SUCCESS;
exit_with_status:
	return clnt->err.status;
}

static void clntudp_geterr(struct client *clnt, struct rpc_err *perr) {
	assert((clnt != NULL) && (perr != NULL));

	memcpy(perr, &clnt->err, sizeof clnt->err);
}

static void clntudp_destroy(struct client *clnt) {
	if (clnt != NULL) {
		auth_destroy(clnt->ath);
		close(clnt->sock);
		free(clnt);
	}
}

static const struct clnt_ops clntudp_ops = {
		.cl_call = clntudp_call,
		.cl_geterr = clntudp_geterr,
		.cl_destroy = clntudp_destroy
};
