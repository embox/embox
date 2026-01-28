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

#include <kernel/sched.h>

#include <sched.h>

int sched_getscheduler(pid_t pid) {
	log_debug("stub %s(%ld)", __func__, (long) pid);
	return 0;
}
