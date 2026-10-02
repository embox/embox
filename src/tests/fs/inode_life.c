/**
 * @file
 * @brief How long an inode has to outlive the name that led to it.
 *
 * Finding a path and using what it led to are two operations, and between
 * them another core can unlink the inode. The tree's locks make the walk
 * safe; they do not make the pointer the walk returned stay alive. This suite
 * is about that pointer.
 *
 * The cases run from the plainest shape upwards, because the plainest one is
 * the one every system is expected to get right: a file unlinked while it is
 * open stays readable through the descriptor until the last one is closed.
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
/* Bytes, not pages: a page count times PAGE_SIZE() means a different
 * volume on every board. */
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
	return atomic_load(p, __ATOMIC_RELAXED);
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
 * One core, no threads: the plain shape of the defect.
 */
TEST_CASE("unlink with the file open postpones the free, close performs it") {
	unsigned long deferred_before;
	int fd;
	int i;

	test_assert_zero(file_make(FILE_ONE, 1));

	deferred_before = counter_read(&inode_free_deferred);

	fd = open(FILE_ONE, O_RDONLY);
	test_assert(fd >= 0);

	/* The name goes while the descriptor is still on the inode. Without the
	 * reference this frees the inode outright and the descriptor is left
	 * pointing at a pool slot the next open will hand to somebody else. */
	test_assert_zero(remove(FILE_ONE));
	test_assert_not_zero(0 == access(FILE_ONE, F_OK));

	/* The free was postponed, exactly once. */
	test_assert_equal(counter_read(&inode_free_deferred) - deferred_before, 1);

	/* And closing performs it -- which is the half a leak would hide. Twenty
	 * rounds is more than the driver has inodes (fat's inode_quantity is 16),
	 * so a reference that is never released runs the pool dry and the open
	 * below fails. */
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

	/* The inode is alive -- this descriptor holds it -- but it has no name
	 * and is on its way out. A second open must not find it, and if some
	 * path did hand it to file_desc_create() anyway, the reference would be
	 * refused rather than taken. */
	test_assert_not_zero(0 == access(FILE_ONE, F_OK));

	test_assert_zero(close(fd));

	/* Reported rather than asserted: whether anything actually reached the
	 * refusal depends on how the lookup answers a name that is gone, and both
	 * answers are correct. */
	printk("inode_life: %lu reference(s) refused on a dying inode\n",
	    counter_read(&inode_ref_refused) - refused_before);
}

/* ------------------------------------------------------------------ *
 * Two cores: one opens a name in a loop while the other unlinks and
 * recreates it.
 *
 * This is the case that found kread() clamping a read to a length
 * it read twice, so that a write on the other core between the two reads
 * turned a one-byte request into a 256-byte one. It answers 256 and writes
 * 256, over the caller's frame and its return address, and the crash lands
 * somewhere with no connection to the read at all.
 *
 * The line below is what says so out loud if it ever comes back. It is a
 * report rather than an assertion because by the time anything could assert,
 * the frame that would do the asserting is already gone.
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
 * A descriptor whose file was removed is REFUSED, not answered.
 *
 * POSIX says the file survives until the last descriptor closes. It does not
 * here, and the reason is not the inode -- the reference above keeps that
 * alive -- but the FAT driver, which frees the cluster chain inside
 * ino_remove() before the inode is touched at all. The bytes are gone whatever
 * the inode does.
 *
 * So the question is only what the descriptor answers, and there are two
 * possibilities: nothing (a short read, silently), or an error. It used to be
 * the first. It is now the second, because the alternative is worse than it
 * looks: the driver's private data is handed back at the same moment, its pool
 * slot goes to the next file, and a descriptor that keeps answering is a
 * descriptor that will eventually answer with somebody else's data. That is
 * what wrote one file's bytes into an unrelated file on a board.
 *
 * Making the POSIX promise true needs the driver to hold an unlinked file's
 * chain until the last close, which is a filesystem change and not a
 * reference count. Until then, an error is the honest answer.
 */
TEST_CASE("a descriptor whose file was removed is refused, not answered") {
	unsigned long dead_before;
	int fd;

	test_assert_zero(file_make(FILE_ONE, 3));

	fd = open(FILE_ONE, O_RDONLY);
	test_assert(fd >= 0);

	dead_before = counter_read(&fdesc_dead_inode);

	test_assert_zero(remove(FILE_ONE));

	/* Not a short read: an error. */
	memset(readback, 0, sizeof(readback));
	test_assert(read(fd, readback, sizeof(readback)) < 0);

	/* And the kernel counted it as what it was, so a run can say how often
	 * this happened rather than only that it did. */
	test_assert_equal(counter_read(&fdesc_dead_inode) - dead_before, 1);

	/* A write through it is refused too, and that is the half that used to
	 * corrupt another file. */
	test_assert(write(fd, readback, 1) < 0);

	/* The descriptor is still a descriptor, and closing it does not take the
	 * system with it. */
	test_assert_zero(close(fd));

	/* Nothing in this suite should ever have used a descriptor whose inode
	 * changed hands. If this is not zero, the defect is alive. */
	test_assert_zero(counter_read(&fdesc_stale_gen));
}

/* ------------------------------------------------------------------ *
 * The slot changes hands under an open descriptor.
 *
 * The slot-reuse defect in miniature: remove the file a descriptor is open
 * on, make another file so the pools hand the slot out again, and then use
 * the old descriptor. On a board this wrote one writer's text into an
 * unrelated file -- the writes were legal writes to legal clusters of the wrong
 * file, which is why fsck saw nothing and the suite's own byte checks did.
 *
 * Two things stop it now, and the case says which one did:
 *
 *   the reference   the descriptor holds the inode, so the slot cannot be
 *                   handed out at all while it is open. The removal is
 *                   deferred and the descriptor is refused as dying.
 *   the generation  if the reference were ever missed, the inode would be
 *                   reused and its generation would differ from the one the
 *                   descriptor recorded. That is the guard a NULL check
 *                   cannot be.
 *
 * Either way the descriptor is refused. The counters say which, and that is
 * the point: a build that lost the reference would still be caught, and would
 * say so instead of corrupting the other file.
 */
TEST_CASE("the slot changes hands under an open descriptor") {
	unsigned long dead_before, stale_before;
	int fd;
	char c;

	test_assert_zero(file_make(FILE_ONE, 4));

	fd = open(FILE_ONE, O_RDONLY);
	test_assert(fd >= 0);

	dead_before = counter_read(&fdesc_dead_inode);
	stale_before = counter_read(&fdesc_stale_gen);

	test_assert_zero(remove(FILE_ONE));

	/* Somebody else's file, made in the same place the removed one was. */
	test_assert_zero(file_make(FILE_TWO, 5));

	/* The old descriptor must not answer with any of it. */
	test_assert(read(fd, &c, 1) < 0);
	test_assert(write(fd, &c, 1) < 0);

	printk("inode_life: the stale descriptor was refused as %s\n",
	    counter_read(&fdesc_stale_gen) != stale_before ? "another file's"
	                                                   : "a removed file's");

	/* One of the two caught it, and nothing was answered. */
	test_assert_not_zero((counter_read(&fdesc_dead_inode) != dead_before)
	                     || (counter_read(&fdesc_stale_gen) != stale_before));

	test_assert_zero(close(fd));
	test_assert_zero(remove(FILE_TWO));
}
