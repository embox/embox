/**
 * @file
 *
 * @date Sep 26, 2020
 * @author Anton Bondarev
 */
#include <errno.h>
#include <sys/resource.h>

int setrlimit(int resource, const struct rlimit *rlp) {
	(void) resource;
	(void) rlp;
	return SET_ERRNO(ENOSYS);
}

int getrlimit(int resource, struct rlimit *rlp) {
	(void) resource;
	(void) rlp;
	return SET_ERRNO(ENOSYS);
}
