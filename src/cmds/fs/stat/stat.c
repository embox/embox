/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

static void print_usage(void) {
	printf("Usage: stat [FILE]...\n");
}
static const char *get_filetype(struct stat *stat) {
	if (S_ISCHR(stat->st_mode)) {
		return "character device";
	}
	if (S_ISDIR(stat->st_mode)) {
		return "directory";
	}
	if (S_ISREG(stat->st_mode)) {
		return "regular file";
	}
	if (S_ISBLK(stat->st_mode)) {
		return "block device";
	}

	return "unknown";
}

static void print_statistic(struct stat *filestat) {
	printf("\tSize:     %ju\n", (uintmax_t)filestat->st_size);
	printf("\tBlocks:   %ju\n", (uintmax_t)filestat->st_blocks);
	printf("\tIO Block: %ju\n", (uintmax_t)filestat->st_blksize);
	printf("\tType:     %s\n", get_filetype(filestat));
	printf("\tDev:      %ju\n", (uintmax_t)filestat->st_dev);
	printf("\tInode:    %ju\n", (uintmax_t)filestat->st_ino);
	printf("\tLinks:    %ju\n", (uintmax_t)filestat->st_nlink);
	printf("\tMode:     %ju\n", (uintmax_t)filestat->st_mode);
	printf("\tUid:      %ju\n", (uintmax_t)filestat->st_uid);
	printf("\tGid:      %ju\n", (uintmax_t)filestat->st_gid);
	printf("\tAccess: 	%ju\n", (uintmax_t)filestat->st_atim.tv_sec);
	printf("\tModify: 	%ju\n", (uintmax_t)filestat->st_mtim.tv_sec);
	printf("\tChange: 	%ju\n", (uintmax_t)filestat->st_ctim.tv_sec);
}

int main(int argc, char **argv) {
	int opt;
	struct stat filestat;

	if (argc < 2) {
		printf("Please enter correct file name\n");
		return 0;
	}

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
	printf("  File: %s\n", argv[argc - 1]);
	print_statistic(&filestat);

	return 0;
}
