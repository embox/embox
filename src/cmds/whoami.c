/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.02.2013
 */

#include <types.h>
#include <unistd.h>
#include <stdio.h>
#include <pwd.h>

#include <embox/cmd.h>

EMBOX_CMD(whoami_cmd);

#define BUF_LEN 1024

static int whoami_cmd(int argc, char **argv) {
	int res;
	uid_t uid = geteuid();
	struct passwd pwd, *result;
	char buf[BUF_LEN];


	if (0 != (res = getpwuid_r(uid, &pwd, buf, BUF_LEN, &result))) {
		return res;
	}

	printf("%s\n", pwd.pw_name);

	return 0;
}
