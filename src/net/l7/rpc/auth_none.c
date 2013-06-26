/**
 * @file
 * @brief
 * @date 04.06.12
 * @author Ilia Vaprol
 */

#include <net/l7/rpc/auth.h>
#include <stdlib.h>
#include <string.h>

static struct auth_ops authnone_ops;

struct auth * authnone_create(void) {
	struct auth *ath;

	ath = (struct auth *)malloc(sizeof *ath);
	if (ath == NULL) {
		return NULL;
	}

	ath->ops = &authnone_ops;
	memcpy(&ath->cred, &__opaque_auth_null, sizeof __opaque_auth_null);
	memcpy(&ath->verf, &__opaque_auth_null, sizeof __opaque_auth_null);

	return ath;
}

static void authnone_destroy(struct auth *ath) {
	free(ath);
}

static struct auth_ops authnone_ops = {
		.destroy = authnone_destroy
};
