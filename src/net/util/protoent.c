/**
 * @file
 * @brief Useful functions for manipulations with a protoent structure
 *
 * @date 29.03.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <framework/mod/options.h>
#include <net/util/protoent.h>
#include <netdb.h>
#include <stddef.h>
#include <string.h>
#include <lib/libds/array.h>

#define MODOPS_MAX_NAME_LEN OPTION_GET(NUMBER, max_name_len)
#define MODOPS_MAX_ALIASES_NUM OPTION_GET(NUMBER, max_aliases_num)

static struct protoent pentry_storage;
static char pentry_name_storage[MODOPS_MAX_NAME_LEN];
static char *pentry_aliases[MODOPS_MAX_ALIASES_NUM + 1]; /* for null-termanated */
static size_t pentry_aliases_sz;
static char pentry_aliases_storage[MODOPS_MAX_ALIASES_NUM][MODOPS_MAX_NAME_LEN];

struct protoent * protoent_create(void) {
	memset(&pentry_storage, 0, sizeof pentry_storage);
	pentry_storage.p_aliases = &pentry_aliases[0];
	pentry_aliases[0] = NULL;
	pentry_aliases_sz = 0;

	return &pentry_storage;
}

int protoent_set_name(struct protoent *pe, const char *name) {
	size_t name_sz;

	if ((pe == NULL) || (name == NULL)) {
		return -EINVAL;
	}

	name_sz = strlen(name) + 1;
	if (name_sz > ARRAY_SIZE(pentry_name_storage)) {
		return -ENOMEM;
	}

	memcpy(&pentry_name_storage[0], name, name_sz);

	pe->p_name = &pentry_name_storage[0];

	return 0;
}

int protoent_add_alias(struct protoent *pe, const char *alias) {
	size_t alias_sz;

	if ((pe == NULL) || (alias == NULL)) {
		return -EINVAL;
	}

	if (pentry_aliases_sz >= ARRAY_SIZE(pentry_aliases_storage)) {
		return -ENOMEM;
	}

	alias_sz = strlen(alias) + 1;
	if (alias_sz > ARRAY_SIZE(pentry_aliases_storage[0])) {
		return -ENOMEM;
	}

	memcpy(&pentry_aliases_storage[pentry_aliases_sz][0], alias, alias_sz);

	pentry_aliases[pentry_aliases_sz] = &pentry_aliases_storage[pentry_aliases_sz][0];
	pentry_aliases[++pentry_aliases_sz] = NULL;

	return 0;
}

int protoent_set_proto(struct protoent *pe, int proto) {
	if (pe == NULL) {
		return -EINVAL;
	}

	pe->p_proto = proto;

	return 0;
}
