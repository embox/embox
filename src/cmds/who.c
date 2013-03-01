/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.02.2013
 */

#include <unistd.h>
#include <stdio.h>
#include <utmp.h>

#include <embox/cmd.h>

EMBOX_CMD(who_cmd);

const static char *login = "LOGIN";

static int who_cmd(int argc, char **argv) {
	struct utmp *ut;
	const char *user;
	printf("%8s %8s %8s %4s\n", "NAME", "LINE", "TIME", "PID");

	setutent();

	while (NULL != (ut = getutent())) {
		if (ut->ut_type == USER_PROCESS) {
			user = ut->ut_user;
		} else if (ut->ut_type == DEAD_PROCESS) {
			break;
		} else {
			switch(ut->ut_type) {
			case LOGIN_PROCESS:
				user = login;
				break;
			default:
				user = "";
			}
		}

		printf("%8s %8s %8s %4d\n", user, ut->ut_line, "", ut->ut_pid);
	}

	return 0;
}
