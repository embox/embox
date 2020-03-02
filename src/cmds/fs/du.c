/**
 * @file
 * @brief
 *
 * @date 26.02.20
 * @author Bartlomiej Kocot
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>

#define BLK_SIZE 512

static void print_usage(void) {
	printf("Usage: du [-b BYTES] [-B BLOCK_SIZE] [FILE_OR_DIR] ...\n");
}

long long divide_with_round_up(long long size, size_t block_size) {
	long long  result = size / block_size;
	return result * block_size == size ? result : result + 1;
}

long long print_file_mem_usage(const char *path, struct stat *sb, size_t block_size) {
	long long size = divide_with_round_up((long long)sb->st_size, block_size);
	printf("%-21llu %s\n", size, path);
	return size;
}

long long print(char *path, DIR *dir, size_t block_size) {
	struct dirent *dent;
	long long dir_size = 0;
	char *line;

	while (NULL != (dent = readdir(dir))) {
		int pathlen = strlen(path);
		int dent_namel = strlen(dent->d_name);
		line = (char*)malloc(sizeof(char) * (pathlen + dent_namel + 3));
		if (line == NULL) {
			printf("Failed to allocate memory for buffer!\n");
			return -1;
		}
		struct stat sb;

		if (pathlen > 0) {
			sprintf(line, "%s/%s", path, dent->d_name);
		} else {
			strcpy(line, dent->d_name);
		}

		if (-1 == stat(line, &sb)) {
			printf("Cannot stat %s\n", line);
			free(line);
			continue;
		}

		if (S_ISREG(sb.st_mode)) {
			dir_size += print_file_mem_usage(line, &sb, block_size);
		} else if (S_ISDIR(sb.st_mode)) {
			DIR *d;

			if (NULL == (d = opendir(line))) {
				printf("Cannot recurse to %s\n", line);
			}
			long long next_dir_size = print(line, d, block_size);
			if (next_dir_size != -1) {
				dir_size += next_dir_size;
			} else {
				return -1;
			}
			closedir(d);
		}
		free(line);
    }

	printf("%-21llu %s\n", dir_size, path);
	return dir_size;
}

int main(int argc, char *argv[]) {
	int opt;
	DIR *dir;
	char dir_name[NAME_MAX];
	long long dir_size;
	size_t block_size = BLK_SIZE;

	while (-1 != (opt = getopt(argc, argv, "B:bh"))) {
		switch(opt) {
			case 'h':
				print_usage();
				return 0;
			case 'b':
				block_size = 1;
				break;
			case 'B':
				if ((optarg == NULL) || (!sscanf(optarg, "%zu", &block_size)) || block_size <= 0) {
					printf("du -B: positive number expected\n");
					print_usage();
					return -EINVAL;
				}
				break;
			case '?':
				break;
			default:
				printf("du: invalid option -- '%c'\n", optopt);
				return -EINVAL;
		}
	}

	if (optind < argc) {
		struct stat sb;

		if (-1 == stat(argv[optind], &sb)) {
			return 0;
		}

		if (!S_ISDIR(sb.st_mode)) {
			print_file_mem_usage(argv[optind], &sb, block_size);
			return 0;
		}

		sprintf(dir_name, "%s", argv[optind]);
	} else {
		strcpy(dir_name, ".");
	}

	if (NULL == (dir = opendir(dir_name))) {
		return -errno;
	}

	dir_size = print(dir_name, dir, block_size);
	closedir(dir);

	if (dir_size == -1) {
		return -ENOMEM;
	}
	return 0;
}
