/**
 * @file
 * @brief Useful functions for manipulations with a netent structure
 *
 * @date 29.03.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <framework/mod/options.h>
#include <net/util/netent.h>
#include <netdb.h>
#include <stddef.h>
#include <string.h>
#include <lib/libds/array.h>

#define MODOPS_MAX_NAME_LEN OPTION_GET(NUMBER, max_name_len)
#define MODOPS_MAX_ALIASES_NUM OPTION_GET(NUMBER, max_aliases_num)

static struct netent nentry_storage;
static char nentry_name_storage[MODOPS_MAX_NAME_LEN];
static char *nentry_aliases[MODOPS_MAX_ALIASES_NUM + 1]; /* for null-termanated */
static size_t nentry_aliases_sz;
static char nentry_aliases_storage[MODOPS_MAX_ALIASES_NUM][MODOPS_MAX_NAME_LEN];

struct netent * netent_create(void) {
	memset(&nentry_storage, 0, sizeof nentry_storage);
	nentry_storage.n_aliases = &nentry_aliases[0];
	nentry_aliases[0] = NULL;
	nentry_aliases_sz = 0;

	return &nentry_storage;
}

int netent_set_name(struct netent *ne, const char *name) {
	size_t name_sz;

	if ((ne == NULL) || (name == NULL)) {
		return -EINVAL;
	}

	name_sz = strlen(name) + 1;
	if (name_sz > ARRAY_SIZE(nentry_name_storage)) {
		return -ENOMEM;
	}

	memcpy(&nentry_name_storage[0], name, name_sz);

	ne->n_name = &nentry_name_storage[0];

	return 0;
}

int netent_add_alias(struct netent *ne, const char *alias) {
	size_t alias_sz;

	if ((ne == NULL) || (alias == NULL)) {
		return -EINVAL;
	}

	if (nentry_aliases_sz >= ARRAY_SIZE(nentry_aliases_storage)) {
		return -ENOMEM;
	}

	alias_sz = strlen(alias) + 1;
	if (alias_sz > ARRAY_SIZE(nentry_aliases_storage[0])) {
		return -ENOMEM;
	}

	memcpy(&nentry_aliases_storage[nentry_aliases_sz][0], alias, alias_sz);

	nentry_aliases[nentry_aliases_sz] = &nentry_aliases_storage[nentry_aliases_sz][0];
	nentry_aliases[++nentry_aliases_sz] = NULL;

	return 0;
}

int netent_set_addr_info(struct netent *ne, int addrtype) {
	if (ne == NULL) {
		return -EINVAL;
	}

	ne->n_addrtype = addrtype;

	return 0;
}

int netent_set_net(struct netent *ne, uint32_t net) {
	if (ne == NULL) {
		return -EINVAL;
	}

	ne->n_net = net;

	return 0;
}
