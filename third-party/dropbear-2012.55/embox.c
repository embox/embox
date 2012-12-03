/**
 * @file
 *
 * @brief
 * @date 23.11.2012
 * @author Alexander Kalmuk
 */


#include "includes.h"

static struct passwd embox_passwd = {
		.pw_name = "embox",
		.pw_uid = 0,
		.pw_gid = 0,
		.pw_dir = "/",
		.pw_shell = "tish",
		.pw_passwd = "123"
};

char *strdup(const char *s) {
	char *new = malloc(strlen(s));
	strcpy(new, s);
	return new;
}

struct passwd *getpwnam(const char *name) {
	return &embox_passwd;
}

char *crypt(const char *key, const char *salt) {
	return embox_passwd.pw_passwd;
}

void exit(int status) {
	task_exit(NULL);
}

void _exit(int status) {
	task_exit(NULL);
}

struct passwd *getpwuid(uid_t uid) {
	return &embox_passwd;
}

int gethostname(char *name, size_t len) {
	name = embox_passwd.pw_name;
	return 0;
}


