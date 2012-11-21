/**
 * @file
 * @brief
 *
 * @date 25.04.2012
 * @author Andrey Gazukin
 */
#include <embox/cmd.h>
#include <getopt.h>
#include <fs/vfs.h>
#include <types.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: echo \"STRING\" >> FILE\n");
}

static int exec(int argc, char **argv) {
	int opt, file;

	getopt_init();
	while (-1 != (opt = getopt(argc - 1, argv, "h"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		default:
			break;
		}
	}

	if (argc > 3) {
		if(-1 == (file = open((const char *) argv[argc - 1], O_WRONLY))) {
			return file;
		}

		if(0 != strcmp((const char *) argv[argc - 2], ">>")) {
			print_usage();
			return 0;
		}

		lseek(file, 0, SEEK_END);
		write(file, (const void *) argv[1], strlen((const char *) argv[1]));
		write(file, (const void *) "\n", 1);
		close(file);
		return 0;
	}
	else if (argc == 2) {
		printf("%s \n",argv[argc - 1]);
	}
	else {
		print_usage();
	}
	return 0;
}
