/**
 * @file
 *
 * @date 28.10.13
 * @author Anton Bondarev
 */

#include <limits.h>
#include <string.h>
#include <libgen.h>

static char *path_final_slash(char *path) {
	size_t path_len = strlen(path);
	char *c;

	c = path + path_len - 1;
	while (*c == '/') {
		*c-- = '\0';
		path_len--;
	}

	while (path_len > 0 && *c != '/') {
		c--;
		path_len--;
	}

	if (path_len > 0) {
		return c;
	}

	return NULL;
}

char * basename(char *path) {
	char *sl;

	if (!path || *path == '\0') {
		return ".";
	}

	sl = path_final_slash(path);

	if (!sl) {
		if (!*path)
			/* path wasn't empty, but now it does =>
 			 * only trailing /// was presented */
			return strcpy(path, "/");
		return path;
	}

	return sl + 1;
}

char * dirname(char *path) {
	char *sl;

	if (!path || *path == '\0') {
		return ".";
	}

	sl = path_final_slash(path);
	if (!sl) {
		if (!*path)
			/* path wasn't empty, but now it does =>
 			 * only trailing /// was presented */
			return strcpy(path, "/");
		return strcpy(path, ".");
	}

	if (path == sl) {
		*(sl + 1) = '\0';
	} else {
		*sl = '\0';
	}

	return path;
}

