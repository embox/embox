/**
 * @file
 *
 * @brief
 * @date 23.11.2012
 * @author Alexander Kalmuk
 */


#include "includes.h"


#if 0
#define PWD_CHARLEN 64

static struct passwd embox_passwd;
static char charbuf[PWD_CHARLEN];

struct passwd *getpwnam(const char *name) {
	struct passwd *ret;

	getpwnam_r(name, &embox_passwd, charbuf, PWD_CHARLEN, &ret);

	return ret;
}
#endif

char *crypt(const char *key, const char *salt) {
	struct passwd *pwd;
	static char buff[0x20];

	pwd = getpwuid(geteuid());
	if (pwd == NULL) {
		return 0;
	}
	memcpy(buff, pwd->pw_passwd, sizeof(buff));

	return buff;
}


