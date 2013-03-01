/**
 * @file
 * @brief
 *
 * @date 27.02.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <mem/misc/pool.h>
#include <util/math.h>
#include <util/array.h>
#include <unistd.h>
#include <framework/mod/options.h>

#define MODOPS_ENV_AMOUNT OPTION_GET(NUMBER, env_amount)
#define MODOPS_ENV_STR_LEN OPTION_GET(NUMBER, env_str_len)

char **environ = NULL;

static char *env_vals[MODOPS_ENV_AMOUNT + 1];
POOL_DEF(env_pool, char [MODOPS_ENV_STR_LEN + 1], MODOPS_ENV_AMOUNT);
static size_t env_next = 0;
static char env_buff[MODOPS_ENV_STR_LEN + 1];

static size_t env_lookup(const char *name) {
	size_t i, name_len;

	assert(name != NULL);

	name_len = strlen(name);

	for (i = 0; i < env_next; ++i) {
		assert(env_vals[i] != NULL);
		if ((strncmp(name, env_vals[i], name_len) == 0)
				&& (env_vals[i][name_len] == '=')) {
			break;
		}
	}

	return i;
}

char * getenv(const char *name) {
	size_t index;

	if ((name == NULL) || (*name == '\0')
			|| (strchr(name, '=') != NULL)) {
		return NULL;
	}

	index = env_lookup(name);
	if (index == env_next) {
		return NULL;
	}

	assert(env_vals[index] != NULL);
	strcpy(&env_buff[0], env_vals[index] + strlen(name) + 1);

	return &env_buff[0];
}

int putenv(char *string) {
	size_t str_len, name_len, index;

	assert(string != NULL);
	assert(*string != '=');
	assert(strchr(string, '=') != NULL);

	name_len = strchr(string, '=') - string;
	memcpy(&env_buff[0], string, name_len);
	env_buff[name_len] = '\0';

	index = env_lookup(&env_buff[0]);
	str_len = strlen(string);

	if ((str_len >= MODOPS_ENV_STR_LEN)
			|| (index == ARRAY_SIZE(env_vals) - 1)) {
		SET_ERRNO(ENOMEM);
		return -1;
	}

	if (index == env_next) {
		env_vals[env_next] = pool_alloc(&env_pool);
		if (env_next == 0) {
			environ = &env_vals[0];
		}
		env_vals[++env_next] = NULL;
	}

	assert(env_vals[index] != NULL);
	memcpy(env_vals[index], string, str_len + 1);

	return 0;
}

int setenv(const char *name, const char *value, int overwrite) {
	size_t name_len, val_len, index;
	char *buff;

	if ((name == NULL) || (*name == '\0')
			|| (strchr(name, '=') != NULL)
			|| (value == NULL)) {
		SET_ERRNO(EINVAL);
		return -1;
	}

	index = env_lookup(name);
	if ((index < env_next) && !overwrite) {
		return 0;
	}

	name_len = strlen(name);
	val_len = strlen(value);

	if ((name_len + val_len >= MODOPS_ENV_STR_LEN)
			|| (index == ARRAY_SIZE(env_vals) - 1)) {
		SET_ERRNO(ENOMEM);
		return -1;
	}

	if (index == env_next) {
		env_vals[env_next] = pool_alloc(&env_pool);
		if (env_next == 0) {
			environ = &env_vals[0];
		}
		env_vals[++env_next] = NULL;
	}

	buff = env_vals[index];
	assert(buff != NULL);

	memcpy(buff, name, name_len);
	buff[name_len] = '=';
	memcpy(buff + name_len + 1, value, val_len  + 1);

	return 0;
}

int unsetenv(const char *name) {
	size_t index;

	if ((name == NULL) || (*name == '\0')
			|| (strchr(name, '=') != NULL)) {
		SET_ERRNO(EINVAL);
		return -1;
	}

	index = env_lookup(name);
	if ((index == env_next) || (env_next == 0)) {
		return 0;
	}

	pool_free(&env_pool, env_vals[index]);

	memmove(env_vals + index, env_vals + index + 1,
			(--env_next - index) * sizeof env_vals[0]);
	if (env_next != 0) {
		env_vals[env_next] = NULL;
	}
	else {
		environ = NULL;
	}

	return 0;
}

int clearenv(void) {
	size_t index;

	for (index = 0; index < env_next; ++index) {
		pool_free(&env_pool, env_vals[index]);
	}

	env_next = 0;
	environ = NULL;

	return 0;
}
