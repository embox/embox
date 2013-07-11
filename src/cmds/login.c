/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.02.2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <lib/linenoise_1.h>
#include <cmd/shell.h>
#include <pwd.h>
#include <shadow.h>
#include <utmp.h>

#include <embox/cmd.h>

extern char *getpass_r(const char *prompt, char *buf, size_t buflen);

EMBOX_CMD(login_cmd);

#define BUF_LEN 64

#define LOGIN_PROMPT "login: "
#define PASSW_PROMPT "password: "

#define SMAC_USERS "/smac_users"

static struct spwd *spwd_find(const char *spwd_path, const char *name) {
	struct spwd *spwd;
	FILE *shdwf;

	if (NULL == (shdwf = fopen(spwd_path, "r"))) {
		return NULL;
	}

	while (NULL != (spwd = fgetspent(shdwf))) {
		if (0 == strcmp(spwd->sp_namp, name)) {
			break;
		}
	}

	fclose(shdwf);

	return spwd;
}

static int utmp_login(short ut_type, const char *ut_user) {
	struct utmp utmp;
	struct timeval tv;

	utmp.ut_type = ut_type;
	utmp.ut_type = ut_type;
	utmp.ut_pid = getpid();
	snprintf(utmp.ut_id, UT_IDSIZE, "/%d", utmp.ut_pid);
	snprintf(utmp.ut_line, UT_LINESIZE, "pty/%d", utmp.ut_pid);
	strcpy(utmp.ut_user, ut_user);
	memset(&utmp.ut_host, 0, UT_HOSTSIZE);
	memset(&utmp.ut_exit, 0, sizeof(struct exit_status));

	gettimeofday(&tv, NULL);

	utmp.ut_tv.tv_sec = tv.tv_sec;
	utmp.ut_tv.tv_usec = tv.tv_usec;

	if (NULL == pututline(&utmp)) {
		return errno;
	}

	return 0;
}

static void *taskshell(void *data) {
	const struct shell *shell;
	const struct spwd *spwd;
	const struct passwd *result = data;

	printf("Welcome, %s!\n", result->pw_gecos);

	setuid(result->pw_uid);
	setgid(result->pw_gid);

	{
		char smac_cmd[BUF_LEN], *smac_label = "_";

		if (NULL != (spwd = spwd_find(SMAC_USERS, result->pw_name))) {
			smac_label = spwd->sp_pwdp;
		}

		snprintf(smac_cmd, BUF_LEN, "smac_adm -S %s", smac_label);

		if (0 != shell_exec(shell_any(), smac_cmd)) {
			printf("login: cannot initialize SMAC label\n");
		}
	}

	shell = shell_lookup(result->pw_shell);

	if (NULL == shell) {
		shell = shell_lookup("tish");
	}

	if (NULL == shell) {
		shell = shell_any();
	}
	if (NULL == shell) {
		return NULL;
	}

	utmp_login(USER_PROCESS, result->pw_name);
	shell_run(shell);

	utmp_login(DEAD_PROCESS, "");

	return NULL;

}

static int login_cmd(int argc, char **argv) {
	int res;
	struct passwd pwd, *result = NULL;
	struct spwd *spwd = NULL;
	char *name, pwdbuf[BUF_LEN], passbuf[BUF_LEN];
	int tid;

	do {
		if (0 != (res = utmp_login(LOGIN_PROCESS, ""))) {
			/* */
		}

		while (1) {
			//sleep(3);
			printf("\n\n");
			if (NULL == (name = linenoise(LOGIN_PROMPT))) {
				continue;
			}

			res = getpwnam_r(name, &pwd, pwdbuf, BUF_LEN, &result);

			free(name);

			if (result) {
				spwd = getspnam_f(result->pw_name);
			}

			if (result == NULL || spwd == NULL) {
				printf("login: no such user found\n");
				sleep(3);
				continue;
			}


			if (NULL == getpass_r(PASSW_PROMPT, passbuf, BUF_LEN)) {
				sleep(3);
				continue;
			}

			if (0 == (res = strcmp(passbuf, spwd->sp_pwdp))) {
				break;
			}

			sleep(3);
		}

		if (0 > (tid = new_task("sh", taskshell, result))) {
			return tid;
		}

		task_waitpid(tid);

	} while(1);

	return 0;
}
