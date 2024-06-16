/**
 * @file
 * @brief
 * @date 28.05.12
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include <net/lib/rpc/auth.h>
#include <net/lib/rpc/clnt.h>
#include <net/lib/rpc/rpc.h>

#define ERROR_STR_MAX_SZ 256

struct rpc_createerr rpc_create_error; // TODO rename and move

/* List of AUTH errors */
static char *auth_errlist[AUTH_MAX] = {[AUTH_OK] = "Authentication OK",
    [AUTH_BADCRED] = "Invalid client credential",
    [AUTH_REJECTEDCRED] = "Server rejected credential",
    [AUTH_BADVERF] = "Invalid client verifier",
    [AUTH_REJECTEDVERF] = "Server rejected verifier",
    [AUTH_TOOWEAK] = "Client credential too weak",
    [AUTH_INVALIDRESP] = "Invalid server verifier",
    [AUTH_FAILED] = "Failed (unspecified error)"};

/* List of RPC errors */
static char *rpc_errlist[RPC_MAX] = {[RPC_SUCCESS] = "RPC: Success",
    [RPC_CANTENCODEARGS] = "RPC: Can't encode arguments",
    [RPC_CANTDECODERES] = "RPC: Can't decode result",
    [RPC_CANTSEND] = "RPC: Unable to send",
    [RPC_CANTRECV] = "RPC: Unable to receive",
    [RPC_TIMEDOUT] = "RPC: Timed out",
    [RPC_VERSMISMATCH] = "RPC: Incompatible versions of RPC",
    [RPC_AUTHERROR] = "RPC: Authentication error",
    [RPC_PROGUNAVAIL] = "RPC: Program unavailable",
    [RPC_PROGVERSMISMATCH] = "RPC: Program/version mismatch",
    [RPC_PROCUNAVAIL] = "RPC: Procedure unavailable",
    [RPC_CANTDECODEARGS] = "RPC: Server can't decode arguments",
    [RPC_SYSTEMERROR] = "RPC: Remote system error",
    [RPC_UNKNOWNHOST] = "RPC: Unknown host",
    [RPC_PMAPFAILURE] = "RPC: Port mapper failure",
    [RPC_PROGNOTREGISTERED] = "RPC: Program not registered",
    [RPC_FAILED] = "RPC: Failed (unspecified error)",
    [RPC_UNKNOWNPROTO] = "RPC: Unknown protocol"};

struct client *clnt_create(const char *host, __u32 prognum, __u32 versnum,
    const char *prot) {
	struct sockaddr_in raddr;
	struct timeval tv;
	int sock;

	sock = RPC_ANYSOCK;
	raddr.sin_family = AF_INET;
	inet_aton(host, &raddr.sin_addr);
	raddr.sin_port = 0;
	if (strcmp(prot, "udp") == 0) {
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		return clntudp_create(&raddr, prognum, versnum, tv, &sock);
	}
	else if (strcmp(prot, "tcp") == 0) {
		return clnttcp_create(&raddr, prognum, versnum, &sock, 0, 0);
	}

	rpc_create_error.stat = RPC_UNKNOWNPROTO;
	rpc_create_error.err.extra.error = EPFNOSUPPORT;

	return NULL; /* protocol not supported */
}

enum clnt_stat clnt_call(struct client *clnt, __u32 procnum, xdrproc_t inproc,
    char *in, xdrproc_t outproc, char *out, struct timeval wait) {
	assert(clnt != NULL);
	assert(clnt->ops != NULL);
	assert(clnt->ops->call != NULL);

	return (*clnt->ops->call)(clnt, procnum, inproc, in, outproc, out, wait);
}

void clnt_geterr(struct client *clnt, struct rpc_err *perr) {
	assert(clnt != NULL);
	assert(clnt->ops != NULL);
	assert(clnt->ops->geterr != NULL);

	(*clnt->ops->geterr)(clnt, perr);
}

void clnt_destroy(struct client *clnt) {
	if (clnt != NULL) {
		assert(clnt->ops != NULL);
		assert(clnt->ops->destroy != NULL);

		(*clnt->ops->destroy)(clnt);
	}
}

static char *get_auth_msg(enum auth_stat stat) {
	return (((0 <= stat) && (stat < AUTH_MAX)) ? auth_errlist[stat] : NULL);
}

char *clnt_spcreateerror(const char *msg) {
	/* FIXME should use snprintf instead sprintf */
	static char buff[ERROR_STR_MAX_SZ];
	char *curr;

	assert(msg != NULL);

	curr = buff;
	curr += sprintf(curr, "%s: %s", msg, clnt_sperrno(rpc_create_error.stat));
	switch (rpc_create_error.stat) {
	default:
		break;
	case RPC_PMAPFAILURE:
		curr += sprintf(curr, " - %s",
		    clnt_sperrno(rpc_create_error.err.status));
		break;
	case RPC_SYSTEMERROR:
		curr += sprintf(curr, " - %s",
		    strerror(rpc_create_error.err.extra.error));
		break;
	}

	assert(curr < buff + sizeof buff); // TODO remove this

	return buff;
}

char *clnt_sperrno(enum clnt_stat stat) {
	return (((0 <= stat) && (stat < RPC_MAX)) ? rpc_errlist[stat]
	                                          : "RPC: (unknown error code)");
}

char *clnt_sperror(struct client *clnt, const char *msg) {
	/* FIXME should use snprintf instead sprintf */
	static char buff[ERROR_STR_MAX_SZ];
	struct rpc_err err;
	char *curr, *auth_msg;

	assert((clnt != NULL) && (msg != NULL));

	clnt_geterr(clnt, &err);

	curr = buff;
	curr += sprintf(curr, "%s: %s", msg, clnt_sperrno(err.status));
	switch (err.status) {
	default:
		curr += sprintf(curr, "; s1 = %" PRIu32 ", s2 = %" PRIu32,
		    err.extra.mminfo.low, err.extra.mminfo.high);
		break;
	case RPC_SUCCESS:
	case RPC_CANTENCODEARGS:
	case RPC_CANTDECODERES:
	case RPC_TIMEDOUT:
	case RPC_PROGUNAVAIL:
	case RPC_PROCUNAVAIL:
	case RPC_CANTDECODEARGS:
	case RPC_SYSTEMERROR:
	case RPC_UNKNOWNHOST:
	case RPC_PMAPFAILURE:
	case RPC_PROGNOTREGISTERED:
	case RPC_FAILED:
	case RPC_UNKNOWNPROTO:
		break;
	case RPC_CANTSEND:
	case RPC_CANTRECV:
		curr += sprintf(curr, "; errno = %s", strerror(err.extra.error));
		break;
	case RPC_VERSMISMATCH:
		curr += sprintf(curr,
		    "; low version = %" PRIu32 ", high version = %" PRIu32,
		    err.extra.mminfo.low, err.extra.mminfo.high);
		break;
	case RPC_AUTHERROR:
		auth_msg = get_auth_msg(err.extra.reason);
		if (auth_msg != NULL) {
			curr += sprintf(curr, "; why = %s", auth_msg);
		}
		else {
			curr += sprintf(curr, "; why = (unknown authentication error - %d)",
			    err.extra.reason);
		}
		break;
	case RPC_PROGVERSMISMATCH:
		curr += sprintf(curr,
		    "; low version = %" PRIu32 ", high version = %" PRIu32,
		    err.extra.mminfo.low, err.extra.mminfo.high);
		break;
	}

	assert(curr < buff + sizeof buff); // TODO remove this

	return buff;
}

void clnt_pcreateerror(const char *msg) {
	fprintf(stderr, "%s\n", clnt_spcreateerror(msg));
}

void clnt_perrno(enum clnt_stat stat) {
	fprintf(stderr, "%s\n", clnt_sperrno(stat));
}

void clnt_perror(struct client *clnt, const char *msg) {
	fprintf(stderr, "%s\n", clnt_sperror(clnt, msg));
}
