/*
 * @file
 * @brief
 *
 * @date 25.03.13
 * @author Anton Bondarev
 */

#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>

int gethostname(char *name, size_t len) {
	struct utsname utn;

	if (-1 == uname(&utn)) {
		return -1;
	}

	strncpy(name, utn.nodename, len);

	return 0;
}
