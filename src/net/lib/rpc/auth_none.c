/**
 * @file
 * @brief
 * @date 04.06.12
 * @author Ilia Vaprol
 */

#include <net/lib/rpc/auth.h>
#include <stdlib.h>
#include <string.h>

static struct auth_ops authnone_ops;

struct auth * authnone_create(void) {
	struct auth *ath;

	ath = auth_alloc();
	if (ath == NULL) {
		return NULL;
	}

	ath->ops = &authnone_ops;
	memcpy(&ath->cred, &__opaque_auth_null, sizeof __opaque_auth_null);
	memcpy(&ath->verf, &__opaque_auth_null, sizeof __opaque_auth_null);

	return ath;
}

static void authnone_destroy(struct auth *ath) {
	auth_free(ath);
}

static struct auth_ops authnone_ops = {
		.destroy = authnone_destroy
};
