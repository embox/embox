/**
 * @brief https://pubs.opengroup.org/onlinepubs/9699919799/functions/dirname.html
 *
 * @date 22.01.23
 * @author Aleksey Zhmulin
 */

#include <libgen.h>

#include <lib/cwalk.h>

char *dirname(char *path) {
	struct cwk_segment segment;

	if (!path) {
		return ".";
	}

	if (cwk_path_get_last_segment(path, &segment)) {
		if (cwk_path_get_previous_segment(&segment)) {
			*(char *)segment.end = '\0';
			return path;
		}
	}

	return cwk_path_is_absolute(path) ? "/" : ".";
}
