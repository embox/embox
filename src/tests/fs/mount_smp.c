/**
 * @file
 * @brief mount() and umount() while other cores are walking the tree.
 */

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>

#include <drivers/block_dev.h>
#include <drivers/block_dev/ramdisk/ramdisk.h>
#include <embox/test.h>
#include <fs/fsop.h>
#include <kernel/printk.h>
#include <util/err.h>
#include <util/atomic_rmw.h>

/* Not run at boot: the suite is hostile and fails on some configurations,
 * and a failing suite aborts the runlevel. Run it by name: test -t mount_smp */
EMBOX_TEST_SUITE_NOAUTO("mount/umount against a tree in use");

TEST_SETUP_SUITE(setup_suite);
TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME    "vfat"
#define FS_DEV     "/dev/ramdisk_mnt"
/* Bytes, not pages, so the volume is the same size on every board. */
#define FS_BYTES    (2 * 1024 * 1024)
#define FS_DIR     "/tmp_mnt"
#define FS_FILE    FS_DIR "/probe.bin"


#define WORKERS    3
/* Enough rounds for the mount loop to overlap many worker passes. */
#define ROUNDS     24
#define PROBE_SZ   64

/* Read by the workers, written by the suite thread. */
static volatile int workers_run;

/* Set by the suite thread: the workers stop opening files for one turn
 * (see the umount loop). */
static volatile int workers_pause;

/* EBUSY answers the unmounter takes before it asks the workers for a turn,
 * and tries before it gives up. UMOUNT_FAIR must stay large: the unarranged
 * race before it is where the collisions are made. */
#define UMOUNT_FAIR  200
#define UMOUNT_TRIES 10000

static struct {
	unsigned long walks;  /* lookups attempted */
	unsigned long found;  /* ... that saw the volume mounted */
	unsigned long opened; /* descriptors actually obtained */
	unsigned long wrong;  /* ... that were not the file that was asked for */
	int cpu_mask;         /* which cores this worker ran on */
} worker[WORKERS];

extern unsigned long mount_table_contended __attribute__((weak));
extern unsigned long vfs_tree_contended __attribute__((weak));
extern unsigned long fdesc_live_lost __attribute__((weak));
extern unsigned long vfs_walk_dying __attribute__((weak));

static unsigned long counter_read(unsigned long *p) {
	return p ? atomic_rmw_load(p, __ATOMIC_RELAXED) : 0;
}

static void *worker_main(void *arg) {
	int idx = (int)(intptr_t)arg;
	char buf[PROBE_SZ];

	while (workers_run) {
		struct stat st;
		int fd;

		if (workers_pause) {
			/* Nothing is held here, so pausing gives the unmounter a
			 * window it cannot otherwise get. */
			usleep(2000);
			continue;
		}

		worker[idx].walks++;

		/* A bare resolution and one ending in a descriptor; the descriptor
		 * makes umount answer EBUSY, so it is held only briefly. */
		if (0 == stat(FS_DIR, &st)) {
			worker[idx].found++;
		}

		fd = open(FS_FILE, O_RDONLY);
		if (fd >= 0) {
			struct stat fst;

			worker[idx].opened++;

			/* These threads share the shell's fd table, and the shell
			 * takes fds 0-2 back for its stdio while a worker may hold one,
			 * so check the descriptor is still the file. Counted, not
			 * asserted: it is not a filesystem defect. */
			if ((0 != fstat(fd, &fst)) || !S_ISREG(fst.st_mode)) {
				/* Print now: by the end the descriptor is closed. */
				if (worker[idx].wrong == 0) {
					printk("mount_smp: worker %d: fd %d is no longer the file "
					       "it opened -- fstat says mode %o size %ld\n",
					    idx, fd, (unsigned)fst.st_mode, (long)fst.st_size);
				}
				worker[idx].wrong++;
			}
			else {
				(void)read(fd, buf, sizeof(buf));
			}
			close(fd);
		}

		/* A path that never exists, so lookup misses run against the same
		 * rewrites. */
		(void)stat(FS_DIR "/nothing-here", &st);
	}

	return NULL;
}

static int probe_write(void) {
	char buf[PROBE_SZ];
	int fd;
	size_t i;

	for (i = 0; i < sizeof(buf); i++) {
		buf[i] = (char)(i & 0xff);
	}

	fd = open(FS_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd < 0) {
		return -errno;
	}
	if (sizeof(buf) != write(fd, buf, sizeof(buf))) {
		close(fd);
		return -EIO;
	}
	close(fd);

	return 0;
}

static int probe_check(void) {
	char buf[PROBE_SZ];
	int fd;
	size_t i;

	fd = open(FS_FILE, O_RDONLY);
	if (fd < 0) {
		return -errno;
	}
	if (sizeof(buf) != read(fd, buf, sizeof(buf))) {
		close(fd);
		return -EIO;
	}
	close(fd);

	for (i = 0; i < sizeof(buf); i++) {
		if (buf[i] != (char)(i & 0xff)) {
			return -EILSEQ;
		}
	}

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
	/* Lay down the probe file, then leave the volume unmounted for the
	 * case. */
	if (0 != (res = mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL))) {
		return res;
	}
	if (0 != (res = probe_write())) {
		umount(FS_DIR);
		return res;
	}
	if (0 != (res = umount(FS_DIR))) {
		return res;
	}

	memset((void *)worker, 0, sizeof(worker));
	workers_run = 0;

	return 0;
}

static int teardown_suite(void) {
	struct stat st;

	workers_run = 0;

	/* Only if the case left it mounted; do not rely on umount() refusing
	 * a directory that is not a mount point. */
	if (0 == stat(FS_FILE, &st)) {
		umount(FS_DIR);
	}

	return ramdisk_delete(FS_DEV);
}

TEST_CASE("umount of a directory that is not a mount point is refused") {
	struct stat st;

	/* An ordinary directory of the root filesystem, not a mount point. */
	test_assert_not_zero(umount(FS_DIR));
	test_assert_equal(errno, EINVAL);

	/* ... and the root is still mounted. */
	test_assert_zero(stat("/", &st));
	test_assert_zero(stat(FS_DIR, &st));
}

TEST_CASE("a volume mounted and unmounted under a tree being walked") {
	pthread_t th[WORKERS];
	unsigned long mt_before, vt_before, wd_before;
	unsigned long walks = 0, opened = 0, wrong = 0;
	int retries_total = 0;
	int i, round;

	mt_before = counter_read(&mount_table_contended);
	vt_before = counter_read(&vfs_tree_contended);
	wd_before = counter_read(&vfs_walk_dying);

	/* Workers are not pinned: they must be free to meet the mount loop on
	 * other cores. */
	workers_run = 1;
	for (i = 0; i < WORKERS; i++) {
		test_assert_zero(
		    pthread_create(&th[i], NULL, worker_main, (void *)(intptr_t)i));
	}

	for (round = 0; round < ROUNDS; round++) {
		int retries = 0;
		int res;

		test_assert_zero(mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL));

		/* Mounted: the file reads back. Only this thread knows the volume
		 * is up, so only it asserts that. */
		test_assert_zero(probe_check());

		/* EBUSY while a worker holds a descriptor is correct: wait it out
		 * rather than quiescing the workers, which keeps the sides apart. */
		while (0 != (res = umount(FS_DIR))) {
			if (++retries > UMOUNT_TRIES) {
				break;
			}
			/* With fewer cores than workers there may never be an instant
			 * with every descriptor closed, so after UMOUNT_FAIR tries ask
			 * the workers for a turn. */
			if (retries == UMOUNT_FAIR) {
				workers_pause = 1;
			}
			usleep(1000);
		}
		workers_pause = 0;
		retries_total += retries;
		test_assert_zero(res);

		/* Unmounted: the path under the mount point is gone. */
		test_assert_not_zero(open(FS_FILE, O_RDONLY) >= 0 ? 0 : 1);

		/* A woken thread does not preempt a running one of its own
		 * priority, and nothing here blocks when umount succeeds at once. */
		sched_yield();
	}

	workers_run = 0;
	for (i = 0; i < WORKERS; i++) {
		test_assert_zero(pthread_join(th[i], NULL));
	}

	for (i = 0; i < WORKERS; i++) {
		walks += worker[i].walks;
		opened += worker[i].opened;
		wrong += worker[i].wrong;
	}

	/* The workers must have run throughout, or the run proves nothing. */
	test_assert_not_zero(walks);

	/* Reported, not asserted: with every thread on one core
	 * (default_mask=1) the workers never find the volume mounted. */
	if (opened == 0) {
		printk("mount_smp: no worker opened anything -- nothing overlapped, "
		       "so this run says nothing about concurrency\n");
	}

	printk("mount_smp: %d mount/umount round(s), %lu walk(s), %lu open(s), "
	       "%d umount retr(ies)\n",
	    ROUNDS, walks, opened, retries_total);
	if (wrong != 0) {
		printk("mount_smp: %lu descriptor(s) were taken back by the shell "
		       "while a worker held them (see worker_main)\n",
		    wrong);
	}
	printk("mount_smp: %lu walk(s) reached an inode already out of the tree\n",
	    counter_read(&vfs_walk_dying) - wd_before);
	printk("mount_smp: mount tree contended %lu time(s), name tree %lu, "
	       "%lu descriptor(s) lost\n",
	    counter_read(&mount_table_contended) - mt_before,
	    counter_read(&vfs_tree_contended) - vt_before,
	    counter_read(&fdesc_live_lost));

}
