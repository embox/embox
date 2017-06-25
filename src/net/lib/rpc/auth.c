/**
 * @file
 * @brief
 * @date 04.06.12
 * @author Ilia Vaprol
 */

#include <net/lib/rpc/auth.h>
#include <net/lib/rpc/xdr.h>
#include <assert.h>
#include <stddef.h>

const struct opaque_auth __opaque_auth_null = {
		.flavor = AUTH_NULL,
		.data = NULL,
		.data_len = 0
};

void auth_destroy(struct auth *ath) {
	if (ath != NULL) {
		assert(ath->ops != NULL);
		assert(ath->ops->destroy != NULL);

		(*ath->ops->destroy)(ath);
	}
}

int xdr_opaque_auth(struct xdr *xs, struct opaque_auth *oa) {
	int32_t oa_flavor;

	assert(oa != NULL);

	oa_flavor = oa->flavor;
	if (!xdr_enum(xs, &oa_flavor)
			|| !xdr_bytes(xs, &oa->data, &oa->data_len, AUTH_DATA_MAX_SZ)) {
		return XDR_FAILURE;
	}

	oa->flavor = oa_flavor;

	return XDR_SUCCESS;
}

int xdr_authunix_parms(struct xdr *xs, struct authunix_parms *aup) {
	assert(aup != NULL);

	return xdr_u_int(xs, &aup->stamp)
			&& xdr_string(xs, &aup->host, HOST_NAME_MAX_SZ)
			&& xdr_u_int(xs, &aup->uid)
			&& xdr_u_int(xs, &aup->gid)
			&& xdr_array(xs, (char **)&aup->gids, &aup->gids_len,
					GIDS_MAX_SZ, sizeof *aup->gids, (xdrproc_t)xdr_u_int);
}
