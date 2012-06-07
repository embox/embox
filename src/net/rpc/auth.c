/**
 * @file
 * @brief
 * @date 04.06.12
 * @author Ilia Vaprol
 */

#include <net/rpc/auth.h>
#include <net/rpc/xdr.h>
#include <assert.h>
#include <stdlib.h>
#include <types.h>

const struct opaque_auth __opaque_auth_null = {
		.flavor = AUTH_NULL,
		.len = 0,
		.data = NULL
};

void auth_destroy(struct auth *ath) {
	if (ath != NULL) {
		assert(ath->ops != NULL);
		assert(ath->ops->destroy != NULL);

		(*ath->ops->destroy)(ath);
	}
}

int xdr_opaque_auth(struct xdr *xs, struct opaque_auth *oa) {
	assert(oa != NULL);

	return xdr_enum(xs, (__s32 *)&oa->flavor)
			&& xdr_bytes(xs, &oa->data, &oa->len, MAX_AUTH_BYTES);
}
