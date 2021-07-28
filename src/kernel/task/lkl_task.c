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

	case LKL_DEV_BLK_TYPE_WRITE: ;
		int i = 0;
		while (i < req->count) {
			printk("%.*s", req->buf[i].iov_len, req->buf[i].iov_base);
			memset(req->buf[i].iov_base, '\0', req->buf[i].iov_len);
			i++;
		}

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


/*
A structure that stores references to specific
implementations of block device operations
*/
static struct lkl_dev_blk_ops embox_lkl_dev_blk_ops = {
	.get_capacity = fd_get_capacity,
	.request = blk_request,
};

// We will get here when the Linux system call ('int 0x80') occurs
static int handler(uint32_t nr, void * data) {
	pt_regs_t * st = (pt_regs_t*)data;
	long ret = -1;

	switch (st->eax) {
		case 4:
			ret = lkl_sys_write((unsigned int)st->ebx, (const char*)st->ecx, (lkl_size_t)st->edx);
			break;

		case 5:
			ret = lkl_sys_open((const char*)st->ebx, (unsigned int*)st->ecx, (unsigned int*)st->edx);
		break;

		case 118:
			ret = lkl_sys_fsync((unsigned int)st->ebx);
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
	struct lkl_disk disk;
	unsigned int disk_id, dev;

	// We will use 'lkl_disk disk' as a "bridge" between LKL and Embox

	// Set disk operations
	disk.ops = &embox_lkl_dev_blk_ops;

	// Attach disk
	ret = lkl_disk_add(&disk);

	if (ret < 0) {
		printk("lkl_disk_add() failed: %s\n", lkl_strerror(ret));
		return -1;
	}
	disk_id = ret;

	// Start LKL
	ret = lkl_start_kernel(&lkl_host_ops, "mem=64M");
	if (ret < 0) {
		printk("Can't start lkl: %s\n", lkl_strerror(ret));
	}

	// Allocate 8 KB memory block
	intptr_t t = (intptr_t)malloc(4096 * 2);

	// Alignment
	t += 0xFFF;
	t &= ~0xFFF;

	// Create file '/test'
	int fd = lkl_sys_open("/test", LKL_O_RDWR | LKL_O_CREAT, 0777);
	printk("lkl_sys_open() returned: %d\n", fd);

	// Write "TESTME" 10 times to the file /test
	for (int q = 0; q < 10; q++) {
		int err_code = lkl_sys_write(fd, "TESTME", 6);
		// Check that write was successful
		assert(err_code == 6);
	}

	/*
	mmap() 4 KB of file '/test' into 8 KB of memmory pre-allocated by Embox earlier.
	We get the address of the mmap-ed data (we interpret it as a set of char-s).
	*/
	char * addr = lkl_sys_mmap((void*)t, 4096, 0, 0x10 | 0x02, fd, 0);
	// Check the location of the mmap-ed data
	printk("lkl_sys_mmap() returned: %p\n", addr);

	/* ToDo: everything fails in this block

	// Print the first 6 chars pointed by 'addr'
	for (int q = 0; q < 6; q++) {
		printk("%c", addr[q]);
	}
	printk("\n");
	*/

	// Get device id of a 'disk' pointed by 'disk_id' and write it to 'dev'
	ret = lkl_get_virtio_blkdev(disk_id, 0, &dev);

	if (ret < 0) {
		printk("lkl_get_virtio_blkdev() error: %s\n", lkl_strerror(ret));
	}

	// Create block device file '/vda' which points to 'dev'
	ret = lkl_sys_mknod("/vda", LKL_S_IFBLK | 0600, dev);

	if (ret < 0) {
		printk("lkl_sys_mknod() error: %s\n", lkl_strerror(ret));
	}

	// Try to get the file descriptor for '/vda'
	fd = lkl_sys_open("/vda", LKL_O_RDWR, 0);
	printk("/vda fd = %d\n", fd);

	if (fd < 0) {
		printk("Can't open the /vda file: %s\n", lkl_strerror(fd));
		return -1;
	}

	// Write "test" to /vda (the data will be pre-written to the cache buffer)
	ret = lkl_sys_write(fd, "test", 4);

	if (ret < 0) {
		printk("Can't write to /test: %s\n", lkl_strerror(ret));
		return -1;
	}

	// Transfer all data from cache buffer to the block device
	ret = lkl_sys_fsync(fd);

	if (ret < 0) {
		printk("lkl_sys_fsync() error: %s\n", lkl_strerror(ret));
	}

	// set lkl handlers for Linux syscalls
	__exception_table[0xd] = handler;

	return 0;
}
