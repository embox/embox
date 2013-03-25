/*
 * @file
 *
 * @date Mar 25, 2013
 * @author: Anton Bondarev
 */
#include <sys/utsname.h>
#include <unistd.h>
#include <string.h>

int gethostname(char *name, size_t len) {
	struct utsname utn;
	uname(&utn);
	strncpy(name, utn.nodename, len);
	return 0;
}
