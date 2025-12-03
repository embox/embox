/**
 * @file
 *
 * @date May 29, 2014
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>

int dn_expand(unsigned char *msg, unsigned char *eomorig,
              unsigned char *comp_dn, char *exp_dn,
              int length) {
	return -1;
}

int res_query(const char *dname, int class, int type,
              unsigned char *answer, int anslen) {
	return -1;
}

char *mktemp(char *template) {
	if (template) {
		*template = 0;
	}
	return template;
}


#include <dirent.h>
void seekdir(DIR *dirp, long offset) {
}

long telldir(DIR *dirp) {
	errno = EPERM;
	return -1;
}

int fnmatch(const char *pattern, const char *string, int flags) {
	return -1;
}

int fork(void) {
	return -1;
}

int getgroups(int gidsetsize, gid_t grouplist[]) {
	return -1;
}
