/**
 * @file
 * @brief
 *
 * @date 28.01.2026
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <errno.h>
#include <sys/types.h>

#include <sched.h>

int sched_getscheduler(pid_t pid) {
	int policy;
	struct sched_param param;

	if (pid == 0) {
		pthread_getschedparam(pthread_self(), &policy, &param);
	} else {
		log_debug("stub %s(%ld)", __func__, (long) pid);
	}

	return 0;
}
