/**
 * @file
 * @brief List directory contents.
 *
 * @date 02.07.09
 * @author Anton Bondarev
 * @author Anton Kozlov
 *	    -- rewritten to use opendir
 */

#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include <lib/cwalk.h>

typedef void item_print(const char *path, struct stat *sb);

static void print_usage(void) {
	printf("Usage: ls [-hlR] path\n");
}

static void print_access(int flags) {
	putchar(flags & S_IROTH ? 'r' : '-');
	putchar(flags & S_IWOTH ? 'w' : '-');
	putchar(flags & S_IXOTH ? 'x' : '-');
}

#define BUFLEN 1024

static void printer_simple(const char *path, struct stat *sb) {
	if (cwk_path_is_absolute(path)) {
		path++;
	}
	printf("\t%s\n", path);
}

static void printer_long(const char *path, struct stat *sb) {
	struct passwd pwd, *res;
	struct group grp, *gres;
	char buf[BUFLEN];
	char type;

	switch (sb->st_mode & S_IFMT) {
	case S_IFDIR:
		type = 'd';
		break;
	case S_IFBLK:
		type = 'b';
		break;
	case S_IFCHR:
		type = 'c';
		break;
	case S_IFIFO:
		type = 'p';
		break;
	case S_IFSOCK:
		type = 's';
		break;
	default:
		type = '-';
		break;
	}

	putchar('\t');
	putchar(type);

	print_access(sb->st_mode >> 6);
	print_access(sb->st_mode >> 3);
	print_access(sb->st_mode);

	getpwuid_r(sb->st_uid, &pwd, buf, BUFLEN, &res);

	if (NULL == res) {
		printf(" %10d", sb->st_uid);
	}
	else {
		printf(" %10s", res->pw_name);
	}

	getgrgid_r(sb->st_gid, &grp, buf, BUFLEN, &gres);

	if (NULL == res) {
		printf(" %10d", sb->st_uid);
	}
	else {
		printf(" %10s", gres->gr_name);
	}

	printf(" %10d", (int)(sb->st_size));
	printer_simple(path, sb);
}

static void print(const char *path, const char *beg, char *end, DIR *dir,
    int recursive, item_print *printer) {
	DIR *d;
	char *endptr;
	struct dirent *dent;
	struct stat sb;

	while (NULL != (dent = readdir(dir))) {
		endptr = stpcpy(end, dent->d_name);

		if (-1 == stat(path, &sb)) {
			printf("Cannot stat %s\n", path);
			continue;
		}

		if (S_ISDIR(sb.st_mode)) {
			endptr = stpcpy(endptr, "/");
		}

		printer(beg, &sb);

		if (S_ISDIR(sb.st_mode) && recursive) {
			if (NULL == (d = opendir(path))) {
				printf("Cannot recurse to %s\n", path);
			}

			print(path, beg, endptr, d, recursive, printer);

			closedir(d);
		}
	}
}

int main(int argc, char **argv) {
	item_print *printer;
	char *endptr;
	DIR *dir;
	int opt;
	int recursive;
	size_t path_len;
	size_t root_len;
	struct stat sb;
	char path[PATH_MAX];

	printer = printer_simple;
	recursive = 0;

	while (-1 != (opt = getopt(argc, argv, "Rlh"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'l':
			printer = printer_long;
			break;
		case 'R':
			recursive = 1;
			break;
		case '?':
			break;
		default:
			printf("ls: invalid option -- '%c'\n", optopt);
			return -EINVAL;
		}
	}

	if (optind < argc) {
		do {
			cwk_path_normalize(argv[optind], path, sizeof(path));

			if (-1 == stat(path, &sb)) {
				return -errno;
			}

			if (!S_ISDIR(sb.st_mode)) {
				printer(path, &sb);
				continue;
			}

			if (NULL == (dir = opendir(path))) {
				return -errno;
			}

			path_len = strlen(path);
			endptr = path + path_len;
			cwk_path_get_root(path, &root_len);
			if (path_len != root_len) {
				endptr = stpcpy(endptr, "/");
			}

			print(path, endptr, endptr, dir, recursive, printer);

			closedir(dir);
		} while (optind++ < argc - 1);
	}
	else {
		endptr = stpcpy(path, "./");

		if (NULL == (dir = opendir(path))) {
			return -errno;
		}

		print(path, endptr, endptr, dir, recursive, printer);

		closedir(dir);
	}

	return 0;
}
