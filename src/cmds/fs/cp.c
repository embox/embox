 /**
 * @file
 * @brief Mini cp implementation for Embox.
 *
 * @date 22.08.09
 * @author Roman Evstifeev
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <types.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <mem/page.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: cp [-h] [source] [dest]\n");
}

static int exec(int argc, char **argv) {
	int opt, src_file, dst_file;
	char buf[PAGE_SIZE()];
	int bytesread;
	const char *src_path,*dst_path;
	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		default:
			break;
		}
	}

	if (argc < 3) {
		print_usage();
		return -1;
	}

	dst_path = argv[argc - 1];
	src_path = argv[argc - 2];

	if (-1 == (src_file = open(src_path, O_RDONLY)))  {
		printf("can't open file %s\n",src_path);
		return -errno;
	}

	if (-1 == (dst_file = open(dst_path, O_WRONLY))) {
		printf("can't open file %s\n",dst_path);
		return -errno;
	}

	lseek(dst_file, 0, SEEK_SET);

	// buf optimized for whole block write
	bytesread = 0;
	while ((bytesread = read(src_file, buf, PAGE_SIZE())) > 0) {
		if (write (dst_file, buf, bytesread)<=0) {
			printf ("WR ERR!\n");
			break;
		}
	}

	if (fsync(dst_file)) {
		return -1;
	}

	close(src_file);
	close(dst_file);
	return 0;
}
