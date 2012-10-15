/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Andrey Gazukin
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

EMBOX_CMD(exec);

stat_t filestat;

static void print_usage(void) {
	printf("Usage: stat [FILE]...\n");
}

static int exec(int argc, char **argv) {
	int opt;
	int fd;

	if(argc < 2) {
		printf("Please enter correct file name\n");
		return 0;
	}
	getopt_init();

	while (-1 != (opt = getopt(argc - 1, argv, "nh"))) {
		switch (opt) {
		case '?':
			printf("Invalid option `-%c'\n", optopt);
			/* FALLTHROUGH */
			argc = 0;
			break;
		case 'h':
			print_usage();
			return 0;
		default:
			return -1;
		}
	}

	if (argc < 2) {
		print_usage();
		return 0;
	}
	if (0 > (fd = open(argv[argc - 1], O_RDONLY))) {
		printf("Can't open file %s\n", argv[argc - 1]);
		return -1;
	}

	stat(fd, &filestat);

	close(fd);
	return 0;
}

