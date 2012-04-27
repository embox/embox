/*
 * @file
 * @brief Shell command tokenizing.
 *
 * @date 26.04.2012
 * @author Eldar Abusalimov
 *         - cmd_XXX functions
 * @author Alexander Kalmuk
 *         - quotes parsing
 */

#include <types.h>
#include <errno.h>
#include <cmd/cmdline.h>
#include <ctype.h>
#include <stdio.h>

/* Parse single quotes */
static char *parse_sq(const char **str) {
	char *ret;
	char *ptr = (char *) *str;

	ret = ++ptr;

	while (*ptr != '\'') {
		if(*ptr == '\0') {
			return NULL;
		}
		ptr++;
	}

	*ptr = ' ';
	*str = (const char *) ptr;

	return ret;
}

/* Parse double quotes */
static char *parse_dq(const char **str) {
	/* right_ptr - point to current processing character,
	 * left_ptr  - point to the end of processed substring */
	char *left_ptr, *right_ptr, *ret;

	left_ptr = right_ptr = ret = (char *) *str;

	/* move before " */
	left_ptr--;

	/* search for '\0' or '\"' */
	while (1) {
		left_ptr++;
		right_ptr++;

		if (*right_ptr == '\0') {
			*str = (const char *) left_ptr;
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

	*str = (const char *) left_ptr;

	return ret;
}

static char *cmdline_next_token(const char **str) {
	char *ret;
	const char *ptr = (const char *) *str;

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
	*str = (const char *) ptr;

	return ret;
}

int cmdline_tokenize(char *string, char **argv) {
	int argc = 0;
	char *token;

	while (*string != '\0') {
		if (*string == '\'') {
			argv[argc++] = (token = parse_sq((const char **) &string));
		} else if(*string == '\"') {
			argv[argc++] = (token = parse_dq((const char **) &string));
		} else {
			argv[argc++] = (token = cmdline_next_token((const char **) &string));
		}

		if (*string) {
			*string++ = '\0';
		}
	}

	return argc;
}

