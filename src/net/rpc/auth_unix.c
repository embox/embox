/**
 * @file
 * @brief
 * @date 07.06.12
 * @author Ilia Vaprol
 */

#include <net/rpc/auth.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

static struct auth_ops authunix_ops;

struct auth * authunix_create(char *host, int uid, int gid,
		int len, int *aup_gids) {
	struct auth *ath;

	ath = (struct auth *)malloc(sizeof *ath);
	if (ath == NULL) {
		return NULL;
	}

	ath->ops = &authunix_ops;

	memcpy(&ath->cred, &__opaque_auth_null, sizeof __opaque_auth_null);
	memcpy(&ath->verf, &__opaque_auth_null, sizeof __opaque_auth_null);

	return ath;
}

static void authunix_destroy(struct auth *ath) {
	free(ath);
}

static struct auth_ops authunix_ops = {
		.destroy = authunix_destroy
};
