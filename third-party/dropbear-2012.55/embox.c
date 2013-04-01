/**
 * @file
 *
 * @brief
 * @date 23.11.2012
 * @author Alexander Kalmuk
 */


#include "includes.h"

#define PWD_CHARLEN 64

static struct passwd embox_passwd;
static char charbuf[PWD_CHARLEN];

struct passwd *getpwnam(const char *name) {
	struct passwd *ret;

	getpwnam_r(name, &embox_passwd, charbuf, PWD_CHARLEN, &ret);

	return ret;
}

char *crypt(const char *key, const char *salt) {
	return embox_passwd.pw_passwd;
}


