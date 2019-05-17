/**
 * @file
 * @brief List directory contents.
 *
 * @date 02.07.09
 * @author Anton Bondarev
 * @author Anton Kozlov
 *	    -- rewritten to use opendir
 */

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <limits.h>

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
	printf(" %s\n", path);
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

	putchar(type);

	print_access(sb->st_mode >> 6);
	print_access(sb->st_mode >> 3);
	print_access(sb->st_mode);

	getpwuid_r(sb->st_uid, &pwd, buf, BUFLEN, &res);

	if (NULL == res) {
		printf(" %10d", sb->st_uid);
	} else {
		printf(" %10s", res->pw_name);
	}

	getgrgid_r(sb->st_gid, &grp, buf, BUFLEN, &gres);

	if (NULL == res) {
		printf(" %10d", sb->st_uid);
	} else {
		printf(" %10s", gres->gr_name);
	}

	printf(" %10d", (int)(sb->st_size));
	printer_simple(path, sb);
}

static void print(char *path, DIR *dir, int recursive, item_print *printer) {
	struct dirent *dent;

	while (NULL != (dent = readdir(dir))) {
		int pathlen = strlen(path);
		int dent_namel = strlen(dent->d_name);
		char line[pathlen + dent_namel + 3];
		struct stat sb;

		if (pathlen > 0) {
			sprintf(line, "%s/%s", path, dent->d_name);
		} else {
			strcpy(line, dent->d_name);
		}

		if (-1 == stat(line, &sb)) {
			printf("Cannot stat %s\n", line);
			continue;
		}

		printer(line, &sb);

		if (sb.st_mode & S_IFDIR && recursive) {
			DIR *d;

			if (NULL == (d = opendir(line))) {
				printf("Cannot recurse to %s\n", line);
			}

			print(line, d, recursive, printer);

			closedir(d);
		}
	}
}

int main(int argc, char **argv) {
	DIR *dir;

	int opt;
	char dir_name[NAME_MAX];
	int recursive;
	item_print *printer;

	printer = printer_simple;
	recursive = 0;

	while (-1 != (opt = getopt(argc, argv, "Rlh"))) {
		switch(opt) {
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
		struct stat sb;

		if (-1 == stat(argv[optind], &sb)) {
			return -errno;
		}

		if (~sb.st_mode & S_IFDIR) {
			printer(argv[optind], &sb);
			return 0;
		}

		sprintf(dir_name, "%s", argv[optind]);
	} else {
		sprintf(dir_name, "%s", "");
	}

	if (NULL == (dir = opendir(dir_name))) {
		return -errno;
	}

	print(dir_name, dir, recursive, printer);

	closedir(dir);

	return 0;
}
