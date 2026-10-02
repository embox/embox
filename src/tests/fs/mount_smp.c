/**
 * @file
 * @brief mount() and umount() while other cores are walking the tree.
 *
 * The mount tree is rewritten by mount() and umount() and read by every path
 * resolution in the system. It has a lock now, and the counter next to that
 * lock said 0 on every run we had: the FAT hammer that produced the rest of
 * the evidence stops all of its workers before each unmount, by design, so
 * the two sides never met. A lock nobody has been seen to need is a lock
 * nobody has tested.
 *
 * This suite is that meeting. Worker threads walk paths under a mount point
 * without pause -- deliberately unpinned, so the scheduler is free to put
 * them on other cores -- while the suite's own thread mounts and unmounts the
 * volume underneath them.
 *
 * What it asserts:
 *
 *   - every mount and every umount completes (umount may answer EBUSY while a
 *     worker holds a descriptor, which is correct and is retried);
 *   - a file written before the run is readable after every mount, and its
 *     path is gone after every umount -- checked from the suite's own thread,
 *     which is the only one that knows which state the volume is in;
 *   - the workers keep running throughout and none of them sees a torn tree:
 *     a lookup answers either "found" or an error, never a bad pointer.
 *
 * What it reports rather than asserts: how many times the two sides actually
 * collided inside the mount tree's lock. That number is why the suite exists,
 * and asserting on it would make the suite fail on a machine too slow or too
 * small to produce a collision.
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
#include <kernel/printk.h>
#include <util/err.h>
#include <util/atomic_rmw.h>

/* Not run at boot: this suite is deliberately hostile and on some
 * configurations it fails, and a failing suite aborts the runlevel -- leaving
 * it on autorun would cost every other suite and the shell. Run it by name:
 *
 *   test -t mount_smp
 *
 * What the runs say, all of them 24 rounds against three workers:
 *
 *   aarch64, four cores  the two sides meet: hundreds of collisions inside
 *                        the mount table's lock and thousands inside the name
 *                        tree's, and no descriptor lost. It is also the
 *                        configuration that finds the open defect below.
 *   x86, two cpus        the unmount side is starved: EBUSY on all ten
 *                        thousand tries while the three workers opened and
 *                        closed the file about thirty-five thousand times
 *                        each -- three and a half opens per try. Where there
 *                        are fewer cores than workers there is no instant
 *                        with every descriptor closed to be found, which is
 *                        why the unmount loop asks for one after UMOUNT_FAIR
 *                        tries rather than waiting forever.
 *   riscv, two harts     runs, once the riscv timer is fixed; before that the
 *                        second hart destroyed the time base and the first
 *                        pthread_create() never returned.
 *
 * And the reason worker_main checks what open() gave it, which cost a day to
 * find and is worth writing down: these threads run in the SHELL'S TASK, so
 * they share the shell's descriptor table. open() answers with the lowest
 * free number, and in a boot that has reached tish that means 0, 1 and 2 --
 * the shell's own stdio, which it takes back whenever it starts a command.
 * On four cores it does that while a worker is holding one, and the worker's
 * next read() goes to the serial port and waits for a key that never comes.
 * One worker parked there hangs the whole suite at pthread_join().
 *
 * It is not a filesystem defect, so it is counted and reported rather than
 * asserted on. The lesson generalises: in Embox an fd table belongs to a
 * task, and a threaded test running under the shell must not trust a
 * descriptor number to still be its own.
 */
EMBOX_TEST_SUITE_NOAUTO("mount/umount against a tree in use");

TEST_SETUP_SUITE(setup_suite);
TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME    "vfat"
#define FS_DEV     "/dev/ramdisk_mnt"
/* Bytes, not pages: a page count times PAGE_SIZE() means a different
 * volume on every board. */
#define FS_BYTES    (2 * 1024 * 1024)
#define FS_DIR     "/tmp_mnt"
#define FS_FILE    FS_DIR "/probe.bin"


#define WORKERS    3
/* Enough rounds that the mount loop overlaps many worker passes, few enough
 * that the suite stays a test rather than a soak. */
#define ROUNDS     24
#define PROBE_SZ   64

/* Read by the workers, written by the suite thread. */
static volatile int workers_run;

/* Also read by the workers, also written by the suite thread: "stop opening
 * things for one turn". See the umount loop for why it has to exist. */
static volatile int workers_pause;

/* How many honest EBUSY answers the unmounting thread takes before it asks
 * for that turn, and how many tries it gives up after.
 *
 * The first number is the whole point of the suite and must stay large: for
 * UMOUNT_FAIR milliseconds the two sides race with nothing arranged, and that
 * is when the collisions this suite counts are made. The second is only a
 * bound on a test that would otherwise hang. */
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
	return p ? atomic_load(p, __ATOMIC_RELAXED) : 0;
}

static void *worker_main(void *arg) {
	int idx = (int)(intptr_t)arg;
	char buf[PROBE_SZ];

	while (workers_run) {
		struct stat st;
		int fd;

		if (workers_pause) {
			/* Nothing is held at this point in the loop, so honouring the
			 * request costs one sleep and gives the unmounting thread a
			 * window it cannot otherwise get. */
			usleep(2000);
			continue;
		}

		worker[idx].walks++;

		/* Both shapes of use: a bare resolution, and one that ends in a
		 * descriptor. The second is what makes umount answer EBUSY, so it is
		 * kept short on purpose. */
		if (0 == stat(FS_DIR, &st)) {
			worker[idx].found++;
		}

		fd = open(FS_FILE, O_RDONLY);
		if (fd >= 0) {
			struct stat fst;

			worker[idx].opened++;

			/* What came back has to be the regular file that was asked
			 * for, and that is checked rather than assumed.
			 *
			 * These workers run in the shell's task, so they share the
			 * shell's descriptor table, and open() answers with the lowest
			 * free number -- which in a boot that has reached tish means 0,
			 * 1 and 2. Those are the shell's, and it takes them back for its
			 * own stdio whenever it starts a command. On four cores that
			 * happens while a worker is holding one:
			 *
			 *   SLOT 0 <- del 0x...6570 (a file)  th 1   <- the shell,
			 *   SLOT 0 <- add 0x...7300 (console) th 1      closing a
			 *   SLOT 1 <- lock 0x...7300          th 1      worker's fd
			 *
			 * and the worker's next read() on the number it still believes
			 * is its file goes to the serial port and waits there for a key
			 * that never comes. One worker parked in the console hangs the
			 * whole suite at pthread_join(), which is a report of nothing.
			 *
			 * So the descriptor is checked, counted and let go. This is not
			 * a filesystem defect and it is not asserted on: an fd table
			 * belongs to a task, this suite is a guest in the shell's, and
			 * the only thing it can do about that is not to trust a number
			 * it did not keep locked. */
			if ((0 != fstat(fd, &fst)) || !S_ISREG(fst.st_mode)) {
				/* Print at the point of the observation, not at the end:
				 * what this descriptor is is the whole question, and by the
				 * end it has been closed. */
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

		/* Also walk a path that never exists, so the miss side of the lookup
		 * runs against the same rewrites. */
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
	/* Mount once to lay down the file the workers will be looking for, then
	 * leave it unmounted: the case drives mount and umount itself. */
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

	/* Only if the case left it mounted. umount() of a directory that is not a
	 * mount point is refused now (see kumount.c), but this suite is also the
	 * thing that found that out, so it does not lean on it. */
	if (0 == stat(FS_FILE, &st)) {
		umount(FS_DIR);
	}

	return ramdisk_delete(FS_DEV);
}

TEST_CASE("umount of a directory that is not a mount point is refused") {
	struct stat st;

	/* An ordinary directory of the root filesystem. Unmounting it used to
	 * unmount the root: umount_walker() deleted every inode, and the next
	 * path resolution died on a NULL mount descriptor. */
	test_assert_not_zero(umount(FS_DIR));
	test_assert_equal(errno, EINVAL);

	/* ... and the root is still there, which is the half that matters. */
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

	/* Does NOT apply here: this case is about two cores meeting inside the
	 * mount tree, so the workers must be free to go wherever the scheduler
	 * puts them. Nothing is pinned on purpose. */
	workers_run = 1;
	for (i = 0; i < WORKERS; i++) {
		test_assert_zero(
		    pthread_create(&th[i], NULL, worker_main, (void *)(intptr_t)i));
	}

	for (round = 0; round < ROUNDS; round++) {
		int retries = 0;
		int res;

		test_assert_zero(mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL));

		/* Mounted: the file is there and reads back correctly. Only this
		 * thread knows the volume is up, so only this thread may insist. */
		test_assert_zero(probe_check());

		/* A worker holding a descriptor makes umount answer EBUSY. That is
		 * the right answer -- wait for the descriptor to go rather than
		 * quiescing the workers, because quiescing them is exactly what kept
		 * the two sides from ever meeting. */
		while (0 != (res = umount(FS_DIR))) {
			if (++retries > UMOUNT_TRIES) {
				break;
			}
			/* EBUSY while a worker holds a descriptor is the right answer,
			 * and for the first UMOUNT_FAIR tries waiting it out is the
			 * honest race this suite is for.
			 *
			 * It is not a race the unmounter can always win, though, and
			 * that is measured rather than assumed: on x86 with two CPUs it
			 * answered EBUSY on all ten thousand tries while the three
			 * workers opened and closed the file about thirty-five thousand
			 * times each -- three and a half opens per try. Where there are
			 * fewer cores than workers there is no instant with every
			 * descriptor closed for the unmounter to find, and no retry
			 * budget fixes that. So after the honest part it asks for a turn
			 * instead of waiting for one. The collisions are already made by
			 * then; the counters below say how many. */
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

	/* The workers must have been running throughout, or the run proves
	 * nothing at all. */
	test_assert_not_zero(walks);

	/* Whether any of them got a descriptor is reported, not asserted. With
	 * the boot-core fence up (default_mask=1) every thread is on one core,
	 * and the mount loop never yields between mount and umount -- so the
	 * workers walk, find nothing mounted, and open nothing. That is the
	 * fence doing its job, and it is also why a run with zero opens proves
	 * nothing about the overlap this suite exists to produce. */
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
