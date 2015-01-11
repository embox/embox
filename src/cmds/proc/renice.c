/**
 * @file
 * @brief
 *
 * @date 11.03.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <sys/resource.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>

int main(int argc, char **argv) {
	int with_incr, incr, ind, prior, with_n, which, ret;
	id_t who;
	struct passwd *p;

	if (argc == 1) {
		return -EINVAL;
	}

	ret = 0;
	which = PRIO_PROCESS;
	with_incr = incr = with_n = 0;
	for (ind = 1; ind < argc; ++ind) {
		if (strcmp(argv[ind], "-h") == 0) {
			printf("Usage: %s [-n] priority [[-p] pid ...] [[-g] pgrp ...] [[-u] user ...]\n", argv[0]);
			return 0;
		}
		else if (strcmp(argv[ind], "-n") == 0) {
			if (with_n) {
				return -EINVAL;
			}
			with_n = 1;
		}
		else if (strcmp(argv[ind], "-p") == 0) {
			which = PRIO_PROCESS;
		}
		else if (strcmp(argv[ind], "-g") == 0) {
			which = PRIO_PGRP;
		}
		else if (strcmp(argv[ind], "-u") == 0) {
			which = PRIO_USER;
		}
		else {
			if (!with_incr) {
				if (sscanf(argv[ind], "%d", &incr) != 1) {
					return -EINVAL;
				}
				with_incr = 1;
				continue;
			}

			if (which == PRIO_USER) {
				p = getpwnam(argv[ind]);
				if (p == NULL) {
					printf("%s: unknown user '%s`\n", argv[0], argv[ind]);
					ret = -errno;
					continue;
				}
				who = p->pw_uid;
			}
			else {
				if (sscanf(argv[ind], "%d", &who) != 1) {
					printf("%s: bad value '%s`\n", argv[0], argv[ind]);
					ret = -EINVAL;
					continue;
				}
			}

			if (with_n) {
				errno = 0;
				prior = getpriority(which, who);
				if ((prior == -1) && (errno != 0)) {
					return -errno;
				}
				prior += incr;
			}
			else {
				prior = incr;
			}

			if (-1 == setpriority(which, who, prior)) {
				printf("%s: failed to set priority for %d (%s): %s\n",
						argv[0], who,
						which == PRIO_PROCESS ? "process ID"
							: which == PRIO_PGRP ? "process group ID"
							: "user ID",
						strerror(errno));
				ret = -errno;
			}
		}
	}

	return ret;
}
