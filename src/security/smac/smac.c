/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.02.2013
 */

#include <assert.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h> /* snprintf */
#include <fcntl.h> /* O_CREAT, O_APPEND */
#include <sys/stat.h>
#include <sys/time.h>

#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <embox/unit.h>
#include <security/seculog/seculog.h>

#include <security/smac/smac.h>
#include <kernel/task/resource/security.h>
#include <config/embox/kernel/task/resource/security.h>
#include <framework/mod/options.h>

EMBOX_UNIT_INIT(smac_init);

static_assert(sizeof(struct smac_task) == OPTION_MODULE_GET(embox__kernel__task__resource__security, NUMBER, security_size));

#define SMAC_MAX_ENTS OPTION_GET(NUMBER, max_entries)
#define SMAC_AUDIT OPTION_GET(NUMBER, audit)
#define AUDITLINE_LEN 256

const char *smac_floor = "_";
static const char *smac_star  = "*";
static const char *smac_hat   = "^";

#if 0
static const char *smac_huh   = "?";
static const char *smac_web   = "@";
#endif

const char *smac_xattrkey = "SMAC32LABEL";
const char *smac_admin = "smac_admin";

static struct smac_entry smac_env[SMAC_MAX_ENTS];
static int smac_env_n;

#define foreach_entry(ent) \
	for (ent = smac_env; ent != smac_env + smac_env_n; ++ent)


int smac_audit_prepare(struct smac_audit *audit, const char *fn_name, const char *file_name) {

	audit->fn_name = fn_name;
	audit->file_name = file_name;

	return 0;
}

/* RFC 868 */
#define SECONDS_1900_1970 2208988800L
static void audit_log(const char *subject, const char *object,
		int may_access, int ret, struct smac_audit *audit) {
	static char no_audit;
	char line[AUDITLINE_LEN];
	uid_t uid;
	char pwd_sbuf[64];
	struct passwd pwd_buf;
	struct passwd *pwd;
	time_t time;
	struct timeval tv;

	if (no_audit) {
		return;
	}

	uid = getuid();
	no_audit = 1;
	getpwuid_r(uid, &pwd_buf, pwd_sbuf, sizeof(pwd_sbuf), &pwd);
	no_audit = 0;

	gettimeofday(&tv, NULL);
	time = (time_t)((uint32_t)tv.tv_sec - SECONDS_1900_1970);
	snprintf(line, AUDITLINE_LEN,
			"[%s] cmd=%s subject=%s(label=%s), object=%s, file=%s, request=%c%c%c, action=%s, function=%s\n",
			ctime(&time),
			task_self() != task_kernel_task() ? task_get_name(task_self()) : "init",
			pwd ? pwd->pw_name : "?",
			subject,
			object,
			audit->file_name ? audit->file_name : "",
			may_access & S_IROTH  ? 'r' : '-',
			may_access & S_IWOTH ? 'w' : '-',
			may_access & S_IXOTH  ? 'x' : '-',
			ret == 0 ? "ALLOW" : "DENINED",
			audit->fn_name);

	seculog_record(SECULOG_LABEL_MANDATORY, line);
}

/*
 * steps below is from smack deciding order
 */
int smac_access(const char *s_subject, const char *s_object,
		int may_access, struct smac_audit *audit) {
	struct smac_entry *ent;
	int ret = 0;

	/* 1 */
	if (0 == strcmp(s_subject, smac_star)) {
		ret = -EACCES;
		goto out;
	}

	/* 2 */
	if (0 == strcmp(s_subject, smac_hat)
		&& 0 == (~(S_IROTH | S_IXOTH) & may_access)) {
		ret = 0;
		goto out;
	}

	/* 3 */
	if (0 == strcmp(s_object, smac_floor)
		&& 0 == (~(S_IROTH | S_IXOTH) & may_access)) {
		ret = 0;
		goto out;
	}

	/* 4 */
	if (0 == strcmp(s_object, smac_star)) {
		ret = 0;
		goto out;
	}

	/* 5 */
	if (0 == strcmp(s_subject, s_object)) {
		ret = 0;
		goto out;
	}

	/* 6 */
	foreach_entry(ent) {
		if (0 == strcmp(s_subject, ent->subject)
				&& 0 == strcmp(s_object, ent->object)) {
			if (~ent->flags & may_access) {
				ret = -EACCES;
				goto out;
			}
			ret = 0;
			goto out;
		}
	}

	/* 7 */
	ret = -EACCES;
out:

#if SMAC_AUDIT
	if ((ret == -EACCES && (SMAC_AUDIT & 1))
			|| (ret == 0 && (SMAC_AUDIT & 2))) {
		audit_log(s_subject, s_object, may_access, ret, audit);
	}
#endif /* SMAC_AUDIT */

	return ret;

}

int smac_getenv(void *buf, size_t buflen, struct smac_env **oenv) {
	struct smac_env *env = (struct smac_env *) buf;
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__, NULL);

	if (0 != (res = smac_access(task_self_resource_security(), smac_admin,
					S_IROTH, &audit))) {
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
	struct smac_audit audit;

	smac_audit_prepare(&audit, __func__, NULL);

	if (0 != (res = smac_access(task_self_resource_security(), smac_admin,
					S_IWOTH, &audit))) {
		return res;
	}

	smac_env_n = 0;

	return 0;
}

int smac_addenv(const char *subject, const char *object, int flags) {
	struct smac_entry *ent;
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__, NULL);

	if (0 != (res = smac_access(task_self_resource_security(), smac_admin,
					S_IWOTH, &audit))) {
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
	struct smac_audit audit;
	int res, newlen;

	smac_audit_prepare(&audit, __func__, NULL);

	if (0 != (res = smac_access(task_self_resource_security(), smac_admin,
					S_IWOTH, &audit))) {
		return res;
	}

	if ((newlen = strlen(label)) > SMAC_LABELLEN) {
		return -ERANGE;
	}

	strcpy(((struct smac_task *) task_self_resource_security())->label, label);

	return 0;
}

int smac_labelget(char *label, size_t len) {
	struct smac_audit audit;
	int res, thislen;

	smac_audit_prepare(&audit, __func__, NULL);

	if (0 != (res = smac_access(task_self_resource_security(), smac_admin,
					S_IROTH, &audit))) {
		return res;
	}

	if (len < (thislen = strlen(task_self_resource_security()))) {
		return -ERANGE;
	}

	strcpy(label, ((struct smac_task *) task_self_resource_security())->label);

	return 0;
}

static int smac_init(void) {
	strcpy(((struct smac_task *) task_self_resource_security())->label, smac_admin);

	/* should allow ourself do anything with not labeled file as there is no
 	 * security at all.
	 * Otherwise boot could hang at mount, etc.
	 */
 	 smac_addenv(smac_admin, smac_def_file_label,
			S_IROTH | S_IWOTH | S_IXOTH);

	return 0;
}
