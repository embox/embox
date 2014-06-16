/**
 * @file
 *
 * @date Jun 13, 2014
 * @author Anton Bondarev
 */


#include <regex.h>
#include <trex.h>

int regcomp(regex_t *preg, const char *regex, int cflags) {
	preg->regex_extended = trex_compile((char *)regex, (const char**) &preg->regex_error);
	return 0;
}

int regexec(const regex_t *preg, const char *string, size_t nmatch,
        regmatch_t pmatch[], int eflags) {
	return 0;
}

size_t regerror(int errcode, const regex_t *preg, char *errbuf,
        size_t errbuf_size) {
	return 0;
}

void regfree(regex_t *preg) {

}
