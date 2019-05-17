/**
 * @file
 * @brief
 *
 * @date 25.02.14
 * @author Vita Loginova
 * @author Alex Kalmuk
 *         -- Adaptation for POSIX
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

#define CHMOD_MAX_MODES 10

static void help_invalid_mode(const char *mode) {
	printf("chmod: invalid mode: '%s'\n"
			"Try 'chmod -h' for more information.\n", mode);
}

static void help(void) {
	printf("Usage: chmod [OPTION]... MODE[,MODE]... FILE...\n");
}

static int build_mode(char *mode_str, mode_t *mode) {
	char op;
	char *modep = mode_str;
	int count = 0;
	mode_t type = 0;
	mode_t mask = 0;

parse:
	if (count >= CHMOD_MAX_MODES) {
		printf("chmod: too many modes: %s\n", mode_str);
		return -1;
	}

	while (*modep != '=' && *modep != '+' && *modep != '-') {
		switch(*(modep++)) {
		case 'u':
			type |= 0700;
			break;
		case 'g':
			type |= 0070;
			break;
		case 'o':
			type |= 0007;
			break;
		case 'a':
			type |= 0777;
		break;
		default:
			help_invalid_mode(mode_str);
			return -1;
		}
	}

	if (!type) {
		type |= 0777;
	}

	op = *(modep++);

	while (*modep != '\0') {
		switch(*(modep++)) {
		case 'r':
			mask |= 0444;
			break;
		case 'w':
			mask |= 0222;
			break;
		case 'x':
			mask |= 0111;
			break;
		//case 'X':
		//	dir_mask |= 0111;
		//	break;
		case ',':
			count++;
			goto parse;
		default:
			help_invalid_mode(mode_str);
			return -1;
		}
	}

	switch(op) {
	case '+':
		*mode |= (mask & type);
		break;
	case '-':
		*mode &= ~(mask & type);
		break;
	case '=':
		*mode &= ~type;
		*mode |= mask & type;
		break;
	default:
		help_invalid_mode(mode_str);
		return -1;
	}

	return 0;
}

static int change_mode(char *path, int recursive, char *mode_str) {
	struct dirent *dent;
	DIR *dir;
	struct stat sb;
	mode_t mode;

	if (-1 == stat(path, &sb)) {
		printf("Cannot stat %s\n", path);
		return -1;
	}
	mode = sb.st_mode;
	if (0 > build_mode(mode_str, &mode)) {
		return -1;
	}

	if (!(sb.st_mode & S_IFDIR && recursive)) {
		return chmod(path, mode);
	}

	if (NULL == (dir = opendir(path))) {
		return -1;
	}

	while (NULL != (dent = readdir(dir))) {
		int pathlen = strlen(path);
		int dent_namel = strlen(dent->d_name);
		char line[pathlen + dent_namel + 3];

		if (pathlen > 0) {
			sprintf(line, "%s/%s", path, dent->d_name);
		} else {
			strcpy(line, dent->d_name);
		}

		if (-1 == stat(line, &sb)) {
			printf("Cannot stat %s\n", line);
			continue;
		}
		mode = sb.st_mode;
		if (0 > build_mode(mode_str, &mode)) {
			continue;
		}

		chmod(line, mode);

		if (sb.st_mode & S_IFDIR && recursive) {
			DIR *d;

			if (NULL == (d = opendir(line))) {
				printf("Cannot recurse to %s\n", line);
			}

			change_mode(line, recursive, mode_str);

			closedir(d);
		}
	}

	closedir(dir);
	return 0;
}

int main(int argc, char **argv) {
	int opt, is_recursive = 0;
	char *path, *mode_str;

	if (argc <= 1) {
		help();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "Rh"))) {
		switch (opt) {
		case 'R':
			is_recursive = 1;
			break;
		case 'h':
			help();
			return 0;
		default:
			printf("chmod: invalid option -%c\n", optopt);
			help();
			return 0;
		}
	}

	if (optind + 2 != argc) {
		help();
		return 0;
	}

	mode_str = argv[optind];
	path = argv[optind + 1];

	change_mode(path, is_recursive, mode_str);

	return 0;
}
