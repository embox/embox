/**
 * @file
 * @brief
 * @date 28.05.12
 * @author Ilia Vaprol
 */


#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>

#include <net/lib/rpc/clnt.h>
#include <net/lib/rpc/auth.h>
#include <net/lib/rpc/rpc.h>
#include <net/lib/rpc/rpc_msg.h>
#include <net/lib/rpc/pmap.h>


#include <util/sys_log.h>

#include <kernel/time/ktime.h>

#define UDP_MSG_MAX_SZ 1024

static const struct clnt_ops clntudp_ops;

struct client * clntudp_create(struct sockaddr_in *raddr, uint32_t prognum,
		uint32_t versnum, struct timeval resend, int *psock) {
	struct client *clnt;
	struct auth *ath;
	int sock;
	uint16_t port;

	assert((raddr != NULL) && (psock != NULL));

	clnt = clnt_alloc(), ath = authnone_create();
	if ((clnt == NULL) || (ath == NULL)) {
		rpc_create_error.stat = RPC_SYSTEMERROR;
		rpc_create_error.err.extra.error = ENOMEM;
		goto exit_with_error;
	}

	port = ntohs(raddr->sin_port);
	if (port == 0) {
		port = pmap_getport(raddr, prognum, versnum, IPPROTO_UDP);
		if (port == 0) {
			goto exit_with_error;
		}
	}

	assert(*psock < 0); /* TODO remove this */
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		rpc_create_error.stat = RPC_SYSTEMERROR;
		rpc_create_error.err.extra.error = errno;
		goto exit_with_error;
	}

	/* Fill client structure */
	clnt->ops = &clntudp_ops;
	clnt->sock = sock;
	clnt->ath = ath;
	memcpy(&clnt->extra.udp.sin, raddr, sizeof *raddr);
	clnt->extra.udp.sin.sin_port = htons(port);
	clnt->prognum = prognum;
	clnt->versnum = versnum;
	memcpy(&clnt->extra.udp.resend, &resend, sizeof resend);

	*psock = sock;

	return clnt;
exit_with_error:
	auth_destroy(ath);
	clnt_free(clnt);
	return NULL;
}

static enum clnt_stat clntudp_call(struct client *clnt, uint32_t procnum,
		xdrproc_t inproc, char *in, xdrproc_t outproc, char *out,
		struct timeval timeout) {
	int res;
	char buff[UDP_MSG_MAX_SZ];
	struct xdr xstream;
	struct rpc_msg msg_reply, msg_call;
	size_t buff_len;
	struct timeval tmp, until, sent, elapsed;

	assert((clnt != NULL) && (inproc != NULL));

	msg_call.xid = (uint32_t)rand();
	msg_call.type = CALL;
	msg_call.b.call.rpcvers = RPC_VERSION;
	msg_call.b.call.prog = clnt->prognum;
	msg_call.b.call.vers = clnt->versnum;
	msg_call.b.call.proc = procnum;
	memcpy(&msg_call.b.call.cred, &clnt->ath->cred, sizeof clnt->ath->cred);
	memcpy(&msg_call.b.call.verf, &clnt->ath->verf, sizeof clnt->ath->verf);

	xdrmem_create(&xstream, buff, sizeof buff, XDR_ENCODE);
	if (!xdr_rpc_msg(&xstream, &msg_call)
			|| !(*inproc)(&xstream, in)) {
		clnt->err.status = RPC_CANTENCODEARGS;
		xdr_destroy(&xstream);
		goto exit_with_status;
	}
	buff_len = xdr_getpos(&xstream);
	xdr_getpos(&xstream);
	xdr_destroy(&xstream);

	if (-1 == fcntl(clnt->sock, F_SETFL, O_NONBLOCK)) {
		clnt->err.status = RPC_SYSTEMERROR;
		clnt->err.extra.error = errno;
		goto exit_with_status;
	}

	ktime_get_timeval(&tmp);
	timeradd(&tmp, &timeout, &until);

send_again:
	res = sendto(clnt->sock, buff, buff_len, 0,
			(struct sockaddr *)&clnt->extra.udp.sin, sizeof clnt->extra.udp.sin);
	ktime_get_timeval(&sent);
	if (res == -1) {
		if (errno != EAGAIN) {
			LOG_ERROR("clntudp_call", "sendto error");
			clnt->err.status = RPC_CANTSEND;
			clnt->err.extra.error = errno;
			goto exit_with_status;
		}
		if (timercmp(&sent, &until, >=)) {
			LOG_ERROR("clntudp_call", "sendto timeout error");
			clnt->err.status = RPC_TIMEDOUT;
			goto exit_with_status;
		}
		goto send_again;
	}

recv_again:
	res = recvfrom(clnt->sock, buff, sizeof buff, 0, NULL, NULL);
	ktime_get_timeval(&tmp);
	if (res == -1) {
		if (errno != EAGAIN) {
			LOG_ERROR("clntudp_call", "recvfrom error");
			clnt->err.status = RPC_CANTRECV;
			clnt->err.extra.error = errno;
			goto exit_with_status;
		}
		if (timercmp(&tmp, &until, >=)) {
			LOG_ERROR("clntudp_call", "recvfrom timeout error");
			clnt->err.status = RPC_TIMEDOUT;
			goto exit_with_status;
		}
		timersub(&tmp, &sent, &elapsed);
		if (timercmp(&elapsed, &clnt->extra.udp.resend, >=)) {
			goto send_again;
		}
		goto recv_again;
	}

	xdrmem_create(&xstream, buff, res, XDR_DECODE);
	msg_reply.b.reply.r.accepted.d.result.decoder = outproc;
	msg_reply.b.reply.r.accepted.d.result.param = out;
	if (!xdr_rpc_msg(&xstream, &msg_reply)) {
		clnt->err.status = RPC_CANTDECODERES;
		xdr_destroy(&xstream);
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
	assert(clnt != NULL);

	auth_destroy(clnt->ath);
	close(clnt->sock);
	clnt_free(clnt);
}

static const struct clnt_ops clntudp_ops = {
		.call = clntudp_call,
		.geterr = clntudp_geterr,
		.destroy = clntudp_destroy
};
