/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.02.23
 */
#include <errno.h>
#include <sys/resource.h>

int getrusage(int who, struct rusage *usage) {
	(void)who;
	(void)usage;
	return SET_ERRNO(ENOSYS);
}
