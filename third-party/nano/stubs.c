/**
 * @file
 *
 * @date May 29, 2014
 * @author: Anton Bondarev
 */

#include <regex.h>

int regcomp(regex_t *preg, const char *regex, int cflags) {
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

#if 0

#include <unistd.h>

int chown(const char *path, uid_t owner, gid_t group) {
	return 0;
}

int fchown(int fd, uid_t owner, gid_t group) {
	return 0;
}

#include <utime.h>

int utime(const char *path, const struct utimbuf *times) {
	return 0;
}

#include <sys/stat.h>

int fchmod(int fd, mode_t mode) {
	return 0;
}
#endif
