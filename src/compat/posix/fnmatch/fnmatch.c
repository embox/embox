/**
 * @file
 * @brief fnmatch_function, now working with two flags - FNM_MATCH and FNM_PATHNAME
 * @date 20.12.18
 * @author Filipp Chubukov
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fnmatch.h>

static int bracket_check(const char *pattern) {
	while (*pattern != ']') {
		if (*pattern == '/') {
			return 0;
		}
		pattern++;
	}
	return 1;
}

static int fnmatch_string(const char *pattern, const char *string, int flags) {
	int flag;
	int denial_flag;
	while (*pattern != '\0') {
		if ((*string == '/') && (*pattern != '/') && (*pattern != '*') && (flags == FNM_PATHNAME)) {
			return FNM_NOMATCH;
		}

		if ((*string == '\0') && (*pattern != '*')) {
			return FNM_NOMATCH;
		}

		switch (*pattern) {
			case '?':
				break;  

			case '*':
				pattern++;
				if (*pattern == '\0' && flags != FNM_PATHNAME) {
					return 0;
				}
				while (*string != '\0') {
					if (!fnmatch_string(pattern, string, flags)) {
						return 0;
					}
					if (*string == '/' && flags == FNM_PATHNAME) {
						return FNM_NOMATCH;
					}
					string++;
				}
				break;	

			case '[':
				flag = 0;
				denial_flag = 0;
				if (flags == FNM_PATHNAME) {
					if (!bracket_check(pattern)) {
						if (*pattern != *string) {
							return FNM_NOMATCH;
						}
						return fnmatch_string(pattern + 1, string + 1, flags);
					}
				}	

				pattern++;
				if (*pattern == '^') {
					denial_flag = 1;
					pattern++;
				}
				while (*pattern != ']') {
					if (*string == *pattern) {
						flag = 1;
					}
					pattern++;
				}

				if ((!flag && !denial_flag) || (flag && denial_flag)) {
					return FNM_NOMATCH;
				}
				break;

			default:
				if (*pattern != *string) {
					return FNM_NOMATCH;
				}

		}
		string++;
		pattern++;
	}
	if (*string != '\0') {
		return FNM_NOMATCH;
	}
	return 0;
}

int fnmatch(const char *pattern, const char *string, int flags) {

	if (pattern == NULL || string == NULL) {
		return EINVAL;
	}
	/* Begin of pattern-check */
	char *bracket_open = strrchr(pattern, '[');
	char *bracket_close = strrchr(pattern, ']');

	if ((bracket_open != NULL) && (bracket_close < bracket_open)) {
		return EINVAL;
	}
	/* End of pattern-check */

	return fnmatch_string(pattern, string, flags); 
}
