/**
 * @file
 * @brief Useful functions for manipulations with a servent structure
 *
 * @date 29.03.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <errno.h>
#include <framework/mod/options.h>
#include <net/util/servent.h>
#include <netdb.h>
#include <stddef.h>
#include <string.h>
#include <lib/libds/array.h>

#define MODOPS_MAX_NAME_LEN OPTION_GET(NUMBER, max_name_len)
#define MODOPS_MAX_ALIASES_NUM OPTION_GET(NUMBER, max_aliases_num)
#define MODOPS_MAX_PROTO_LEN OPTION_GET(NUMBER, max_proto_len)

static struct servent sentry_storage;
static char sentry_name_storage[MODOPS_MAX_NAME_LEN];
static char *sentry_aliases[MODOPS_MAX_ALIASES_NUM + 1]; /* for null-termanated */
static size_t sentry_aliases_sz;
static char sentry_aliases_storage[MODOPS_MAX_ALIASES_NUM][MODOPS_MAX_NAME_LEN];
static char sentry_proto_storage[MODOPS_MAX_PROTO_LEN];

struct servent * servent_create(void) {
	memset(&sentry_storage, 0, sizeof sentry_storage);
	sentry_storage.s_aliases = &sentry_aliases[0];
	sentry_aliases[0] = NULL;
	sentry_aliases_sz = 0;

	return &sentry_storage;
}

int servent_set_name(struct servent *se, const char *name) {
	size_t name_sz;

	if ((se == NULL) || (name == NULL)) {
		return -EINVAL;
	}

	name_sz = strlen(name) + 1;
	if (name_sz > ARRAY_SIZE(sentry_name_storage)) {
		return -ENOMEM;
	}

	memcpy(&sentry_name_storage[0], name, name_sz);

	se->s_name = &sentry_name_storage[0];

	return 0;
}

int servent_add_alias(struct servent *se, const char *alias) {
	size_t alias_sz;

	if ((se == NULL) || (alias == NULL)) {
		return -EINVAL;
	}

	if (sentry_aliases_sz >= ARRAY_SIZE(sentry_aliases_storage)) {
		return -ENOMEM;
	}

	alias_sz = strlen(alias) + 1;
	if (alias_sz > ARRAY_SIZE(sentry_aliases_storage[0])) {
		return -ENOMEM;
	}

	memcpy(&sentry_aliases_storage[sentry_aliases_sz][0], alias, alias_sz);

	sentry_aliases[sentry_aliases_sz] = &sentry_aliases_storage[sentry_aliases_sz][0];
	sentry_aliases[++sentry_aliases_sz] = NULL;

	return 0;
}

int servent_set_port(struct servent *se, int port) {
	if (se == NULL) {
		return -EINVAL;
	}

	se->s_port = htons(port);

	return 0;
}

int servent_set_proto(struct servent *se, const char *proto) {
	size_t proto_sz;

	if ((se == NULL) || (proto == NULL)) {
		return -EINVAL;
	}

	proto_sz = strlen(proto) + 1;
	if (proto_sz > ARRAY_SIZE(sentry_proto_storage)) {
		return -ENOMEM;
	}

	memcpy(&sentry_proto_storage[0], proto, proto_sz);

	se->s_proto = &sentry_proto_storage[0];

	return 0;
}

struct servent * servent_make(const char *name, int port,
		const char *proto) {
	struct servent *se;

	se = servent_create();
	if (se == NULL) {
		return NULL; /* error: no memory */
	}

	if (name != NULL) {
		if (0 != servent_set_name(se, name)) {
			return NULL; /* error: see ret */
		}
	}

	if (0 != servent_set_port(se, port)) {
		return NULL; /* error: see ret */
	}

	if (proto != NULL) {
		if (0 != servent_set_proto(se, proto)) {
			return NULL; /* error: see ret */
		}
	}

	return se;
}
