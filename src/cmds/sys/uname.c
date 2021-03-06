/**
 * @file
 * @brief
 *
 * @date 01.10.12
 * @author Sergey Ivantsov
 * @author Ilia Vaprol
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <lib/sysctl.h>
#include <errno.h>
#include <string.h>

struct uname_args {
	char with_a;
	char with_s;
	char with_n;
	char with_r;
	char with_v;
	char with_m;
	char with_p;
	char with_i;
	char with_o;
};

static int uname_args_not_empty(struct uname_args *args) {
	return args->with_a || args->with_s || args->with_n || args->with_r
			|| args->with_v || args->with_m || args->with_p || args->with_i
			|| args->with_o;
}

int main(int argc, char *argv[]) {
	int opt;
	struct utsname info;
	struct sysct sysct_info;
	struct uname_args args;

	memset(&args, 0, sizeof args);

	while (-1 != (opt = getopt(argc, argv, "hasnrvmpio"))) {
		switch(opt) {
		default:
			printf("Try '%s -h` for more information.", argv[0]);
			return -EINVAL;
		case 'h':
			printf("Usage: %s [-hasnrvmpio]\n", argv[0]);
			return 0;
		case 'a': args.with_a = 1; break;
		case 's': args.with_s = 1; break;
		case 'n': args.with_n = 1; break;
		case 'r': args.with_r = 1; break;
		case 'v': args.with_v = 1; break;
		case 'm': args.with_m = 1; break;
		case 'p': args.with_p = 1; break;
		case 'i': args.with_i = 1; break;
		case 'o': args.with_o = 1; break;
		}
	}

	if (!uname_args_not_empty(&args)) {
		args.with_s = 1;
	}

	if (-1 == uname(&info)) {
		printf("uname failed with code error %d", errno);
		return errno;
	}

	if (-1 == sysctl(&sysct_info)) {
		printf("uname failed with code error %d", errno);
		return errno;
	}

	if (args.with_a || args.with_s) printf("%s ", info.sysname);
	if (args.with_a || args.with_n) printf("%s ", info.nodename);
	if (args.with_a || args.with_r) printf("%s ", info.release);
	if (args.with_a || args.with_v) printf("%s ", info.version);
	if (args.with_a || args.with_m) printf("%s ", info.machine);

	if (args.with_a || args.with_p) printf("%s ", sysct_info.processor);
	if (args.with_a || args.with_i) printf("%s ", sysct_info.platform);
	if (args.with_a || args.with_o) printf("%s ", sysct_info.system);


	printf("\n");

	return 0;
}
