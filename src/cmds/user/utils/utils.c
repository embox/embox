/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    27.02.2014
 */

#include "user.h"

void user_put_string(char *data, FILE *fd, char delim) {
	fwrite(data, sizeof(char), strlen(data), fd);
	fputc(delim, fd);
}

void user_put_int(int data, FILE *fd, char delim) {
	char buf[20];

	sprintf(buf, "%i", data);
	fwrite(buf, sizeof(char), strlen(buf), fd);
	fputc(delim, fd);
}

int user_is_user_exists(char *name) {
	char pwdbuf[BUF_LEN];
	struct passwd pwd, *result;

	return getpwnam_r(name, &pwd, pwdbuf, BUF_LEN, &result) || result;
}

int user_get_group(char *group) {
	struct group _group, *_group_res;
	char buf[BUF_LEN];
	int res = 0;

	if (isdigit(group[0])) {
		res = getgrgid_r(atoi(group), &_group, buf, BUF_LEN, &_group_res);
	} else {
		res = getgrnam_r(group, &_group, buf, BUF_LEN, &_group_res);
	}
	return !res && _group_res != NULL ? _group_res->gr_gid : -1;
}

void user_write_user_passwd(struct passwd *pwd, FILE *fd) {
	user_put_string(pwd->pw_name, fd, ':');
	user_put_string(pwd->pw_passwd, fd, ':');
	user_put_int(pwd->pw_uid, fd, ':');
	user_put_int(pwd->pw_gid, fd, ':');
	user_put_string(pwd->pw_gecos, fd, ':');
	user_put_string(pwd->pw_dir, fd, ':');
	user_put_string(pwd->pw_shell, fd, '\n');
}

void user_write_user_spwd(struct spwd *spwd, FILE *fd) {
	user_put_string(spwd->sp_namp, fd, ':');
	user_put_string(spwd->sp_pwdp, fd, ':');
	user_put_string("::::::", fd, '\n');
#if 0
	put_int(spwd->sp_lstchg, fd, ':');
	put_int(spwd->sp_min, fd, ':');
	put_int(spwd->sp_max, fd, ':');
	put_int(spwd->sp_warn, fd, ':');
	put_int(spwd->sp_inact, fd, ':');
	put_int(spwd->sp_expire, fd, ':');
	put_int(spwd->sp_expire, fd, '\n');
#endif
}

int user_set_options_passwd(struct passwd *result, char *home, char *shell,
		char *gecos, int group) {
	if (0 != strcmp(home, "")) {
		result->pw_dir = home;
	}

	if (0 != strcmp(shell, "")) {
		result->pw_shell = shell;
	}

	if (0 != strcmp(gecos, "")) {
		result->pw_gecos = gecos;
	}

	if (group >= 0) {
		result->pw_gid = group;
	}

	return 0;
}

int user_set_options_spwd(struct spwd *result, char *name, char *pswd) {
	if (0 != strcmp(name, "")) {
		result->sp_namp = name;
	}

	if (NULL != pswd) {
		result->sp_pwdp = pswd;
	}

	return 0;
}

int user_copy(const char* from, const char* to) {
	FILE *fromf, *tof;
	int a;

	if (NULL == (fromf = fopen(from, "r"))) {
		return -1;
	}

	if (NULL == (tof = fopen(to, "w"))) {
		fclose(fromf);
		return -1;
	}

	while ((a = fgetc(fromf)) != EOF) {
		fputc(a, tof);
	}

	fclose(fromf);
	fclose(tof);

	return 0;
}
