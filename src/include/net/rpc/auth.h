/**
 * @file
 * @brief
 * @date 02.06.12
 * @author Ilia Vaprol
 */

#ifndef NET_RPC_AUTH_H_
#define NET_RPC_AUTH_H_

/* Errors of a authentication */
enum auth_stat {
	AUTH_OK = 0,
	AUTH_BADCRED = 1,      /* bad credentials (seal broken) */
	AUTH_REJECTEDCRED = 2, /* client must begin new session */
	AUTH_BADVERF = 3,      /* bad verifier (seal broken)    */
	AUTH_REJECTEDVERF = 4, /* verifier expired or replayed  */
	AUTH_TOOWEAK = 5,      /* rejected for security reasons */
	AUTH_INVALIDRESP = 6,  /* bogus response verifier */
	AUTH_FAILED = 7        /* some unknown reason */
};

enum auth_flavor {
	AUTH_NULL = 0,
	AUTH_UNIX = 1,
	AUTH_SHORT = 2,
	AUTH_DES = 3
};

struct opaque_auth {
	enum auth_flavor flavor;
	__u32 len;
	char *data; // FIXME not used
};

#endif /* NET_RPC_AUTH_H_ */
