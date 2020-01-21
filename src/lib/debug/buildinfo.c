/**
 * @file buildinfo.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 20.01.2020
 */

#include <stdbool.h>

extern char *buildinfo_begin;
extern char *buildinfo_end;

static bool buildinfo_presents(void) {
	return 0 != (&buildinfo_end - &buildinfo_begin);
}

char *buildinfo_date(void) {
	if (!buildinfo_presents()) {
		return "";
	}

	return (char *) &buildinfo_begin;
}
