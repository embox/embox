/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    10.06.2014
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <shadow.h>

#define SMAC_USERS "/smac_users"
#define TMP_SMAC_USERS "/smac_users.tmp"

static int cmd_smac_adm_user_replace(const char *name, const char *label) {
	struct spwd *spwd;
	FILE *su, *tsu;
	mode_t old_umask;

	old_umask = umask(0077);

	su = fopen(SMAC_USERS, "r");
	if (!su) {
		umask(old_umask);
		return -errno;
	}

	tsu = fopen(TMP_SMAC_USERS, "w");
	if (!tsu) {
		umask(old_umask);
		fclose(su);
		return -errno;
	}

	while ((spwd = fgetspent(su))) {
		if (!strcmp(spwd->sp_namp, name)) {
			spwd->sp_pwdp = (char *) label;
		}

		putspent(spwd, tsu);
	}

	fclose(tsu);
	fclose(su);

	system("rm " SMAC_USERS);
	system("mv -f " TMP_SMAC_USERS " " SMAC_USERS);

	umask(old_umask);

	return 0;
}

static int cmd_smac_adm_user_append(const char *name, const char *label) {
	FILE *su;
	struct spwd spwd;
	int ret;

	su = fopen(SMAC_USERS, "a");
	if (!su) {
		return -errno;
	}

	memset(&spwd, 0, sizeof(spwd));
	spwd.sp_namp = (char *) name;
	spwd.sp_pwdp = (char *) label;

	ret = putspent(&spwd, su);
	fclose(su);
	return ret;
}

int cmd_smac_adm_user_set(const char *name, const char *label) {
	struct spwd *spwd;

	spwd = spwd_find(SMAC_USERS, name);
	if (!spwd) {
		return cmd_smac_adm_user_append(name, label);
	}

	if (!strcmp(spwd->sp_pwdp, label)) {
		return 0;
	}

	return cmd_smac_adm_user_replace(name, label);
}

int cmd_smac_adm_user_get(const char *name, char *buf, size_t buflen) {
	struct spwd *spwd;

	spwd = spwd_find(SMAC_USERS, name);
	if (NULL == spwd) {
		return -ENOENT;
	}

	strncpy(buf, spwd->sp_pwdp, buflen);
	return 0;
}

