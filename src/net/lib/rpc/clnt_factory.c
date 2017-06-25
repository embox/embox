/**
 * @file
 * @brief
 * @date 07.02.14
 * @author Ilia Vaprol
 */

#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <net/lib/rpc/clnt.h>

#define MODOPS_AMOUNT_CLNT OPTION_GET(NUMBER, amount_clnt)

POOL_DEF(clnt_pool, struct client, MODOPS_AMOUNT_CLNT);

struct client * clnt_alloc(void) {
	return pool_alloc(&clnt_pool);
}

void clnt_free(struct client *clnt) {
	pool_free(&clnt_pool, clnt);
}
