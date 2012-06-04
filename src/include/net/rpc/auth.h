/**
 * @file
 * @brief
 * @date 02.06.12
 * @author Ilia Vaprol
 */

#ifndef NET_RPC_AUTH_H_
#define NET_RPC_AUTH_H_


#define MAX_AUTH_BYTES	400

/* Errors of a authentication */
enum auth_stat {
	AUTH_OK,
	AUTH_BADCRED,      /* bad credentials (seal broken) */
	AUTH_REJECTEDCRED, /* client must begin new session */
	AUTH_BADVERF,      /* bad verifier (seal broken)    */
	AUTH_REJECTEDVERF, /* verifier expired or replayed  */
	AUTH_TOOWEAK,      /* rejected for security reasons */
	AUTH_INVALIDRESP,  /* bogus response verifier */
	AUTH_FAILED,       /* some unknown reason */
	AUTH_MAX
};

enum auth_flavor {
	AUTH_NULL = 0,
	AUTH_UNIX = 1,
	AUTH_SHORT = 2,
	AUTH_DES = 3
};

struct opaque_auth {
	enum auth_flavor flavor;
	char *data;
	__u32 len;
};

struct auth;

struct auth_ops {
	void (*ah_destroy)(struct auth *);
};

struct auth {
	struct opaque_auth cred;
	struct opaque_auth verf;
	struct auth_ops *ops;
};

extern const struct opaque_auth __auth_null;

#endif /* NET_RPC_AUTH_H_ */
