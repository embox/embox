/**
 * @file
 * @brief
 *	TODO: have no support for saved set-user-ID
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

int setreuid(uid_t ruid, uid_t euid) {
	struct task_u_area *u_area = task_self_u_area();

	if (ruid != (uid_t)-1) {
		if (u_area->euid == ROOT_UID || ruid == u_area->reuid
				|| ruid == u_area->euid) {
			u_area->reuid = ruid;
		} else {
			SET_ERRNO(EPERM);
			return -1;
		}
	}

	if (euid != (uid_t)-1) {
		if (u_area->euid == ROOT_UID || euid == u_area->reuid
				|| euid == u_area->euid) {
			u_area->euid = euid;
		} else {
			SET_ERRNO(EPERM);
			return -1;
		}
	}

	return 0;
}

int setuid(uid_t uid) {
	struct task_u_area *u_area = task_self_u_area();

	if (u_area->euid == ROOT_UID) {
		u_area->euid = uid;
		u_area->reuid = uid;
	} else if (u_area->reuid == uid) {
		u_area->euid = uid;
	} else {
		SET_ERRNO(EPERM);
		return -1;
	}

	return 0;
}

int seteuid(uid_t uid) {
	struct task_u_area *u_area = task_self_u_area();

	if (u_area->euid == ROOT_UID || u_area->reuid == uid) {
	       u_area->euid = uid;
	} else {
		SET_ERRNO(EPERM);
		return -1;
	}

	return 0;
}

int setregid(gid_t rgid, gid_t egid) {
	struct task_u_area *u_area = task_self_u_area();

	if (rgid != (gid_t)-1) {
		if (u_area->egid == ROOT_UID || rgid == u_area->regid
				|| rgid == u_area->egid) {
			u_area->regid = rgid;
		} else {
			SET_ERRNO(EPERM);
			return -1;
		}
	}

	if (egid != (gid_t)-1) {
		if (u_area->egid == ROOT_UID || egid == u_area->regid
				|| egid == u_area->egid) {
			u_area->egid = egid;
		} else {
			SET_ERRNO(EPERM);
			return -1;
		}
	}

	return 0;
}

int setgid(gid_t gid) {
	struct task_u_area *u_area = task_self_u_area();

	if (u_area->egid == ROOT_UID) {
		u_area->egid = gid;
		u_area->regid = gid;
	} else if (u_area->regid == gid) {
		u_area->egid = gid;
	} else {
		SET_ERRNO(EPERM);
		return -1;
	}

	return 0;
}

int setegid(gid_t gid) {
	struct task_u_area *u_area = task_self_u_area();

	if (u_area->egid == ROOT_UID || u_area->regid == gid) {
	       u_area->egid = gid;
	} else {
		SET_ERRNO(EPERM);
		return -1;
	}

	return 0;
}
