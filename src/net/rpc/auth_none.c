/**
 * @file
 * @brief
 * @date 04.06.12
 * @author Ilia Vaprol
 */

#include <stdlib.h>
#include <net/rpc/auth.h>
#include <string.h>
#include <stdlib.h>

static struct auth_ops authnone_ops;
static struct auth authnone_default = { .ops = NULL };

struct auth * authnone_create(void) {
	if (authnone_default.ops == NULL) {
		memcpy(&authnone_default.cred, &__opaque_auth_null, sizeof __opaque_auth_null);
		memcpy(&authnone_default.verf, &__opaque_auth_null, sizeof __opaque_auth_null);
		authnone_default.ops = &authnone_ops;
	}

	return &authnone_default;
}

static void authnone_destroy(struct auth *ath) {
	;
}

static struct auth_ops authnone_ops = {
		.destroy = authnone_destroy
};
