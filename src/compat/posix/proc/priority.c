/**
 * @file
 * @brief
 *
 * @date 11.03.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <sys/resource.h>

int getpriority(int which, id_t who) {
#if 0
	errno = ENOSYS;
	return -1;
#endif
	return 0;
}

int setpriority(int which, id_t who, int value) {
#if 0
	errno = ENOSYS;
	return -1;
#endif
	return 0;
}
