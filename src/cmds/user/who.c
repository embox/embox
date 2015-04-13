/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.02.2013
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <utmp.h>

static const char *login = "LOGIN";
static const char *localhost = "(localhost)";

int main(int argc, char **argv) {
	struct utmp *ut;
	const char *user, *host;
	printf("%8s %8s %16s %8s %4s\n", "NAME", "LINE", "HOST", "TIME", "PID");

	setutent();

	while (NULL != (ut = getutent())) {
		host = strlen(ut->ut_host) ? ut->ut_host : localhost;
		user = ut->ut_user;

		switch (ut->ut_type) {
		case USER_PROCESS:
			break;
		case LOGIN_PROCESS:
			user = login;
			break;
		case DEAD_PROCESS:
		default:
			continue;
		}

		printf("%8s %8s %16s %8s %4d\n", user, ut->ut_line,
				host, "", ut->ut_pid);
	}

	return 0;
}
