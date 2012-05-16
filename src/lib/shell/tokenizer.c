/*
 * @file
 * @brief Shell command tokenizing.
 *
 * @date 26.04.2012
 * @author Eldar Abusalimov
 *         - cmdline_XXX functions
 * @author Alexander Kalmuk
 *         - quotes parsing
 */

#include <types.h>
#include <errno.h>
#include <cmd/cmdline.h>
#include <ctype.h>
#include <stdio.h>

#define BUF_INP_SIZE 100

/* Parse single quotes */
static char *parse_sq(char **str) {
	char *ret;
	char *ptr = *str;

	ret = ++ptr;

	while (*ptr != '\'') {
		if(*ptr == '\0') {
			return NULL;
		}
		ptr++;
	}

	*ptr = ' ';
	*str = ptr;

	return ret;
}

/* Parse double quotes */
static char *parse_dq(char **str) {
	/* right_ptr - point to current processing character,
	 * left_ptr  - point to the end of processed substring */
	char *left_ptr, *right_ptr, *ret;

	left_ptr = right_ptr = ret = *str;

	/* move before " */
	left_ptr--;

	/* search for '\0' or '\"' */
	while (1) {
		left_ptr++;
		right_ptr++;

		if (*right_ptr == '\0') {
			*str = left_ptr;
			return (char*) NULL;
		}

		if (*right_ptr == '\"' && *(right_ptr - 1) != '\\') {
			*right_ptr = ' ';
			break;
		}

		if (*right_ptr == '\\') {
			right_ptr++;
			if (*right_ptr != '\\' && *right_ptr != '\"' && *right_ptr != '$') {
				right_ptr--;
			} else {
				*(right_ptr - 1) = ' ';
			}
		}

		*left_ptr = *right_ptr;
		*right_ptr = ' ';
	}

	*str = left_ptr;

	return ret;
}

static char *cmdline_next_token(char **str) {
	char *ret;
	char *ptr = *str;

	/* Skip whitespace characters. */
	while (isspace(*ptr)) {
		++ptr;
	}

	if (!*ptr) {
		/* Got EOL. */
		return NULL;
	}

	/* Found start of token. */
	ret = (char *) ptr;

	/* Now skip all non-whitespace characters to get end of token. */
	while (*ptr && !isspace(*ptr)) {
		++ptr;
	}

	/* Save end of token into the argument. */
	*str = ptr;

	return ret;
}

int cmdline_tokenize(char *string, char **argv) {
	int argc = 0;
	char *token;

	while (*string != '\0') {
		if (*string == '\'') {
			argv[argc++] = (token = parse_sq(&string));
		} else if(*string == '\"') {
			argv[argc++] = (token = parse_dq(&string));
		} else {
			argv[argc++] = (token = cmdline_next_token(&string));
		}

		if (*string) {
			*string++ = '\0';
		}
	}

	return argc;
}

