/**
 * @file
 * @brief Useful functions for manipulations with a hostent structure
 *
 * @date 20.08.12
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <framework/mod/options.h>
#include <net/util/hostent.h>
#include <netdb.h>
#include <stddef.h>
#include <string.h>
#include <lib/libds/array.h>

#define MODOPS_MAX_NAME_LEN OPTION_GET(NUMBER, max_name_len)
#define MODOPS_MAX_ALIASES_NUM OPTION_GET(NUMBER, max_aliases_num)
#define MODOPS_MAX_ADDR_LEN OPTION_GET(NUMBER, max_addr_len)
#define MODOPS_MAX_ADDRS_NUM OPTION_GET(NUMBER, max_addrs_num)

static struct hostent hentry_storage;
static char hentry_name_storage[MODOPS_MAX_NAME_LEN];
static char *hentry_aliases[MODOPS_MAX_ALIASES_NUM + 1]; /* for null-termanated */
static size_t hentry_aliases_sz;
static char hentry_aliases_storage[MODOPS_MAX_ALIASES_NUM][MODOPS_MAX_NAME_LEN];
static char *hentry_addrs[MODOPS_MAX_ADDRS_NUM + 1]; /* for null-terminated */
static size_t hentry_addrs_sz;
static char hentry_addrs_storage[MODOPS_MAX_ADDRS_NUM][MODOPS_MAX_ADDR_LEN];

struct hostent * hostent_create(void) {
	memset(&hentry_storage, 0, sizeof hentry_storage);
	hentry_storage.h_aliases = &hentry_aliases[0];
	hentry_storage.h_addr_list = &hentry_addrs[0];
	hentry_aliases[0] = hentry_addrs[0] = NULL;
	hentry_aliases_sz = hentry_addrs_sz = 0;

	return &hentry_storage;
}

void hostent_destroy(struct hostent *he) {
	(void)he;
}

int hostent_set_name(struct hostent *he, const char *name) {
	size_t name_sz;

	if ((he == NULL) || (name == NULL)) {
		return -EINVAL;
	}

	name_sz = strlen(name) + 1;
	if (name_sz > ARRAY_SIZE(hentry_name_storage)) {
		return -ENOMEM;
	}

	memcpy(&hentry_name_storage[0], name, name_sz);

	he->h_name = &hentry_name_storage[0];

	return 0;
}

int hostent_add_alias(struct hostent *he, const char *alias) {
	size_t alias_sz;

	if ((he == NULL) || (alias == NULL)) {
		return -EINVAL;
	}

	if (hentry_aliases_sz >= ARRAY_SIZE(hentry_aliases_storage)) {
		return -ENOMEM;
	}

	alias_sz = strlen(alias) + 1;
	if (alias_sz > ARRAY_SIZE(hentry_aliases_storage[0])) {
		return -ENOMEM;
	}

	memcpy(&hentry_aliases_storage[hentry_aliases_sz][0], alias, alias_sz);

	hentry_aliases[hentry_aliases_sz] = &hentry_aliases_storage[hentry_aliases_sz][0];
	hentry_aliases[++hentry_aliases_sz] = NULL;

	return 0;
}

int hostent_set_addr_info(struct hostent *he, int addrtype, int addrlen) {
	if ((he == NULL) || (addrlen <= 0)) {
		return -EINVAL;
	}

	if (addrlen > ARRAY_SIZE(hentry_addrs_storage[0])) {
		return -ENOMEM;
	}

	he->h_addrtype = addrtype;
	he->h_length = addrlen;

	return 0;
}

int hostent_add_addr(struct hostent *he, const void *addr) {
	if ((he == NULL) || (addr == NULL)) {
		return -EINVAL;
	}

	if (hentry_addrs_sz >= ARRAY_SIZE(hentry_addrs_storage)) {
		return -ENOMEM;
	}

	memcpy(&hentry_addrs_storage[hentry_addrs_sz][0], addr, he->h_length);

	hentry_addrs[hentry_addrs_sz] = &hentry_addrs_storage[hentry_addrs_sz][0];
	hentry_addrs[++hentry_addrs_sz] = NULL;

	return 0;
}

struct hostent * hostent_make(const char *name, int addrtype,
		int addrlen, const void *addr) {
	struct hostent *he;

	he = hostent_create();
	if (he == NULL) {
		return NULL; /* error: no memory */
	}

	if (name != NULL) {
		if (0 != hostent_set_name(he, name)) {
			return NULL; /* error: see ret */
		}
	}

	if (0 != hostent_set_addr_info(he, addrtype, addrlen)) {
		return NULL; /* error: see ret */
	}

	if (addr != NULL) {
		if (0 != hostent_add_addr(he, addr)) {
			return NULL; /* error: see ret */
		}
	}

	return he;
}
