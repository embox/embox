/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.02.2013
 */

#include <errno.h>
#include <string.h>
#include <fs/flags.h>
#include <kernel/task.h>
#include <embox/unit.h>

#include <security/smac.h>

EMBOX_UNIT_INIT(smac_init);

static const char *smac_floor = "_";
static const char *smac_star  = "*";
static const char *smac_hat   = "^";
/*static const char *smac_huh   = "?";*/
/*static const char *smac_web   = "@";*/

const char *smac_xattrkey = "SMAC32LABEL";
const char *smac_admin = "smac_admin";

#define SMAC_MAX_ENTS OPTION_GET(NUMBER, max_entries)

static struct smac_entry smac_env[SMAC_MAX_ENTS];
static int smac_env_n;

#define foreach_entry(ent) \
	for (ent = smac_env; ent != smac_env + smac_env_n; ++ent)

/*
 * steps below is from smack deciding order
 */
int smac_access(const char *s_subject, const char *s_object,
		int may_access) {
	struct smac_entry *ent;

	/* 1 */
	if (0 == strcmp(s_subject, smac_star)) {
		return -EACCES;
	}

	/* 2 */
	if (0 == strcmp(s_subject, smac_hat)
		&& 0 == (~(FS_MAY_READ | FS_MAY_EXEC) & may_access)) {
		return 0;
	}

	/* 3 */
	if (0 == strcmp(s_object, smac_floor)
		&& 0 == (~(FS_MAY_READ | FS_MAY_EXEC) & may_access)) {
		return 0;
	}

	/* 4 */
	if (0 == strcmp(s_object, smac_star)) {
		return 0;
	}

	/* 5 */
	if (0 == strcmp(s_subject, s_object)) {
		return 0;
	}

	/* 6 */
	foreach_entry(ent) {
		if (0 == strcmp(s_subject, ent->subject)
				&& 0 == strcmp(s_object, ent->object)) {
			if (~ent->flags & may_access) {
				return -EACCES;
			}

			return 0;
		}
	}

	/* 7 */
	return -EACCES;
}

int smac_getenv(void *buf, size_t buflen, struct smac_env **oenv) {
	struct smac_env *env = (struct smac_env *) buf;
	int res;

	if (0 != (res = smac_access(task_self_security(), smac_admin,
					FS_MAY_READ))) {
		return res;
	}

	if (buflen < sizeof(struct smac_env) + smac_env_n * sizeof(struct smac_entry)) {
		return -ERANGE;
	}

	env->n = smac_env_n;
	memcpy(env->entries, smac_env, smac_env_n * sizeof(struct smac_entry));

	*oenv = env;
	return 0;
}

int smac_flushenv(void) {
	int res;

	if (0 != (res = smac_access(task_self_security(), smac_admin,
					FS_MAY_WRITE))) {
		return res;
	}

	smac_env_n = 0;

	return 0;
}

int smac_addenv(const char *subject, const char *object, int flags) {
	struct smac_entry *ent;
	int res;

	if (0 != (res = smac_access(task_self_security(), smac_admin,
					FS_MAY_WRITE))) {
		return res;
	}

	foreach_entry(ent) {
		if (0 == strcmp(ent->subject, subject)
				&& 0 == strcmp(ent->object, subject)) {
			ent->flags = flags;
			return 0;
		}
	}

	ent = &smac_env[smac_env_n++];
	strcpy(ent->subject, subject);
	strcpy(ent->object, object);
	ent->flags = flags;

	return 0;
}

int smac_setenv(struct smac_env *env) {
	int i, res;

	if (0 != (res = smac_flushenv())) {
		return res;
	}

	for (i = 0; i < env->n; i++) {
		struct smac_entry *ent = &env->entries[i];
		smac_addenv(ent->subject, ent->object, ent->flags);
	}

	return 0;
}

int smac_labelset(const char *label) {
	int res, newlen;

	if (0 != (res = smac_access(task_self_security(), smac_admin,
					FS_MAY_WRITE))) {
		return res;
	}

	if ((newlen = strlen(label)) > SMAC_LABELLEN) {
		return -ERANGE;
	}

	strcpy(((struct smac_task *) task_self_security())->label, label);

	return 0;
}

int smac_labelget(char *label, size_t len) {
	int res, thislen;

	if (0 != (res = smac_access(task_self_security(), smac_admin,
					FS_MAY_READ))) {
		return res;
	}

	if (len < (thislen = strlen(task_self_security()))) {
		return -ERANGE;
	}

	strcpy(label, ((struct smac_task *) task_self_security())->label);

	return 0;
}

static int smac_init(void) {
	strcpy(((struct smac_task *) task_self_security())->label, smac_admin);

	return 0;
}
