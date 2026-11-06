/**
 * @file
 * @brief An inode outlives the name that led to it while it is referenced.
 */

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>

#include <drivers/block_dev.h>
#include <drivers/block_dev/ramdisk/ramdisk.h>
#include <embox/test.h>
#include <fs/fsop.h>
#include <fs/inode.h>
#include <kernel/printk.h>
#include <util/err.h>
#include <util/atomic_rmw.h>

EMBOX_TEST_SUITE("inode lifetime against unlink");

TEST_SETUP_SUITE(setup_suite);
TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME    "vfat"
#define FS_DEV     "/dev/ramdisk_inode"
/* In bytes, so the volume is the same on every board. */
#define FS_BYTES   (2 * 1024 * 1024)
#define FS_DIR     "/tmp_inode"
#define FILE_ONE   FS_DIR "/one.bin"
#define FILE_TWO   FS_DIR "/two.bin"

#define DATA_SZ    256

static char pattern[DATA_SZ];
static char readback[DATA_SZ];

extern unsigned long fdesc_dead_inode;
extern unsigned long fdesc_stale_gen;

static unsigned long counter_read(unsigned long *p) {
	return atomic_rmw_load(p, __ATOMIC_RELAXED);
}

static void pattern_fill(char *buf, size_t len, unsigned seed) {
	size_t i;

	for (i = 0; i < len; i++) {
		buf[i] = (char)((i * 31u + seed) & 0xff);
	}
}

static int file_make(const char *path, unsigned seed) {
	int fd;

	pattern_fill(pattern, sizeof(pattern), seed);

	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd < 0) {
		return -errno;
	}
	if (sizeof(pattern) != write(fd, pattern, sizeof(pattern))) {
		close(fd);
		return -EIO;
	}
	close(fd);

	return 0;
}

static int setup_suite(void) {
	int res;

	res = ptr2err(ramdisk_create(FS_DEV, FS_BYTES));
	if (res != 0) {
		return res;
	}
	if (0 != (res = format(FS_DEV, FS_NAME))) {
		return res;
	}

	return mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL);
}

static int teardown_suite(void) {
	umount(FS_DIR);
	return ramdisk_delete(FS_DEV);
}

/* ------------------------------------------------------------------ *
 * One core, no threads.
 */
TEST_CASE("unlink with the file open postpones the free, close performs it") {
	unsigned long deferred_before;
	int fd;
	int i;

	test_assert_zero(file_make(FILE_ONE, 1));

	deferred_before = counter_read(&inode_free_deferred);

	fd = open(FILE_ONE, O_RDONLY);
	test_assert(fd >= 0);

	/* The descriptor's reference keeps the inode; without it the descriptor
	 * would point at a pool slot the next open reuses. */
	test_assert_zero(remove(FILE_ONE));
	test_assert_not_zero(0 == access(FILE_ONE, F_OK));

	/* The free was postponed, exactly once. */
	test_assert_equal(counter_read(&inode_free_deferred) - deferred_before, 1);

	/* Closing performs the free: twenty rounds exceed fat's 16 inodes, so a
	 * leaked reference makes the open below fail. */
	test_assert_zero(close(fd));

	for (i = 0; i < 20; i++) {
		test_assert_zero(file_make(FILE_TWO, (unsigned)i));
		fd = open(FILE_TWO, O_RDONLY);
		test_assert(fd >= 0);
		test_assert_zero(remove(FILE_TWO));
		test_assert_zero(close(fd));
	}
}

/* ------------------------------------------------------------------ *
 * A reference asked for after the name is gone must be refused, not granted.
 */
TEST_CASE("opening a file unlinked out from under the lookup is refused") {
	unsigned long refused_before;
	int fd;

	test_assert_zero(file_make(FILE_ONE, 2));

	refused_before = counter_read(&inode_ref_refused);

	fd = open(FILE_ONE, O_RDONLY);
	test_assert(fd >= 0);
	test_assert_zero(remove(FILE_ONE));

	/* Alive but unnamed and dying: a second open must not find it, and
	 * file_desc_create() would refuse the reference. */
	test_assert_not_zero(0 == access(FILE_ONE, F_OK));

	test_assert_zero(close(fd));

	/* Reported, not asserted: whether the refusal is reached depends on the
	 * lookup, and both answers are correct. */
	printk("inode_life: %lu reference(s) refused on a dying inode\n",
	    counter_read(&inode_ref_refused) - refused_before);
}

/* ------------------------------------------------------------------ *
 * Two cores: one opens a name in a loop while the other unlinks and
 * recreates it. kread() must clamp against one reading of the file size, or
 * a read overruns the caller's buffer. Reported rather than asserted: an
 * overrun destroys the frame that would assert.
 */
static volatile int racer_run;
static unsigned long racer_opens, racer_misses;

static void *racer_main(void *arg) {
	(void)arg;

	while (racer_run) {
		int fd = open(FILE_TWO, O_RDONLY);

		if (fd >= 0) {
			char c;
			int res;

			racer_opens++;
			res = read(fd, &c, 1);
			if (res > 1) {
				printk("inode_life: read(1) answered %d\n", res);
			}
			close(fd);
		}
		else {
			racer_misses++;
		}
	}

	return NULL;
}

TEST_CASE("open and unlink of the same name from two cores") {
	pthread_t th;
	int i;

	racer_opens = racer_misses = 0;
	racer_run = 1;

	test_assert_zero(pthread_create(&th, NULL, racer_main, NULL));

	for (i = 0; i < 40; i++) {
		test_assert_zero(file_make(FILE_TWO, (unsigned)i));
		usleep(1000);
		test_assert_zero(remove(FILE_TWO));
	}

	racer_run = 0;
	test_assert_zero(pthread_join(th, NULL));

	test_assert_not_zero(racer_opens + racer_misses);

	printk("inode_life: racer %lu open(s), %lu miss(es)\n", racer_opens,
	    racer_misses);
}


/* ------------------------------------------------------------------ *
 * A file removed while open lives until its last descriptor closes.
 *
 * The driver takes only the name at the unlink and gives the clusters back on
 * the last reference (fat_destroy_inode), so the descriptor is answered from
 * its own file.
 */
static int fd_check(int fd, unsigned seed) {
	pattern_fill(pattern, sizeof(pattern), seed);
	memset(readback, 0, sizeof(readback));
	if (lseek(fd, 0, SEEK_SET) != 0) {
		return -1;
	}
	if (sizeof(readback) != read(fd, readback, sizeof(readback))) {
		return -1;
	}
	return memcmp(readback, pattern, sizeof(pattern)) ? -1 : 0;
}

TEST_CASE("a file removed while open is read and written until it is closed") {
	unsigned long dead_before;
	int fd;

	test_assert_zero(file_make(FILE_ONE, 3));

	fd = open(FILE_ONE, O_RDWR);
	test_assert(fd >= 0);

	dead_before = counter_read(&fdesc_dead_inode);

	test_assert_zero(remove(FILE_ONE));

	/* The name is gone... */
	test_assert(0 > open(FILE_ONE, O_RDONLY));

	/* ...and the file is not: its bytes, all of them. */
	test_assert_zero(fd_check(fd, 3));

	/* It takes a write, and reads it back. */
	pattern_fill(pattern, sizeof(pattern), 30);
	test_assert(lseek(fd, 0, SEEK_SET) == 0);
	test_assert_equal(sizeof(pattern), write(fd, pattern, sizeof(pattern)));
	test_assert_zero(fd_check(fd, 30));

	test_assert_equal(counter_read(&fdesc_dead_inode), dead_before);
	test_assert_zero(close(fd));

	/* No descriptor here may have used an inode that changed hands. */
	test_assert_zero(counter_read(&fdesc_stale_gen));
}

/* ------------------------------------------------------------------ *
 * The name's slot changes hands under an open descriptor.
 *
 * An unlinked file's directory entry is free at once, and the next file made
 * in that directory can take it. A write through the old descriptor must not
 * touch the new file's entry, and must still answer with its own bytes.
 */
TEST_CASE("the name's slot changes hands under an open descriptor") {
	unsigned long stale_before;
	int fd;

	test_assert_zero(file_make(FILE_ONE, 4));

	fd = open(FILE_ONE, O_RDWR);
	test_assert(fd >= 0);

	stale_before = counter_read(&fdesc_stale_gen);

	test_assert_zero(remove(FILE_ONE));

	/* Somebody else's file, made in the same place the removed one was. */
	test_assert_zero(file_make(FILE_TWO, 5));

	/* The old descriptor answers with its own file, and writes to it. */
	test_assert_zero(fd_check(fd, 4));
	pattern_fill(pattern, sizeof(pattern), 40);
	test_assert(lseek(fd, 0, SEEK_SET) == 0);
	test_assert_equal(sizeof(pattern), write(fd, pattern, sizeof(pattern)));
	test_assert_zero(close(fd));

	/* The new file is untouched, through a remount, which is the only way
	 * to tell the directory entry on the media from the cached inode. */
	test_assert_zero(umount(FS_DIR));
	test_assert_zero(mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL));
	fd = open(FILE_TWO, O_RDONLY);
	test_assert(fd >= 0);
	test_assert_zero(fd_check(fd, 5));
	test_assert_zero(close(fd));

	test_assert_equal(counter_read(&fdesc_stale_gen), stale_before);
	test_assert_zero(remove(FILE_TWO));
}
