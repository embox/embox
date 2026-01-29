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

int sched_getparam(pid_t pid, struct sched_param *param) {
	int policy;

	if (pid == 0) {
		pthread_getschedparam(pthread_self(), &policy, param);
	} else {
		log_debug("stub %s(%ld, %p)", __func__, (long) pid, param);
	}

	return 0;
}
