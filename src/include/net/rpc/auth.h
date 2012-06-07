/**
 * @file
 * @brief
 * @date 02.06.12
 * @author Ilia Vaprol
 */

#ifndef NET_RPC_AUTH_H_
#define NET_RPC_AUTH_H_

#include <types.h>

/* Prototypes */
struct xdr;
struct auth;

#define MAX_AUTH_BYTES	400

/* Errors of a authentication */
enum auth_stat {
	AUTH_OK = 0,
	AUTH_BADCRED = 1,      /* bad credentials (seal broken) */
	AUTH_REJECTEDCRED = 2, /* client must begin new session */
	AUTH_BADVERF = 3,      /* bad verifier (seal broken)    */
	AUTH_REJECTEDVERF = 4, /* verifier expired or replayed  */
	AUTH_TOOWEAK = 5,      /* rejected for security reasons */
	AUTH_INVALIDRESP = 6,  /* bogus response verifier */
	AUTH_FAILED = 7,       /* some unknown reason */
	AUTH_MAX
};

enum auth_flavor {
	AUTH_NULL =  0,
	AUTH_UNIX = 1,
	AUTH_SHORT = 2,
	AUTH_DES = 3
};

struct opaque_auth {
	enum auth_flavor flavor;
	char *data;
	__u32 len;
};

struct auth_ops {
	void (*destroy)(struct auth *);
};

struct auth {
	struct opaque_auth cred;
	struct opaque_auth verf;
	const struct auth_ops *ops;
};

extern const struct opaque_auth __opaque_auth_null;

extern struct auth * authnone_create(void);

extern void auth_destroy(struct auth *ath);

extern int xdr_opaque_auth(struct xdr *xs, struct opaque_auth *oa);

#endif /* NET_RPC_AUTH_H_ */
