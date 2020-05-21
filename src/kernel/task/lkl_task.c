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

#define DISK_CAPACITY 4096


EMBOX_UNIT_INIT(lkl_task_init);

extern __trap_handler __exception_table[0x20];

static int fd_get_capacity(struct lkl_disk disk, unsigned long long *res) {
	*res = DISK_CAPACITY;
	return 0;
}

static int blk_request(struct lkl_disk disk, struct lkl_blk_req *req) {
	int err = 0;

	switch (req->type) {
	case LKL_DEV_BLK_TYPE_READ:
		break;

	case LKL_DEV_BLK_TYPE_WRITE:
		break;

	case LKL_DEV_BLK_TYPE_FLUSH:

	case LKL_DEV_BLK_TYPE_FLUSH_OUT:
		break;

	default:
		return LKL_DEV_BLK_STATUS_UNSUP;
	}

	if (err < 0) {
		return LKL_DEV_BLK_STATUS_IOERR;
	}

	return LKL_DEV_BLK_STATUS_OK;
}


static struct lkl_dev_blk_ops embox_lkl_dev_blk_ops = {
	.get_capacity = fd_get_capacity,
	.request = blk_request,
};

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
	int fd;
	struct lkl_disk disk;
	unsigned int disk_id, dev;

	disk.ops = &embox_lkl_dev_blk_ops;

	ret = lkl_disk_add(&disk);
	if (ret < 0) {
		printk("Can't add disk: %s\n", lkl_strerror(ret));
		return -1;
	}
	disk_id = ret;

	snprintf(cmdline, sizeof(cmdline), "mem=64M loglevel=8"); // loglevel=8 is from tests
  ret = lkl_start_kernel(&lkl_host_ops, cmdline);
	if (ret < 0) {
		printk("Can't start lkl: %s\n", lkl_strerror(ret));
	}

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

	ret = lkl_get_virtio_blkdev(disk_id, 0, &dev);

	if (ret < 0) {
		printk("lkl_get_virtio_blkdev error: %s\n", lkl_strerror(ret));
	}

	ret = lkl_sys_mknod("/vda", LKL_S_IFBLK | 0600, dev);

	if (ret < 0) {
		printk("lkl_sys_mknod error: %s\n", lkl_strerror(ret));
	}

	fd = lkl_sys_open("/vda", LKL_O_RDWR, 0);

	if (fd < 0) {
		printk("Can't open the file: %s\n", lkl_strerror(fd));
		return -1;
	}

	ret = lkl_sys_write(fd, "test", 4);

	if (ret < 0) {
		printk("can't write: %s\n", lkl_strerror(ret));
		return -1;
	}

	ret = lkl_sys_fsync(fd);

	if (ret < 0) {
		printk("fsync error: %s\n", lkl_strerror(ret));
	}

	// set lkl handlers for syscalls
	__exception_table[0xd] = handler;
	return 0;
}
