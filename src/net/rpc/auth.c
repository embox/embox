/**
 * @file
 * @brief
 * @date 04.06.12
 * @author Ilia Vaprol
 */

#include <net/rpc/auth.h>

const struct opaque_auth __auth_null = {
		.flavor = AUTH_NULL,
		.len = 0,
		.data = NULL
};

void auth_destroy(struct auth *ath) {
	assert(ath != NULL);
	assert(ath->ops != NULL);
	assert(ath->ops->ah_destroy != NULL);

	(*ath->ops->ah_destroy)(ath);
}
