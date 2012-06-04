/**
 * @file
 * @brief
 * @date 04.06.12
 * @author Ilia Vaprol
 */

#include <stdlib.h>
#include <net/rpc/auth.h>

static struct auth_ops authnone_ops;

struct auth * authnone_create(void) {
	struct auth *ath;

	ath = (struct auth *)malloc(sizeof *ath);
	if (ath == NULL) {
		return NULL;
	}

	memcpy(&ath->cred, &__auth_null, sizeof __auth_null);
	memcpy(&ath->verf, &__auth_null, sizeof __auth_null);
	ath->ops = &authnone_ops;

	return ath;
}

void authnone_destroy(struct auth *ath) {
	free(ath);
}

static struct auth_ops authnone_ops = {
		.ah_destroy = authnone_destroy
};
