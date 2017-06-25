/**
 * @file
 * @brief
 * @date 02.06.12
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_RPC_AUTH_H_
#define NET_LIB_RPC_AUTH_H_

#include <stdint.h>

/* Prototypes */
struct xdr;
struct auth;

#define AUTH_DATA_MAX_SZ 400
#define HOST_NAME_MAX_SZ 255
#define GIDS_MAX_SZ      16

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
	uint32_t data_len;
};

struct auth_ops {
	void (*destroy)(struct auth *);
};

struct auth {
	struct opaque_auth cred;
	struct opaque_auth verf;
	const struct auth_ops *ops;
};

struct authunix_parms {
	uint32_t stamp;
	char *host;
	uint32_t uid;
	uint32_t gid;
	uint32_t *gids;
	uint32_t gids_len;
};

extern const struct opaque_auth __opaque_auth_null;

/* Auth factory */
extern struct auth * auth_alloc(void);
extern void auth_free(struct auth *ath);

extern struct auth * authnone_create(void);
extern struct auth * authunix_create(char *host, int uid, int gid,
		int user_gids_len, int *user_gids);

extern void auth_destroy(struct auth *ath);

extern int xdr_opaque_auth(struct xdr *xs, struct opaque_auth *oa);
extern int xdr_authunix_parms(struct xdr *xs, struct authunix_parms *aup);

#endif /* NET_LIB_RPC_AUTH_H_ */
