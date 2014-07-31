/**
 * @file
 *
 * @brief
 *
 * @date 13.09.2011
 * @author Anton Bondarev
 */

#include <kernel/task.h>
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *status, int options) {
	return task_waitpid_posix(pid, status, options);
}

pid_t wait(int *status) {
	return waitpid(-1, status, 0);
}

struct rusage;

pid_t wait3(int *status, int options, struct rusage *rusage) {
	return waitpid(-1, status, options);

}

pid_t wait4(pid_t pid, int *status, int options, struct rusage *rusage) {
	return waitpid(pid, status, options);

}

