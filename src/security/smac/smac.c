/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.02.2013
 */

#include <errno.h>
#include <string.h>
#include <stdio.h> /* snprintf */
#include <fcntl.h> /* O_CREAT, O_APPEND */
#include <fs/flags.h>
#include <fs/file_desc.h>
#include <fs/kfile.h>
#include <kernel/task.h>
#include <debug/symbol.h>
#include <embox/unit.h>

#include <security/smac.h>

EMBOX_UNIT_INIT(smac_init);

#define SMAC_MAX_ENTS OPTION_GET(NUMBER, max_entries)
#define SMAC_AUDIT OPTION_GET(NUMBER, audit)
#define SMAC_AUDIT_FILE OPTION_STRING_GET(audit_file)
#define AUDITLINE_LEN 256

static const char *smac_floor = "_";
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

static struct file_desc *audit_log_desc;

static int audit_log_open(void) {
	audit_log_desc = kopen(SMAC_AUDIT_FILE, O_CREAT | O_WRONLY | O_APPEND, 0755);

	return audit_log_desc ? 0 : -1;
}

int smac_audit_prepare(struct smac_audit *audit, const char *fn_name) {

	audit->fn_name = fn_name;

	return 0;
}

static void audit_log(const char *subject, const char *object,
		int may_access, int ret, struct smac_audit *audit) {
	char line[AUDITLINE_LEN], straccess[4];
	int linelen;

	if (!audit_log_desc) {
		if (audit_log_open()) {
			return;
		}
	}

	straccess[0] = may_access & FS_MAY_READ  ? 'r' : '-';
	straccess[1] = may_access & FS_MAY_WRITE ? 'w' : '-';
	straccess[2] = may_access & FS_MAY_EXEC  ? 'x' : '-';
	straccess[3] = '\0';

	linelen = snprintf(line, AUDITLINE_LEN,
			"subject=%s, object=%s, request=%s, action=%s, function=%s\n",
			subject, object, straccess, ret == 0 ? "ALLOW" : "DENINED",
			audit->fn_name);

	kwrite(line, linelen, audit_log_desc);
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
		&& 0 == (~(FS_MAY_READ | FS_MAY_EXEC) & may_access)) {
		ret = 0;
		goto out;
	}

	/* 3 */
	if (0 == strcmp(s_object, smac_floor)
		&& 0 == (~(FS_MAY_READ | FS_MAY_EXEC) & may_access)) {
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

	smac_audit_prepare(&audit, __func__);

	if (0 != (res = smac_access(task_self_security(), smac_admin,
					FS_MAY_READ, &audit))) {
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

	smac_audit_prepare(&audit, __func__);

	if (0 != (res = smac_access(task_self_security(), smac_admin,
					FS_MAY_WRITE, &audit))) {
		return res;
	}

	smac_env_n = 0;

	return 0;
}

int smac_addenv(const char *subject, const char *object, int flags) {
	struct smac_entry *ent;
	struct smac_audit audit;
	int res;

	smac_audit_prepare(&audit, __func__);

	if (0 != (res = smac_access(task_self_security(), smac_admin,
					FS_MAY_WRITE, &audit))) {
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

	smac_audit_prepare(&audit, __func__);

	if (0 != (res = smac_access(task_self_security(), smac_admin,
					FS_MAY_WRITE, &audit))) {
		return res;
	}

	if ((newlen = strlen(label)) > SMAC_LABELLEN) {
		return -ERANGE;
	}

	strcpy(((struct smac_task *) task_self_security())->label, label);

	return 0;
}

int smac_labelget(char *label, size_t len) {
	struct smac_audit audit;
	int res, thislen;

	smac_audit_prepare(&audit, __func__);

	if (0 != (res = smac_access(task_self_security(), smac_admin,
					FS_MAY_READ, &audit))) {
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

	audit_log_open(); /* not cheking retcode, as the module can be initialized before
			     fs and no file could be opened.
			     But log_open will repeat every log commit till success
			  */
	return 0;
}
