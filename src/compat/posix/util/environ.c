/**
 * @file
 * @brief
 *
 * @date 27.02.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <kernel/task/resource/env.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <lib/libds/array.h>
#include <unistd.h>
#include <stdio.h>

char *** task_self_environ_ptr(void) {
	return &task_self_resource_env()->envs;
}

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
	struct task_env *env;

	if ((name == NULL) || (*name == '\0')
			|| (strchr(name, '=') != NULL)) {
		return NULL;
	}

	env = task_self_resource_env();
	assert(env != NULL);

	index = env_lookup(name, &env->vals[0], env->next);
	if (index == env->next) {
		return NULL;
	}

	assert(env->vals[index] != NULL);
	strcpy(&env->buff[0], env->vals[index] + strlen(name) + 1);

	return &env->buff[0];
}

int putenv(char *string) {
	size_t str_len, name_len, index;
	struct task_env *env;

	assert(string != NULL);
	//assert(*string != '=');
	if(*string == '=') {
		printf("Exported variable cannot be null\n");
		return -1;
	}

	assert(strchr(string, '=') != NULL);

	env = task_self_resource_env();
	assert(env != NULL);

	name_len = strchr(string, '=') - string;
	memcpy(&env->buff[0], string, name_len);
	env->buff[name_len] = '\0';

	index = env_lookup(&env->buff[0], &env->vals[0], env->next);
	str_len = strlen(string);

	if ((str_len >= ARRAY_SIZE(env->storage[0]))
			|| (index == ARRAY_SIZE(env->vals) - 1)) {
		SET_ERRNO(ENOMEM);
		return -1;
	}

	if (index == env->next) {
		env->vals[env->next] = &env->storage[env->next][0];
		if (env->next == 0) {
			env->envs = &env->vals[0];
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
	struct task_env *env;

	if ((name == NULL) || (*name == '\0')
			|| (strchr(name, '=') != NULL)
			|| (value == NULL)) {
		SET_ERRNO(EINVAL);
		return -1;
	}

	env = task_self_resource_env();
	assert(env != NULL);

	index = env_lookup(name, &env->vals[0], env->next);
	if ((index < env->next) && !overwrite) {
		return 0;
	}

	name_len = strlen(name);
	val_len = strlen(value);

	if ((name_len + val_len >= ARRAY_SIZE(env->storage[0]))
			|| (index == ARRAY_SIZE(env->vals) - 1)) {
		SET_ERRNO(ENOMEM);
		return -1;
	}

	if (index == env->next) {
		env->vals[env->next] = &env->storage[env->next][0];
		if (env->next == 0) {
			env->envs = &env->vals[0];
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
	struct task_env *env;

	if ((name == NULL) || (*name == '\0')
			|| (strchr(name, '=') != NULL)) {
		SET_ERRNO(EINVAL);
		return -1;
	}

	env = task_self_resource_env();
	assert(env != NULL);

	index = env_lookup(name, &env->vals[0], env->next);
	if ((index == env->next) || (env->next == 0)) {
		return 0;
	}

	--env->next;
	memmove(&env->vals[index], &env->vals[index + 1],
			(env->next - index) * sizeof env->vals[0]);
	memmove(&env->storage[index][0], &env->storage[index + 1][0],
			(env->next - index) * ARRAY_SIZE(env->storage[0]));

	if (env->next != 0) {
		env->vals[env->next] = NULL;
	}
	else {
		env->envs = NULL;
	}

	return 0;
}

int clearenv(void) {
	struct task_env *env;

	env = task_self_resource_env();
	assert(env != NULL);

	env->envs = NULL;
	env->next = 0;

	return 0;
}
