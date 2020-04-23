#include <stdio.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-Wundef"
#include <lkl_host.h>
#include <lkl.h>
#pragma GCC diagnostic pop
#include <embox/unit.h>


EMBOX_UNIT_INIT(lkl_task_init);

extern __trap_handler __exception_table[0x20];

static int lkl_task_init(void) {
	printf("LKL TASK\n");
	long ret;
	char cmdline[64];

	snprintf(cmdline, sizeof(cmdline), "mem=64M loglevel=8"); // loglevel=8 is from tests
    ret = lkl_start_kernel(&lkl_host_ops, cmdline);

	// set lkl handlers for syscalls
	__exception_table[exception_nr] = handler;
	return ret;
}
