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
#include <pwd.h>
#include <shadow.h>
#include <utmp.h>
#include <termios.h>
#include <unistd.h>

#include <readline/readline.h>

#include <cmd/shell.h>

#include <kernel/task.h>

#include <security/seculog/seculog.h>

#include <crypt.h>

#define BUF_LEN 64

#define LOGIN_PROMPT "login: "
#define PASSW_PROMPT "password: "

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

#if OPTION_GET(BOOLEAN, security_support)
#include <security/smac/smac.h>
#define SMAC_USERS "/smac_users"
static void login_set_security(struct taskdata *tdata) {
	const struct spwd *spwd;
	char *new_smac_label = "_";

	if (NULL != (spwd = spwd_find(SMAC_USERS, tdata->pwd->pw_name))) {
		new_smac_label = spwd->sp_pwdp;
	}

	if (smac_labelset(new_smac_label)) {
		printf("can't setup smac label\n");
	}
}
#else
static void login_set_security(struct taskdata *tdata) {

}
#endif


static void *taskshell(void *data) {
	const struct shell *shell;
	struct taskdata *tdata = data;

	int ret;

	login_set_security(tdata);

	ret = setuid(tdata->pwd->pw_uid);
	if (ret < 0) {
		printf("Can't setup UID: %s\n", strerror(errno));
		return NULL;
	}

	ret = setgid(tdata->pwd->pw_gid);
	if (ret < 0) {
		printf("Can't setup GID: %s\n", strerror(errno));
		return NULL;
	}

	printf("Welcome, %s!\n", tdata->pwd->pw_gecos);

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

static unsigned char stdin_vintr;
static unsigned char vdisable = -1;

static int fileno_vintr_disable(int fd) {
    struct termios t;

	if (-1 == tcgetattr(fd, &t)) {
		return -errno;
	}

	stdin_vintr = t.c_cc[VINTR];
	t.c_cc[VINTR] = vdisable;

	if (-1 == tcsetattr(fd, TCSANOW, &t)) {
		return -errno;
	}

	return 0;
}

static int fileno_vintr_enable(int fd) {
    struct termios t;

	if (-1 == tcgetattr(fd, &t)) {
		return -errno;
	}

	t.c_cc[VINTR] = stdin_vintr;

	if (-1 == tcsetattr(fd, TCSANOW, &t)) {
		return -errno;
	}

	return 0;
}

static inline void seculog_make_rec(const char *username, char allowed) {
	char seculog_msg[64];
	snprintf(seculog_msg, 64, "login=%s,action=%s\n", username, allowed ? "ALLOW" : "DENY");

	seculog_record(SECULOG_LABEL_LOGIN_ACT, seculog_msg);
}


static int login_user(const char *name, const char *cmd, char with_pass) {
	char pwdbuf[BUF_LEN], passbuf[BUF_LEN];
	struct passwd pwd, *result;
	struct spwd *spwd = NULL;
	int tid;
	int res;

	res = fileno_vintr_disable(STDIN_FILENO);
	if (res != 0) {
		printf("fileno_vintr_disable failed\n");
		goto errret;
	}

	if (with_pass && NULL == getpass_r(PASSW_PROMPT, passbuf, BUF_LEN)) {
		res = -1;
		goto errret;
	}

	if ((res = getpwnam_r(name, &pwd, pwdbuf, BUF_LEN, &result)) ||
			result == NULL) {
		goto errret;
	}

	if (with_pass) {
		if (!(spwd = getspnam_f(result->pw_name))) {
			res = -1;
			goto errret;
		}

		if (strcmp(crypt(passbuf, NULL), spwd->sp_pwdp)) {
			res = -1;
			goto errret;
		}
	}

	res = fileno_vintr_enable(STDIN_FILENO);
	if (res != 0) {
		printf("fileno_vintr_enable failed\n");
		goto errret;
	}

	seculog_make_rec(name, 1);

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
	seculog_make_rec(name, 0);

	sleep(3);

	if (0 != fileno_vintr_enable(STDIN_FILENO)) {
		printf("fileno_vintr_enable failed\n");
	}

	return res;
}

int main(int argc, char **argv) {
	char *name = NULL, *cmd = NULL;
	char with_pass = 1;
	int opt, res;

	if (argc != 1) {


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

		while (!(name = readline(LOGIN_PROMPT))) {
			printf("\n\n");
		}

		login_user(name, NULL, 1);

		free(name);

	} while(1);

	return 0;
}
