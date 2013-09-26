/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Andrey Gazukin
 */

#include <embox/cmd.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

EMBOX_CMD(exec);

stat_t filestat;

static void print_usage(void) {
	printf("Usage: stat [FILE]...\n");
}

static void print_statistic(void *stat) {
	stat_t *filestat;

	filestat = (stat_t *) stat;

	printf("Size: %d    Blocks: %d    IO Block: %d  \n",
		(int)filestat->st_size, filestat->st_blocks, (int)filestat->st_blksize);
	printf("Dev: %d    Inode: %d    Links: %d  \n",
		filestat->st_dev, filestat->st_ino, filestat->st_nlink);
	printf("Access: %d    Uid: %d    Gid: %d  \n",
		filestat->st_mode, filestat->st_uid, filestat->st_gid);
	printf("Access: %d  \n", filestat->st_atime);
	printf("Modify: %d  \n", filestat->st_mtime);
	printf("Change: %d  \n", filestat->st_ctime);
}

static int exec(int argc, char **argv) {
	int opt;

	if (argc < 2) {
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
			return -EINVAL;
		}
	}

	if (argc < 2) {
		print_usage();
		return 0;
	}

	if (-1 == stat(argv[argc - 1], &filestat)) {
		return -errno;
	}

	print_statistic(&filestat);

	return 0;
}
