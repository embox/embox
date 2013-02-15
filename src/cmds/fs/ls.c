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

#include <sys/stat.h>
#include <dirent.h>

#include <embox/cmd.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: ls [-hl] path\n");
}


static char dir_name[MAX_LENGTH_FILE_NAME];
static size_t dir_namel;

/*
static void print_long_list(char *path, node_t *node, int recursive) {
	node_t *item;
	stat_t sb;
	char time_buff[26]; //ctime_r requires it least 26 byte buffer lentgh
	printf("%s\t%s\t%s\t\t\t%s\n", "mode", "size", "mtime", "name");
	tree_foreach_children(item, &node->tree_link, tree_link) {
		//stat((char *) item->name, &sb);
		ctime_r((time_t *) &(sb.st_mtime), time_buff);
		printf("%d\t%d\t%s\t%s\n",
			sb.st_mode,
			sb.st_size,
			time_buff,
			(char *) item->name);
	}
}
*/

static void print_folder(char *path, DIR *dir, int recursive) {
	struct dirent *dent;

	while (NULL != (dent = readdir(dir))) {
		int pathlen = strlen(path);
		int dent_namel = strlen(dent->d_name);
		char line[pathlen + dent_namel + 3];

		if (pathlen > 0) {
			sprintf(line, "%s/%s", path, dent->d_name);
		} else {
			strcpy(line, dent->d_name);
		}

		printf("%s\n", line);

		if (recursive) {
			char this_fullpath[pathlen + dent_namel + dir_namel];
			stat_t sb;
			DIR *d;

			sprintf(this_fullpath, "%s/%s", dir_name, line);

			stat(this_fullpath, &sb);

			if (0 == (sb.st_mode & S_IFDIR)) {
				continue;
			}

			if (NULL == (d = opendir(this_fullpath))) {
				printf("Cannot recurse %s\n", this_fullpath);
			}

			print_folder(line, d, recursive);

			closedir(d);
		}
	}

}


static int exec(int argc, char **argv) {
	DIR *dir;

	void (*print_func)(char *path, DIR *d, int recursive) = print_folder;
	int recursive = 0;

	int opt_cnt = 0;
	int opt;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "Rlh"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		/*case 'l':
			//long_list = 1;
			print_func = print_long_list;
			opt_cnt++;
			break;*/
		case 'R':
			recursive = 1;
			print_func = print_folder;
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
		sprintf(dir_name, "/");
	}

	dir_namel = strlen(dir_name);

	if (NULL == (dir = opendir(dir_name))) {
		return errno;
	}

	print_func("", dir, recursive);

	closedir(dir);

	return 0;
}
