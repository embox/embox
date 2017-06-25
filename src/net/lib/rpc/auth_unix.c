/**
 * @file
 * @brief
 * @date 07.06.12
 * @author Ilia Vaprol
 */

#include <net/lib/rpc/auth.h>
#include <net/lib/rpc/xdr.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static struct auth_ops authunix_ops;

struct auth * authunix_create(char *host, int uid, int gid,
		int user_gids_len, int *user_gids) {
	struct auth *ath;
	char *data;
	struct xdr xstream;
	struct authunix_parms aup;
	__u32 data_len;

	assert((host != NULL) && (user_gids_len >= 0)
			&& ((user_gids_len == 0) || (user_gids != NULL)));

	ath = auth_alloc(), data = malloc(AUTH_DATA_MAX_SZ);
	if ((ath == NULL) || (data == NULL)) {
		goto exit_with_error;
	}

	aup.stamp = (__u32)rand(); /* get unique id */
	aup.host = host;
	aup.uid = uid;
	aup.gid = gid;
	aup.gids = (__u32 *)user_gids;
	aup.gids_len = user_gids_len;

	xdrmem_create(&xstream, data, AUTH_DATA_MAX_SZ, XDR_ENCODE);
	if (!xdr_authunix_parms(&xstream, &aup)) {
		xdr_destroy(&xstream);
		goto exit_with_error;
	}
	data_len = xdr_getpos(&xstream);
	xdr_destroy(&xstream);

	ath->ops = &authunix_ops;
	ath->cred.flavor = AUTH_UNIX;
	ath->cred.data = data;
	ath->cred.data_len = data_len;
	memcpy(&ath->verf, &__opaque_auth_null, sizeof __opaque_auth_null);

	return ath;
exit_with_error:
	auth_free(ath);
	free(data);
	return NULL;
}

static void authunix_destroy(struct auth *ath) {
	auth_free(ath);
}

static struct auth_ops authunix_ops = {
		.destroy = authunix_destroy
};
