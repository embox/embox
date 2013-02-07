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

#include <lib/linenoise.h>
#include <cmd/shell.h>
#include <pwd.h>
#include <shadow.h>

#include <embox/cmd.h>

EMBOX_CMD(login_cmd);

#define BUF_LEN 64

#define LOGIN_PROMPT "\n\nlogin: "
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


static int login_cmd(int argc, char **argv) {
	int res, len;
	struct passwd pwd, *result;
	struct spwd *spwd;
	char namebuf[BUF_LEN], pwdbuf[BUF_LEN], passbuf[BUF_LEN];
	FILE *shdwf;
	const struct shell *shell;


	while (1) {
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

	shell->exec();

	return 0;
}
