/**
 * @file
 * @brief Read block devices block contents.
 *
 * @date 30.01.13
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <mem/phymem.h>
#include <embox/cmd.h>
#include <embox/block_dev.h>

#define BSIZE   512
#define PATH "if="
#define START_B "skip="
#define NUM_B "count="

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: dd if=/path skip=start_block count=size_in_byte \n");
}

static void print_data(char *buff, size_t size, blkno_t start) {
	size_t cnt;
	unsigned int addr;
	int i_substr, i_str;
	char *point, *substr_p;

	substr_p = point = buff;
	addr = start * BSIZE;

	for (cnt = 0; cnt < size; cnt += 16) {
		printf("%08X ", addr);
		for (i_str = 0; i_str < 4; i_str++) {
			for (i_substr = 0; i_substr < 4; i_substr++) {
				printf("%02hhX ", (unsigned char) *point++);
			}
			if(i_str < 3) {
				printf("| ");
			}
		}
		for (i_str = 0; i_str < 16; i_str++) {
			if (((unsigned char) *substr_p) >= 32) { /*is not service simbol */
				printf("%c", *substr_p);
			}
			else {
				printf(".");
			}
			substr_p++;
		}
		printf("\n");
		addr += 16;
	}
}

static int get_arg(int argc, char **argv, const char *mask, char *data) {
	int i;
	int len;

	len = strlen(mask);

	for (i = 0; i < argc; i++) {
		if(0 == strncmp(mask, argv[i], len)) {
			sprintf(data, "%s", &argv[i][len]);
			return 0;
		}
	}
	return -1;
}

#if 0
static int read_file(char *path, char *buffer, size_t size, blkno_t blkno) {
	ssize_t bytesread;
	int file;

	if (0 > (file = open(path, O_RDONLY)))  {
		printf("can't open file %s\n", path);
		return -1;
	}
	if (0 > lseek(file, blkno * BSIZE, SEEK_SET)) {
		return -1;
	}

	bytesread = read(file, buffer, size);
	close(file);

	return bytesread;
}
#endif

static int exec(int argc, char **argv) {
	int rc;
	char path[MAX_LENGTH_PATH_NAME];
	char num[MAX_LENGTH_FILE_NAME];
	size_t bytes;
	blkno_t blkno;
	char *buffer;
	ssize_t bytesread;
	int opt;
	int fd;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		case '?':
			break;
		default:
			printf("dd: invalid option -- '%c'\n", optopt);
			return -1;
		}
	}

	blkno = 0;
	bytes = BSIZE;
	rc = 0;

	if (0 == get_arg(argc, argv, NUM_B, num)) {
		sscanf(num, "%u", &bytes);
	}

	if (0 == get_arg(argc, argv, START_B, num)) {
		sscanf(num, "%u", &blkno);
	}

	if (0 > get_arg(argc, argv, PATH, path)) {
		return -1;
	}

	if (-1 == (fd = open(path, O_RDONLY))) {
		return -errno;
	}

	if ((off_t) -1 == lseek(fd, blkno * BSIZE, SEEK_SET)) {
		return -errno;
	}

	if (NULL == (buffer =
			page_alloc(__phymem_allocator, bytes / PAGE_SIZE() + 1))) {
		return -ENOMEM;
	}

	if (-1 == (bytesread = read(fd, buffer, bytes))) {
		rc = -errno;
		goto out;
	}

	print_data(buffer, bytesread, blkno);

out:
	/* free buffer */
	page_free(__phymem_allocator, buffer, bytes / PAGE_SIZE() + 1);
	return rc;
}
