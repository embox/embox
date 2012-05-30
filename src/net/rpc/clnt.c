/**
 * @file
 * @brief
 * @date 28.05.12
 * @author Ilia Vaprol
 */

#include <net/rpc/rpc.h>
#include <net/rpc/clnt.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <net/in.h>

struct client * clnt_create(const char *host, __u32 prognum,
		__u32 versnum, const char *prot) {
	struct sockaddr_in raddr;
	struct timeval tv;
	int sock;

	if (strcmp(prot, "udp") == 0) {
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		raddr.sin_family = AF_INET;
		inet_aton(host, &raddr.sin_addr);
		raddr.sin_port = htons(100);
		return clntudp_create(&raddr, prognum, versnum, tv, &sock);
	}

	return NULL; /* protocol not supported */
}

enum clnt_stat clnt_call(struct client *clnt, __u32 procnum, xdrproc_t inproc,
		char *in, xdrproc_t outproc, char *out, struct timeval wait) {
	assert(clnt != NULL);
	assert(clnt->ops != NULL);
	assert(clnt->ops->cl_call != NULL);

	return (*clnt->ops->cl_call)(clnt, procnum, inproc, in, outproc, out, wait);
}

void clnt_destroy(struct client *clnt) {
	assert(clnt != NULL);
	assert(clnt->ops != NULL);
	assert(clnt->ops->cl_destroy != NULL);

	return (*clnt->ops->cl_destroy)(clnt);
}
