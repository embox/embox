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
	errno = ENOSYS;
	return -1;
}

int setpriority(int which, id_t who, int value) {
	errno = ENOSYS;
	return -1;
}
