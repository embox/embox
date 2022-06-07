/*
 *  This app can be used to test/debug LKL work in Embox.
 *
 *  Initially, it writes input string paramteter to LKL's '/vda' file,
 *    which should redirect text to Embox terminal.
 *  Also, 'lkl_sys_gettid' is called during app execution.
 *    TID should be the same during the entire execution.
 *
 *  The app uses both INT0x80 and direct 'lkl_sys_{syscall}' calls.
 */

#include <stdio.h>
#include <lkl_host.h>
#include <lkl.h>
#include <kernel/task.h>
#include <kernel/task/resource/lkl_resources.h>

static inline long os_syscall(int syscall,
							  unsigned long arg1, unsigned long arg2,
							  unsigned long arg3, unsigned long arg4) {
	long ret;
	__asm__ __volatile__(
	"int $0x80\n"
	: "=a"(ret)
	: "a"(syscall), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4)
	:
	);
	return ret;
}

int main(int argc, char **argv) {
	// Normally, this is done by 'load_app'
	task_lkl_resources(task_self())->lkl_allowed = 1;

	printf("Started 'echotovda' execution.\n");
	printf("lkl_sys_gettid() = %d.\n", lkl_sys_gettid());

	/*
	 * Try to get FD for special file /vda.
	 * This file should have been created in src/kernel/task/lkl_task.c.
	 */
	long fd = os_syscall(5, "/vda", LKL_O_RDWR, 0, 0);

	if (fd < 0) {
		printf("Can't open /vda\n");
		return 1;
	}
	printf("lkl_sys_open() for /vda returned %d.\n", fd);
	printf("lkl_sys_gettid() = %d.\n", lkl_sys_gettid());

	// Count input length
	int len = 0;
	while (argv[1][len] != '\0') {
		len++;
	}

	// Write to /vda
	long ret_write = os_syscall(4, fd, &argv[1][0], len, 0);

	if (ret_write < 0) {
		printf("Can't write to /vda");
		return 1;
	}

	printf("lkl_sys_write() for /vda returned %d.\n", ret_write);
	printf("lkl_sys_gettid() = %d.\n", lkl_sys_gettid());

	/*
	 * fsync() LKL's /vda device.
	 * /vda driver in src/kernel/task/lkl_task.c will be triggered immediately.
	 */
	long ret_fsync = os_syscall(118, fd, 0, 0, 0);

	if (ret_fsync < 0) {
		printf("Can't fsync LKL's /vda");
		return 1;
	}

	printf("lkl_sys_fsync() for /vda returned %d.\n", ret_fsync);
	printf("lkl_sys_gettid() = %d.\n", lkl_sys_gettid());
}
