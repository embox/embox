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
#include <security/smac.h>

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

struct taskdata {
	const struct passwd *pwd;
	const char *cmd;
};

static void *taskshell(void *data) {
	const struct shell *shell;
	const struct spwd *spwd;
	struct taskdata *tdata = data;

	printf("Welcome, %s!\n", tdata->pwd->pw_gecos);

	setuid(tdata->pwd->pw_uid);
	setgid(tdata->pwd->pw_gid);

	{
		char *new_smac_label = "_";

		if (NULL != (spwd = spwd_find(SMAC_USERS, tdata->pwd->pw_name))) {
			new_smac_label = spwd->sp_pwdp;
		}

		if (smac_labelset(new_smac_label)) {
			printf("can't setup smac label\n");
		}
	}

	shell = shell_lookup(tdata->pwd->pw_shell);

	if (NULL == shell) {
		shell = shell_lookup("tish");
	}

	if (NULL == shell) {
		shell = shell_any();
	}
	if (NULL == shell) {
		return NULL;
	}

	utmp_login(USER_PROCESS, tdata->pwd->pw_name);

	if (!tdata->cmd) {
		shell_run(shell);
	} else {
		shell_exec(shell, tdata->cmd);
	}

	utmp_login(DEAD_PROCESS, "");

	return NULL;

}

static int login_user(const char *name, const char *cmd, char with_pass) {
	char pwdbuf[BUF_LEN], passbuf[BUF_LEN];
	struct passwd pwd, *result;
	struct spwd *spwd = NULL;
	int tid;
	int res;

	if (with_pass && NULL == getpass_r(PASSW_PROMPT, passbuf, BUF_LEN)) {
		goto errret;
	}

	if ((res = getpwnam_r(name, &pwd, pwdbuf, BUF_LEN, &result)) ||
			result == NULL) {
		goto errret;
	}

	if (!(spwd = getspnam_f(result->pw_name))) {
		goto errret;
	}

	if (with_pass && strcmp(passbuf, spwd->sp_pwdp)) {
		goto errret;
	}

	{
		struct taskdata tdata = {
			.pwd = result,
			.cmd = cmd,
		};

		if (0 > (tid = new_task("sh", taskshell, &tdata))) {
			return tid;
		}

		task_waitpid(tid);
	}

	return 0;

errret:
	sleep(3);

	return res;
}

static int login_cmd(int argc, char **argv) {
	char *name = NULL, *cmd = NULL;
	char with_pass = 1;
	int opt, res;

	if (argc != 1) {

		getopt_init();

		while (-1 != (opt = getopt(argc, argv, "pc:"))) {
			switch(opt) {
			case 'c':
				cmd = optarg;
				break;
			case 'p':
				with_pass = 0;
				break;
			default:
				break;
			}
		}

		if (optind < argc) {
			name = argv[optind];
		}


		return login_user(name, cmd, with_pass);
	}

	do {
		if (0 != (res = utmp_login(LOGIN_PROCESS, ""))) {
			/* */
		}

		while (!(name = linenoise(LOGIN_PROMPT))) {
			printf("\n\n");
		}

		login_user(name, NULL, 1);

		free(name);

	} while(1);

	return 0;
}
