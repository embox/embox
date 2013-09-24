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

#include <embox/cmd.h>

EMBOX_CMD(exec);

typedef void item_print(const char *path, stat_t *sb);

static void print_usage(void) {
	printf("Usage: ls [-hlR] path\n");
}

static char dir_name[NAME_MAX];
static size_t dir_namel;
static int recursive;
static item_print *printer;

static void print_access(int flags) {
	putchar(flags & S_IROTH ? 'r' : '-');
	putchar(flags & S_IWOTH ? 'w' : '-');
	putchar(flags & S_IXOTH ? 'x' : '-');
}

#define BUFLEN 1024

static void printer_long(const char *path, stat_t *sb) {
	struct passwd pwd, *res;
	struct group grp, *gres;
	char buf[BUFLEN];

	putchar(sb->st_mode & S_IFDIR ? 'd' : '-');

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

	printf(" %s", path);

	if (sb->st_mode & S_IFDIR) {
		putchar('/');
	}

	putchar('\n');
}

static void printer_simple(const char *path, stat_t *sb) {
	printf("%s\n", path);
}

static void print(char *path, DIR *dir) {
	struct dirent *dent;

	while (NULL != (dent = readdir(dir))) {
		int pathlen = strlen(path);
		int dent_namel = strlen(dent->d_name);
		char line[pathlen + dent_namel + 3];
		stat_t sb;

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

			print(line, d);

			closedir(d);
		}
	}
}

static int exec(int argc, char **argv) {
	DIR *dir;

	int opt_cnt = 0;
	int opt, l;

	printer = printer_simple;
	recursive = 0;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "Rlh"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		case 'l':
			printer = printer_long;
			opt_cnt++;
			break;
		case 'R':
			recursive = 1;
			opt_cnt++;
			break;
		case '?':
			break;
		default:
			printf("ls: invalid option -- '%c'\n", optopt);
			return -EINVAL;
		}
	}

	if (optind < argc) {
		sprintf(dir_name, "%s", argv[optind]);
		// trim trailing slash
		l = strlen(dir_name);
		while (dir_name[l - 1] == '/') dir_name[--l] = 0;
	} else {
		sprintf(dir_name, "");
	}

	dir_namel = strlen(dir_name);

	if (NULL == (dir = opendir(dir_name))) {
		return -errno;
	}

	print(dir_name, dir);

	closedir(dir);

	return 0;
}
