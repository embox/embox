/**
 * @file
 * @brief
 * @date 28.05.12
 * @author Ilia Vaprol
 */

#include <net/rpc/rpc.h>
#include <net/rpc/clnt.h>
#include <net/rpc/auth.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <net/in.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


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
		raddr.sin_port = htons(725);
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

void clnt_geterr(struct client *clnt, struct rpc_err *perr) {
	assert(clnt != NULL);
	assert(clnt->ops != NULL);
	assert(clnt->ops->cl_geterr != NULL);

	return (*clnt->ops->cl_geterr)(clnt, perr);
}

void clnt_destroy(struct client *clnt) {
	assert(clnt != NULL);
	assert(clnt->ops != NULL);
	assert(clnt->ops->cl_destroy != NULL);

	return (*clnt->ops->cl_destroy)(clnt);
}


/*
 * rpc perror api
 */

/* List of RPC and AUTH errors */
static struct { enum auth_stat err; char *str_err; } auth_errlist[] = {
		{ AUTH_OK, "Authentication OK" },
		{ AUTH_BADCRED, "Invalid client credential" },
		{ AUTH_REJECTEDCRED, "Server rejected credential" },
		{ AUTH_BADVERF, "Invalid client verifier" },
		{ AUTH_REJECTEDVERF, "Server rejected verifier" },
		{ AUTH_TOOWEAK, "Client credential too weak" },
		{ AUTH_INVALIDRESP, "Invalid server verifier" },
		{ AUTH_FAILED, "Failed (unspecified error)" }
};

static struct { enum clnt_stat err; char *str_err; } rpc_errlist[] = {
		{ RPC_SUCCESS, "RPC: Success" },
		{ RPC_CANTENCODEARGS, "RPC: Can't encode arguments" },
		{ RPC_CANTDECODERES, "RPC: Can't decode result" },
		{ RPC_CANTSEND, "RPC: Unable to send" },
		{ RPC_CANTRECV, "RPC: Unable to receive" },
		{ RPC_TIMEDOUT, "RPC: Timed out" },
		{ RPC_VERSMISMATCH, "RPC: Incompatible versions of RPC" },
		{ RPC_AUTHERROR, "RPC: Authentication error" },
		{ RPC_PROGUNAVAIL, "RPC: Program unavailable" },
		{ RPC_PROGVERSMISMATCH, "RPC: Program/version mismatch" },
		{ RPC_PROCUNAVAIL, "RPC: Procedure unavailable" },
		{ RPC_CANTDECODEARGS, "RPC: Server can't decode arguments" },
		{ RPC_SYSTEMERROR, "RPC: Remote system error" },
		{ RPC_UNKNOWNHOST, "RPC: Unknown host" },
		{ RPC_PMAPFAILURE, "RPC: Port mapper failure" },
		{ RPC_PROGNOTREGISTERED, "RPC: Program not registered" },
		{ RPC_FAILED, "RPC: Failed (unspecified error)" },
		{ RPC_UNKNOWNPROTO, "RPC: Unknown protocol" }
};

static char * auth_errmsg(enum auth_stat stat) {
	size_t i;

	for (i = 0; i < sizeof auth_errlist / sizeof auth_errlist[0]; ++i) {
		if (auth_errlist[i].err == stat) {
			return auth_errlist[i].str_err;
		}
	}

	return NULL;
}

char * clnt_sperrno(enum clnt_stat stat) {
	size_t i;

	for (i = 0; i < sizeof rpc_errlist / sizeof rpc_errlist[0]; ++i) {
		if (rpc_errlist[i].err == stat) {
			return rpc_errlist[i].str_err;
		}
	}

	return "RPC: (unknown error code)";
}

void clnt_perrno(enum clnt_stat stat) {
	printf("%s\n", clnt_sperrno(stat));
}

char * clnt_spcreateerror(const char *s) {
	return "";
}

void clnt_pcreateerror(const char *s) {
	printf("%s\n", clnt_spcreateerror(s));
}

void clnt_perror(struct client *clnt, const char *s) {
	char *err;

	printf("%s: %s", s, clnt_sperrno(clnt->err.status));
	switch (clnt->err.status) {
	case RPC_SUCCESS:
	case RPC_CANTENCODEARGS:
	case RPC_CANTDECODERES:
	case RPC_TIMEDOUT:
	case RPC_PROGUNAVAIL:
	case RPC_PROCUNAVAIL:
	case RPC_CANTDECODEARGS:
	case RPC_SYSTEMERROR:
	case RPC_UNKNOWNHOST:
	case RPC_UNKNOWNPROTO:
	case RPC_PMAPFAILURE:
	case RPC_PROGNOTREGISTERED:
	case RPC_FAILED:
		break;
	case RPC_CANTSEND:
	case RPC_CANTRECV:
		printf("; errno = %s", strerror(clnt->err.extra.code));
		break;
	case RPC_VERSMISMATCH:
		printf("; low version = %u, high version = %u",
				clnt->err.extra.vers.low, clnt->err.extra.vers.high);
		break;
	case RPC_AUTHERROR:
		printf("; why = ");
		err = auth_errmsg(clnt->err.extra.reason);
		if (err != NULL) {
			printf("%s", err);
		}
		else {
			printf("(unknown authentication error - %d)", clnt->err.extra.reason);
		}
		break;
	case RPC_PROGVERSMISMATCH:
		printf("; low version = %u, high version = %u",
				clnt->err.extra.vers.low, clnt->err.extra.vers.high);
		break;
	default:
		printf("; s1 = %u, s2 = %u", clnt->err.extra.vers.low,
				clnt->err.extra.vers.high);
		break;

	}
	printf("\n");
}
