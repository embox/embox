/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.02.2013
 */

#include <types.h>
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

#include <embox/cmd.h>

EMBOX_CMD(login_cmd);

#define BUF_LEN 64

#define LOGIN_PROMPT "login: "
#define PASSW_PROMPT "password: "

#define SHADOW_FILE "/shadow"

static void echo_mod(char on) {
	/*printf("\033[12%c", on ? 'l' : 'h');*/
}

static int pass_prompt(const char *prompt, char *buf, int buflen) {
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

static int login_cmd(int argc, char **argv) {
	int res, len;
	struct passwd pwd, *result;
	struct spwd *spwd;
	char namebuf[BUF_LEN], pwdbuf[BUF_LEN], passbuf[BUF_LEN];
	FILE *shdwf;
	const struct shell *shell;

	if (0 != (res = utmp_login(LOGIN_PROCESS, ""))) {
		return res;
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
		if (0 != res || result == NULL) {
			printf("No such user found\n");
			continue;
		}

		if (NULL == (shdwf = fopen(SHADOW_FILE, "r"))) {
			return -ENOENT;
		}

		while (NULL != (spwd = fgetspent(shdwf))) {
			if (0 == strcmp(spwd->sp_namp, result->pw_name)) {
				break;
			}
		}

		fclose(shdwf);

		if (NULL == result || NULL == spwd) {
			continue;
		}

		if (0 > (res = pass_prompt(PASSW_PROMPT, passbuf, BUF_LEN))) {
			continue;
		}

		if (0 == (res = strcmp(passbuf, spwd->sp_pwdp))) {
			break;
		}

	}

	if (res != 0) {
		return res;
	}

	printf("Welcome, %s!", result->pw_gecos);

	setuid(result->pw_uid);
	setgid(result->pw_gid);

	shell = shell_lookup(result->pw_shell);

	if (NULL == shell) {
		shell = shell_any();
	}

	if (NULL == shell) {
		return -ENOENT;
	}

	if (0 != (res = utmp_login(USER_PROCESS, namebuf))) {
		return res;
	}

	shell->exec();

	res = utmp_login(DEAD_PROCESS, "");

	return 0;
}
