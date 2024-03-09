/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Andrey Gazukin
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>

static void print_usage(void) {
	printf("Usage: stat [FILE]...\n");
}
static const char *get_filetype(struct stat *stat) {
	if (S_ISCHR(stat->st_mode)) {
		return "character special file";
	}
	if (S_ISDIR(stat->st_mode)) {
		return "directory";
	}
	if (S_ISREG(stat->st_mode)) {
		return "regular file";
	}
	if (S_ISBLK(stat->st_mode)) {
		return "block special file";
	}

	return "unknown file type";
}

static void print_statistic(struct stat *filestat) {
	printf("  Size: %8d    Blocks: %8d    IO Block: %8d  %s\n",
		(int)filestat->st_size, (int)filestat->st_blocks,
		(int)filestat->st_blksize, get_filetype(filestat));
	printf("   Dev: %8d     Inode: %8ld       Links: %8d\n",
		filestat->st_dev, filestat->st_ino, filestat->st_nlink);
	printf("Access: %8d       Uid: %8d         Gid: %8d\n",
		filestat->st_mode, filestat->st_uid, filestat->st_gid);
	printf("Access: %8ld  \n", (long)filestat->st_atime);
	printf("Modify: %8ld  \n", (long)filestat->st_mtime);
	printf("Change: %8ld  \n", (long)filestat->st_ctime);
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
