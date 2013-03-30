/**
 * @file
 * @brief
 *
 * @date 27.02.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <mem/misc/pool.h>
#include <util/array.h>
#include <unistd.h>
#include <framework/mod/options.h>
#include <kernel/task/env.h>

#define MODOPS_ENV_AMOUNT OPTION_GET(NUMBER, env_amount)
#define MODOPS_ENV_STR_LEN OPTION_GET(NUMBER, env_str_len)

POOL_DEF(env_pool, char [MODOPS_ENV_STR_LEN + 1], MODOPS_ENV_AMOUNT);
static char env_buff[MODOPS_ENV_STR_LEN + 1];

static size_t env_lookup(const char *name, char **vals, size_t next) {
	size_t i, name_len;

	assert(name != NULL);
	assert(vals != NULL);

	name_len = strlen(name);

	for (i = 0; i < next; ++i) {
		assert(vals[i] != NULL);
		if ((strncmp(name, vals[i], name_len) == 0)
				&& (vals[i][name_len] == '=')) {
			break;
		}
	}

	return i;
}

char * getenv(const char *name) {
	size_t index;
	struct env_struct *env;

	if ((name == NULL) || (*name == '\0')
			|| (strchr(name, '=') != NULL)) {
		return NULL;
	}

	env = task_self_env_ptr();
	assert(env != NULL);

	index = env_lookup(name, &env->vals[0], env->next);
	if (index == env->next) {
		return NULL;
	}

	assert(env->vals[index] != NULL);
	strcpy(&env_buff[0], env->vals[index] + strlen(name) + 1);

	return &env_buff[0];
}

int putenv(char *string) {
	size_t str_len, name_len, index;
	struct env_struct *env;

	assert(string != NULL);
	assert(*string != '=');
	assert(strchr(string, '=') != NULL);

	name_len = strchr(string, '=') - string;
	memcpy(&env_buff[0], string, name_len);
	env_buff[name_len] = '\0';

	env = task_self_env_ptr();
	assert(env != NULL);

	index = env_lookup(&env_buff[0], &env->vals[0], env->next);
	str_len = strlen(string);

	if ((str_len >= MODOPS_ENV_STR_LEN)
			|| (index == ARRAY_SIZE(env->vals) - 1)) {
		SET_ERRNO(ENOMEM);
		return -1;
	}

	if (index == env->next) {
		env->vals[env->next] = pool_alloc(&env_pool);
		if (env->next == 0) {
			environ = &env->vals[0];
		}
		env->vals[++env->next] = NULL;
	}

	assert(env->vals[index] != NULL);
	memcpy(env->vals[index], string, str_len + 1);

	return 0;
}

int setenv(const char *name, const char *value, int overwrite) {
	size_t name_len, val_len, index;
	char *buff;
	struct env_struct *env;

	if ((name == NULL) || (*name == '\0')
			|| (strchr(name, '=') != NULL)
			|| (value == NULL)) {
		SET_ERRNO(EINVAL);
		return -1;
	}

	env = task_self_env_ptr();
	assert(env != NULL);

	index = env_lookup(name, &env->vals[0], env->next);
	if ((index < env->next) && !overwrite) {
		return 0;
	}

	name_len = strlen(name);
	val_len = strlen(value);

	if ((name_len + val_len >= MODOPS_ENV_STR_LEN)
			|| (index == ARRAY_SIZE(env->vals) - 1)) {
		SET_ERRNO(ENOMEM);
		return -1;
	}

	if (index == env->next) {
		env->vals[env->next] = pool_alloc(&env_pool);
		if (env->next == 0) {
			environ = &env->vals[0];
		}
		env->vals[++env->next] = NULL;
	}

	buff = env->vals[index];
	assert(buff != NULL);

	memcpy(buff, name, name_len);
	buff[name_len] = '=';
	memcpy(buff + name_len + 1, value, val_len  + 1);

	return 0;
}

int unsetenv(const char *name) {
	size_t index;
	struct env_struct *env;

	if ((name == NULL) || (*name == '\0')
			|| (strchr(name, '=') != NULL)) {
		SET_ERRNO(EINVAL);
		return -1;
	}

	env = task_self_env_ptr();
	assert(env != NULL);

	index = env_lookup(name, &env->vals[0], env->next);
	if ((index == env->next) || (env->next == 0)) {
		return 0;
	}

	pool_free(&env_pool, env->vals[index]);

	memmove(env->vals + index, env->vals + index + 1,
			(--env->next - index) * sizeof env->vals[0]);
	if (env->next != 0) {
		env->vals[env->next] = NULL;
	}
	else {
		environ = NULL;
	}

	return 0;
}

int clearenv(void) {
	size_t index;
	struct env_struct *env;

	env = task_self_env_ptr();
	assert(env != NULL);

	for (index = 0; index < env->next; ++index) {
		pool_free(&env_pool, env->vals[index]);
	}

	env->next = 0;
	environ = NULL;

	return 0;
}
