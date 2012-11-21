/**
 * @file
 * @brief portmapper
 * @date 03.06.12
 * @author Ilia Vaprol
 */

#include <net/rpc/pmap.h>
#include <net/rpc/xdr.h>
#include <net/rpc/clnt.h>
#include <net/rpc/rpc.h>
#include <types.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>

__u16 pmap_getport(struct sockaddr_in *raddr, __u32 prognum,
		__u32 versnum, __u32 protocol) {
	const struct timeval resend_every = { 5, 0 };
	const struct timeval timeout = { 60, 0 };
	__u16 port;
	int sock;
	struct client *clnt;
	struct sockaddr_in addr;
	struct pmap pm;

	memcpy(&addr, raddr, sizeof *raddr);
	addr.sin_port = htons(PMAPPORT);
	sock = RPC_ANYSOCK;
	clnt = clntudp_create(&addr, PMAPPROG, PMAPVERS,
			resend_every, &sock);
	if (clnt == NULL) {
		return 0;
	}

	pm.prog = prognum;
	pm.vers = versnum;
	pm.prot = protocol;
	port = 0;
	if (clnt_call(clnt, PMAPPROC_GETPORT,
			(xdrproc_t)xdr_pmap, (char *)&pm,
			(xdrproc_t)xdr_u_short, (char *)&port,
			timeout) != RPC_SUCCESS) {
		rpc_create_error.stat = RPC_PMAPFAILURE;
		clnt_geterr(clnt, &rpc_create_error.err);
		port = 0;
	} else if (port == 0) {
		rpc_create_error.stat = RPC_PROGNOTREGISTERED;
	}

	clnt_destroy(clnt);

	return port;
}

int xdr_pmap(struct xdr *xs, struct pmap *pmp) {
	assert(pmp != NULL);

	if (xdr_u_int(xs, &pmp->prog) && xdr_u_int(xs, &pmp->vers)
			&& xdr_u_int(xs, &pmp->prot) && xdr_u_int(xs, &pmp->port)) {
		return XDR_SUCCESS;
	}

	return XDR_FAILURE;
}
