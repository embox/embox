/**
 * @file
 * @brief
 *
 * @date 09.01.2011
 * @author Anton Bondarev
 */

#ifndef SHELL_UTILS_H_
#define SHELL_UTILS_H_

/* *str becomes pointer to first non-space character*/
static void skip_spaces(char **str) {
	while (**str == ' ') {
		(*str)++;
	}
}

/* *str becomes pointer to first space or '\0' character*/
static void skip_word(char **str) {
	while (**str != '\0' && **str != ' ') {
		(*str)++;
	}
}

static int parse_str(char *cmdline, char **words) {
	size_t cnt = 0;
	while (*cmdline != '\0') {
		if (' ' == *cmdline) {
			*cmdline++ = '\0';
			skip_spaces(&cmdline);
		} else {
			words[cnt++] = cmdline;
			skip_word(&cmdline);
		}
	}
	return cnt;
}

#endif /* SHELL_UTILS_H_ */
