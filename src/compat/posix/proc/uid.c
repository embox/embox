/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.02.2013
 */

#include <errno.h>
#include <kernel/task/u_area.h>

#define ROOT_UID 0

uid_t getuid(void) {
	return task_self_u_area()->reuid;
}

uid_t geteuid(void) {
	return task_self_u_area()->euid;
}

uid_t getgid(void) {
	return task_self_u_area()->regid;
}

uid_t getegid(void) {
	return task_self_u_area()->egid;
}

int setuid(uid_t uid) {
	struct task_u_area *u_area = task_self_u_area();

	if (u_area->euid == ROOT_UID) {
		u_area->euid = uid;
		u_area->reuid = uid;
	} else if (u_area->reuid == uid) {
		u_area->euid = uid;
	} else {
		errno = EPERM;
		return -1;
	}

	return 0;
}

int seteuid(uid_t uid) {
	struct task_u_area *u_area = task_self_u_area();

	if (u_area->euid == ROOT_UID || u_area->reuid == uid) {
	       u_area->euid = uid;
	} else {
		errno = EPERM;
		return -1;
	}

	return 0;
}
