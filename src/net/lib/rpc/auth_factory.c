/**
 * @file
 * @brief
 * @date 07.02.14
 * @author Ilia Vaprol
 */

#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <net/lib/rpc/auth.h>

#define MODOPS_AMOUNT_AUTH OPTION_GET(NUMBER, amount_auth)

POOL_DEF(auth_pool, struct auth, MODOPS_AMOUNT_AUTH);

struct auth * auth_alloc(void) {
	return pool_alloc(&auth_pool);
}

void auth_free(struct auth *ath) {
	pool_free(&auth_pool, ath);
}
