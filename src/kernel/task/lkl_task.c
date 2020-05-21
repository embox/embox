#include <stdio.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-Wundef"
#include <lkl_host.h>
#include <lkl.h>
#pragma GCC diagnostic pop
#include <embox/unit.h>
#include <hal/test/traps_core.h>
#include <kernel/printk.h>
#include <assert.h>
#include <stdlib.h>


EMBOX_UNIT_INIT(lkl_task_init);

extern __trap_handler __exception_table[0x20];

static int handler(uint32_t nr, void * data) {
	pt_regs_t * st = (pt_regs_t*)data;
	long ret = -1;

	switch (st->eax) {
		case 4:
			ret = lkl_sys_write((unsigned int)st->ebx, (const char*)st->ecx, (lkl_size_t)st->edx);
			break;
		default:
			printk("Unknown syscall! It's code is %d\n", st->eax);
			break;
	}

	// return result of lkl syscall to program
	st->eax = ret;

	// go to next instruction as handler finished his work
	st->eip += 2;
	return 0;
}

static int lkl_task_init(void) {
	long ret = 0;
	char cmdline[64];

	snprintf(cmdline, sizeof(cmdline), "mem=64M loglevel=8"); // loglevel=8 is from tests
    ret = lkl_start_kernel(&lkl_host_ops, cmdline);

	intptr_t t = (intptr_t)malloc(4096 * 2);
	t += 0xFFF;
	t &= ~0xFFF;

	int fd = lkl_sys_open("/test", LKL_O_RDWR | LKL_O_CREAT, 0777);
	printk("%d\n", fd);

	for (int q = 0; q < 10; q++) {
		int err_code = lkl_sys_write(fd, "TESTME", 6);
		assert(err_code == 6);
	}
	

	char * addr = lkl_sys_mmap((void*)t, 4096, 0, 0x10 | 0x02, fd, 0);
	printk("%p\n", addr);

	for (int q = 0; q < 6; q++) {
		printk("%c", addr[q]);
	}
	printk("\n");


	// set lkl handlers for syscalls
	__exception_table[0xd] = handler;
	return ret;
}
