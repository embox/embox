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
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include <dirent.h>
#include <sys/stat.h>

#include <embox/cmd.h>

EMBOX_CMD(exec);

typedef void item_print(const char *path, stat_t *sb);

static void print_usage(void) {
	printf("Usage: ls [-hlR] path\n");
}

static char dir_name[MAX_LENGTH_FILE_NAME];
static size_t dir_namel;
static int recursive;
static item_print *printer;

static void printer_long(const char *path, stat_t *sb) {
	printf("%3o %6d %6d %s", 0777 & sb->st_mode, sb->st_uid, sb->st_gid, path);
	if (sb->st_mode & S_IFDIR) {
		printf("/");
	}
	printf("\n");
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

		stat(line, &sb);

		printer(line, &sb);

		if (0 != (sb.st_mode & S_IFDIR) && recursive) {
			DIR *d;

			if (NULL == (d = opendir(line))) {
				printf("Cannot recurse %s\n", line);
			}

			print(line, d);

			closedir(d);
		}
	}

}

static int exec(int argc, char **argv) {
	DIR *dir;

	int opt_cnt = 0;
	int opt;

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
			return -1;
		}
	}

	if (optind < argc) {
		sprintf(dir_name, "%s", argv[optind]);
	} else {
		sprintf(dir_name, "");
	}

	dir_namel = strlen(dir_name);

	if (NULL == (dir = opendir(dir_name))) {
		return errno;
	}

	print(dir_name, dir);

	closedir(dir);

	return 0;
}
