/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.03.2013
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <embox/cmd.h>

#include <lib/linenoise_1.h>
#include <cmd/shell.h>
#include <pwd.h>
#include <shadow.h>
#include <utmp.h>

#include <kernel/task/u_area.h>

EMBOX_CMD(su_exec);

#define PASSBUF_LEN 64

extern char *getpass_r(const char *prompt, char *buf, size_t buflen);

static int su_exec(int argc, char *argv[]) {
	struct spwd *spwd;
	struct task_u_area *uarea = task_self_u_area();
	uid_t euid = geteuid();
	uid_t reuid = getuid();
	char passwd[PASSBUF_LEN], *pass;
	int ret;

	uarea->reuid = uarea->euid = 0;

	spwd = getspnam_f("root");

	if (!spwd) {
		ret = -EIO;
		goto out_err;
	}

	if (NULL == (pass = getpass_r("Password: ", passwd, PASSBUF_LEN))) {
		goto out_err;
	}

	if (strcmp(spwd->sp_pwdp, pass)) {
		usleep(2000000);
		fprintf(stderr, "%s: incorrect password\n", argv[0]);
		ret = 0;
		goto out_err;
	}

	shell_run(shell_lookup("tish"));

	ret = 0;

out_err:
	uarea->reuid = reuid;
	uarea->euid = euid;
	return ret;
}

