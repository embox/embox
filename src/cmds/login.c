/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.02.2013
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <lib/linenoise.h>
#include <cmd/shell.h>
#include <pwd.h>
#include <shadow.h>
#include <utmp.h>

#include <kernel/thread/event.h>

#include <embox/cmd.h>

EMBOX_CMD(login_cmd);

#define BUF_LEN 64

#define LOGIN_PROMPT "login: "
#define PASSW_PROMPT "password: "

#define SHADOW_FILE "/shadow"
#define SMAC_USERS "/smac_users"

static void echo_mod(char on) {
	/*printf("\033[12%c", on ? 'l' : 'h');*/
}

static int passw_prompt(const char *prompt, char *buf, int buflen) {
	int ch;
	printf("%s", prompt);

	echo_mod(0);

	ch = fgetc(stdin);

	while ('\n' != ch && '\r' != ch) {

		/* Avoid strange symbols in stdin.
		 * Actually, telnet sends \r as \r\0,
		 * so trying bypass it.
		 */
		if (ch == '\0') {
			ch = fgetc(stdin);
			continue;
		}

		if (buflen-- <= 0) {
			return -ERANGE;
		}

		*buf++ = ch;
		ch = fgetc(stdin);
	}

	if (buflen-- <= 0) {
		return -ERANGE;
	}

	*buf++ = '\0';

	echo_mod(1);

	printf("\n");

	return 0;
}

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

struct subshell_data {
	const struct passwd *result;
	struct event *event;
};

static void *taskshell(void *data) {
	const struct shell *shell;
	const struct spwd *spwd;
	struct subshell_data *subdata = (struct subshell_data *) data;
	const struct passwd *result = subdata->result;

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

	event_notify(subdata->event);

	return NULL;

}

static int login_cmd(int argc, char **argv) {
	int res, len;
	struct passwd pwd, *result = NULL;
	struct spwd *spwd = NULL;
	char namebuf[BUF_LEN], pwdbuf[BUF_LEN], passbuf[BUF_LEN];
	struct event subshell_event;
	struct subshell_data subdata;
	int tid;

	do {
		if (0 != (res = utmp_login(LOGIN_PROCESS, ""))) {
			/* */
		}

		while (1) {
			printf("\n\n");
			if (0 > (res = linenoise(LOGIN_PROMPT, namebuf, BUF_LEN, NULL, NULL))) {
				continue;
			}

			len = strlen(namebuf);
			if (namebuf[len - 1] == '\n') {
				namebuf[--len] = '\0';
			}

			res = getpwnam_r(namebuf, &pwd, pwdbuf, BUF_LEN, &result);

			if (result) {
				spwd = spwd_find(SHADOW_FILE, result->pw_name);
			}

			if (result == NULL || spwd == NULL) {
				printf("login: no such user found\n");
				continue;
			}


			if (0 > (res = passw_prompt(PASSW_PROMPT, passbuf, BUF_LEN))) {
				continue;
			}

			if (0 == (res = strcmp(passbuf, spwd->sp_pwdp))) {
				break;
			}

		}

		event_init(&subshell_event, "subshell event");

		subdata.event = &subshell_event;
		subdata.result = result;

		if (0 > (tid = new_task("sh", taskshell, &subdata))) {
			return tid;
		}

		while(0 > event_wait_ms(&subshell_event, EVENT_TIMEOUT_INFINITE)) {

		}
	} while(1);

	return 0;
}
