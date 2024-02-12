/**
 * @brief https://pubs.opengroup.org/onlinepubs/9699919799/functions/basename.html
 *
 * @date 22.01.23
 * @author Aleksey Zhmulin
 */

#include <libgen.h>

#include <lib/cwalk.h>

char *basename(char *path) {
	struct cwk_segment segment;

	if (!path || (*path == '\0')) {
		return ".";
	}

	if (cwk_path_get_last_segment(path, &segment)) {
		*(char *)segment.end = '\0';
		return (char *)segment.begin;
	}

	return "/";
}
