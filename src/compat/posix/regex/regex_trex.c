/**
 * @file
 *
 * @date Jun 13, 2014
 * @author Anton Bondarev
 */


#include <regex.h>
#include <trex.h>
#include <string.h>
#include <errno.h>

int regcomp(regex_t *preg, const char *regex, int cflags) {
	if (!preg || !regex) {
		return -EINVAL;
	}

	/* Trex is able to search only the first match in the whole string or in
	 * the specified range. Using trex_searchrange() we can get several
	 * matches, but we have to not allow the symbol '^' in the pattern */
	if (regex[0] == '^') {
		return -EINVAL;
	}

	preg->regex_error[0] = '\0';

	preg->regex_extended = trex_compile((char *)regex, preg->regex_error);

	if (!preg->regex_extended) {
		return -EINVAL;
	}

	return 0;
}

int regexec(const regex_t *preg, const char *string, size_t nmatch,
        regmatch_t pmatch[], int eflags) {
	const char _begin, _end;
	const char *begin = &_begin, *end = &_end;
	const char *str_beg = string, *str_end = string + strlen(string);
	int count = 0;

	if (!pmatch || !preg) {
		return -EINVAL;
	}

	if (!preg->regex_extended) {
		return -EINVAL;
	}

	while (trex_searchrange(preg->regex_extended, str_beg, str_end,
			&begin, &end)) {
		if (nmatch <= count) {
			break;
		}

		pmatch[count].rm_so = begin - string;
		pmatch[count].rm_eo = end - string;

		str_beg = begin + 1;
		count++;
	}

	pmatch[count].rm_so = 0;
	pmatch[count].rm_eo = 0;

	if (!count) {
		return -REG_NOMATCH;
	}

	return 0;
}

size_t regerror(int errcode, const regex_t *preg, char *errbuf,
        size_t errbuf_size) {
	if (!errbuf_size || !errbuf) {
		return strlen(preg->regex_error);
	}

	strncpy(errbuf, preg->regex_error, errbuf_size);
	return 0;
}

void regfree(regex_t *preg) {
	if (!preg->regex_extended) {
		return;
	}

	trex_free(preg->regex_extended);
	preg->regex_extended = NULL;
}
