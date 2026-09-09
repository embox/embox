/**
 * @file
 * @brief What a FAT driver has to get right, on a ramdisk instead of a card.
 *
 * Every case here is a defect that was found on real hardware, in a board
 * suite that needs an SD controller, a provisioned card and a host to run
 * fsck afterwards -- so none of it could ever run in CI. The behaviour it was
 * testing is the driver's, not the card's, and that part travels: a ramdisk
 * formatted vfat exercises the same code.
 *
 * What each case is, and what it caught:
 *
 *   read sizes          reads longer than one sector returned short or wrong
 *   read after write    a read in the same session as the write that preceded
 *                       it saw stale bytes -- the driver's cursor and its
 *                       one-sector buffer disagreeing
 *   rewrite             O_TRUNC to the same size, smaller, and larger; the
 *                       smaller case leaves a chain to shorten and the larger
 *                       one a chain to grow
 *   many files          sixteen files in a row, then read back: this is what
 *                       showed the allocator handing out a cluster twice
 *   interleaved         two descriptors written in turn, which is the shape
 *                       an update has when it logs while it downloads
 *   remount             everything written survives umount and mount, which
 *                       is the only way to tell a correct file from a correct
 *                       cache
 *   unlink              deleting a multi-cluster file used to never return,
 *                       and the space it freed used to not come back
 *
 * The pattern written is regenerable and compared byte for byte, because a
 * size and a return code are not evidence: the defect that started all of this
 * reported success for a write that stored 5.4 MiB of the 8 requested.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>

#include <errno.h>

#include <drivers/block_dev.h>
#include <fs/fat_pool_use.h>
#include <drivers/block_dev/ramdisk/ramdisk.h>
#include <embox/test.h>
#include <fs/fsop.h>
#include <kernel/printk.h>
#include <util/err.h>

EMBOX_TEST_SUITE("FAT driver operations");

TEST_SETUP_SUITE(setup_suite);
TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME   "vfat"
#define FS_DEV    "/dev/ramdisk_fat"
/* Big enough that a file spans many clusters and that sixteen of them fit:
 * every defect below needed more than one cluster to show. */
/* Bytes, not pages: a page count times PAGE_SIZE() means a different
 * volume on every board. */
#define FS_BYTES   (8 * 1024 * 1024)
#define FS_DIR    "/tmp_fat"

#define FILE_A    FS_DIR "/a.bin"
#define FILE_B    FS_DIR "/b.bin"

/* One file's worth of bytes, and the buffer everything is read back into.
 * Static: a test suite runs in kernel context and a stack is not the place. */
#define DATA_SZ   (32 * 1024)
static char pattern[DATA_SZ];
static char readback[DATA_SZ];

/* A regenerable pattern: byte i of the file is a function of i alone, so a
 * mismatch names the offset it happened at rather than "the file differs". */
static void pattern_fill(char *buf, size_t len, unsigned seed) {
	size_t i;

	for (i = 0; i < len; i++) {
		buf[i] = (char)((i * 31u + (i >> 8) * 7u + seed) & 0xff);
	}
}

static int pattern_first_diff(const char *got, const char *want, size_t len) {
	size_t i;

	for (i = 0; i < len; i++) {
		if (got[i] != want[i]) {
			return (int)i;
		}
	}
	return -1;
}

/* Write `len` bytes of the pattern into `path`, one write() call. */
static int write_file(const char *path, size_t len, unsigned seed) {
	int fd;
	ssize_t n;

	pattern_fill(pattern, len, seed);

	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd < 0) {
		/* Said out loud: which call failed and why is the whole difference
		 * between a defect and a volume that is simply full. */
		printk("fat_ops: open(%s) failed, errno %d\n", path, errno);
		return -1;
	}
	n = write(fd, pattern, len);
	if (n != (ssize_t)len) {
		printk("fat_ops: write(%s, %u) wrote %d, errno %d\n", path,
		    (unsigned)len, (int)n, errno);
	}
	close(fd);

	return (n == (ssize_t)len) ? 0 : -1;
}

/* Read `path` back in `chunk`-sized pieces and compare. Returns the offset of
 * the first wrong byte, -1 if it is all correct, or -2 if the file ended
 * early -- which is a different failure and used to be the common one. */
static int check_file(const char *path, size_t len, size_t chunk) {
	size_t got = 0;
	int fd;

	memset(readback, 0, len);

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		return -2;
	}
	while (got < len) {
		size_t want = (len - got < chunk) ? (len - got) : chunk;
		ssize_t n = read(fd, readback + got, want);

		if (n <= 0) {
			break;
		}
		got += (size_t)n;
	}
	close(fd);

	if (got != len) {
		return -2;
	}
	return pattern_first_diff(readback, pattern, len);
}

TEST_CASE("a read longer than one sector returns the whole thing") {
	test_assert_zero(write_file(FILE_A, DATA_SZ, 1));

	/* 512 is one sector, and the three above it are what the driver's own
	 * buffer does not cover in one go. */
	test_assert_equal(-1, check_file(FILE_A, DATA_SZ, 512));
	test_assert_equal(-1, check_file(FILE_A, DATA_SZ, 4096));
	test_assert_equal(-1, check_file(FILE_A, DATA_SZ, 16384));
	test_assert_equal(-1, check_file(FILE_A, DATA_SZ, DATA_SZ));
}

TEST_CASE("a read in the same session as the write before it is not stale") {
	int fd;
	ssize_t n;

	pattern_fill(pattern, DATA_SZ, 2);

	test_assert_true(0 <= (fd = open(FILE_A, O_RDWR | O_CREAT | O_TRUNC, 0666)));
	n = write(fd, pattern, DATA_SZ);
	test_assert_equal((ssize_t)DATA_SZ, n);

	/* No close in between: this is the case the board suite calls "read in
	 * the same session", and it is the one that was silently wrong. */
	test_assert_equal(0, lseek(fd, 0, SEEK_SET));
	memset(readback, 0, DATA_SZ);
	n = read(fd, readback, DATA_SZ);
	close(fd);

	test_assert_equal((ssize_t)DATA_SZ, n);
	test_assert_equal(-1, pattern_first_diff(readback, pattern, DATA_SZ));
}

TEST_CASE("lseek lands where it says") {
	int fd;
	char byte;

	test_assert_zero(write_file(FILE_A, DATA_SZ, 3));
	pattern_fill(pattern, DATA_SZ, 3);

	test_assert_true(0 <= (fd = open(FILE_A, O_RDONLY)));

	test_assert_equal(DATA_SZ - 1, lseek(fd, DATA_SZ - 1, SEEK_SET));
	test_assert_equal(1, read(fd, &byte, 1));
	test_assert_equal(pattern[DATA_SZ - 1], byte);

	/* Backwards across a cluster boundary, which is where a cursor that is
	 * only ever advanced gets it wrong. */
	test_assert_equal(1024, lseek(fd, 1024, SEEK_SET));
	test_assert_equal(1, read(fd, &byte, 1));
	test_assert_equal(pattern[1024], byte);

	close(fd);
}

TEST_CASE("O_TRUNC rewrites at the same size, smaller and larger") {
	struct stat st;

	test_assert_zero(write_file(FILE_A, DATA_SZ, 4));
	test_assert_equal(-1, check_file(FILE_A, DATA_SZ, 4096));

	/* Same length: the chain is reused as it is. */
	test_assert_zero(write_file(FILE_A, DATA_SZ, 5));
	test_assert_equal(-1, check_file(FILE_A, DATA_SZ, 4096));

	/* Shorter: the tail of the chain has to be given back. */
	test_assert_zero(write_file(FILE_A, DATA_SZ / 4, 6));
	test_assert_equal(-1, check_file(FILE_A, DATA_SZ / 4, 4096));
	test_assert_zero(stat(FILE_A, &st));
	test_assert_equal(DATA_SZ / 4, (int)st.st_size);

	/* Longer than it has ever been: the chain has to grow. */
	test_assert_zero(write_file(FILE_A, DATA_SZ, 7));
	test_assert_equal(-1, check_file(FILE_A, DATA_SZ, 4096));
	test_assert_zero(stat(FILE_A, &st));
	test_assert_equal(DATA_SZ, (int)st.st_size);
}

TEST_CASE("two files written in turn do not take each other's clusters") {
	int fa, fb;
	size_t off;
	const size_t step = 4096;
	const size_t len = 16 * 1024;

	test_assert_true(0 <= (fa = open(FILE_A, O_WRONLY | O_CREAT | O_TRUNC, 0666)));
	test_assert_true(0 <= (fb = open(FILE_B, O_WRONLY | O_CREAT | O_TRUNC, 0666)));

	for (off = 0; off < len; off += step) {
		pattern_fill(pattern, step, 8);
		test_assert_equal((ssize_t)step, write(fa, pattern, step));
		pattern_fill(pattern, step, 9);
		test_assert_equal((ssize_t)step, write(fb, pattern, step));
	}
	close(fa);
	close(fb);

	/* Rebuild each file's pattern the way it was written -- the same block
	 * repeated -- and compare the whole thing. */
	for (off = 0; off < len; off += step) {
		pattern_fill(pattern + off, step, 8);
	}
	test_assert_equal(-1, check_file(FILE_A, len, 4096));

	for (off = 0; off < len; off += step) {
		pattern_fill(pattern + off, step, 9);
	}
	test_assert_equal(-1, check_file(FILE_B, len, 4096));
}

TEST_CASE("what was written survives a remount") {
	test_assert_zero(write_file(FILE_A, DATA_SZ, 10));

	test_assert_zero(umount(FS_DIR));
	test_assert_zero(mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL));

	pattern_fill(pattern, DATA_SZ, 10);
	test_assert_equal(-1, check_file(FILE_A, DATA_SZ, 4096));
}

TEST_CASE("unlinking a multi-cluster file returns, and gives the space back") {
	int i;

	/* It has to be more than one cluster: a single-cluster file deleted fine,
	 * and that difference was the whole diagnosis. */
	test_assert_zero(write_file(FILE_A, DATA_SZ, 11));
	test_assert_zero(remove(FILE_A));
	test_assert_true(0 > open(FILE_A, O_RDONLY));

	/* And the clusters come back: writing the same file again, repeatedly,
	 * must not run the volume out of space. If the free count is a lie or the
	 * chain was not returned, this is where it shows. */
	for (i = 0; i < 8; i++) {
		test_assert_zero(write_file(FILE_A, DATA_SZ, (unsigned)(20 + i)));
		test_assert_equal(-1, check_file(FILE_A, DATA_SZ, 4096));
		test_assert_zero(remove(FILE_A));
	}
}

TEST_CASE("a directory lists what was put in it") {
	DIR *d;
	struct dirent *e;
	int seen = 0;

	test_assert_zero(write_file(FILE_A, 1024, 12));
	test_assert_zero(write_file(FILE_B, 1024, 13));

	test_assert_not_null(d = opendir(FS_DIR));
	while ((e = readdir(d)) != NULL) {
		if (0 == strcmp(e->d_name, "a.bin")) {
			seen |= 1;
		}
		if (0 == strcmp(e->d_name, "b.bin")) {
			seen |= 2;
		}
	}
	closedir(d);

	test_assert_equal(3, seen);

	test_assert_zero(remove(FILE_A));
	test_assert_zero(remove(FILE_B));
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
	/* FS_DIR is not created here: the root is read-only initfs and mkdir()
	 * there fails. It comes from the module's own @InitFS entry. */
	if (0 != (res = mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL))) {
		return res;
	}

	return 0;
}

static int teardown_suite(void) {
	umount(FS_DIR);
	return ramdisk_delete(FS_DEV);
}

/* ------------------------------------------------------------------ *
 * A DIRECTORY THAT WILL NOT GO AWAY -- reported, not asserted.
 *
 * remove() of a directory whose files have all been deleted returns EPERM.
 * Both shapes do it: files that had content, and files created empty. What
 * does NOT do it is fs/filesystem_test next door, which makes two empty files
 * in a directory on a much smaller volume and removes all three -- so the
 * difference is the volume, the file count, or both, and finding out which is
 * a piece of work this suite is not.
 *
 * It is printed rather than asserted because a suite that fails aborts the
 * runlevel: the board never reaches a shell, and one open defect would cost
 * every other case in this file. The line appears on every boot until
 * somebody fixes it, which is the point.
 *
 * These cases run LAST for the same reason they cannot clean up: a directory
 * that stays holds an inode, and the driver has sixteen of them.
 * ------------------------------------------------------------------ */
#define DIR_FILES 8

static void dir_case(const char *dir, int with_content) {
	int res;
	char path[64];
	int i, fd;

	if (0 != mkdir(dir, 0777)) {
		printk("fat_ops: mkdir(%s) failed, errno %d\n", dir, errno);
		return;
	}

	for (i = 0; i < DIR_FILES; i++) {
		snprintf(path, sizeof(path), "%s/f%02d.bin", dir, i);
		if (with_content) {
			test_assert_zero(write_file(path, 4096, (unsigned)(200 + i)));
		}
		else {
			test_assert_true(0 <= (fd = creat(path, 0666)));
			close(fd);
		}
	}

	if (with_content) {
		for (i = 0; i < DIR_FILES; i++) {
			snprintf(path, sizeof(path), "%s/f%02d.bin", dir, i);
			pattern_fill(pattern, 4096, (unsigned)(200 + i));
			test_assert_equal(-1, check_file(path, 4096, 512));
		}
	}

	for (i = 0; i < DIR_FILES; i++) {
		snprintf(path, sizeof(path), "%s/f%02d.bin", dir, i);
		test_assert_zero(remove(path));
	}

	/* This used to be a report rather than an assertion, because rmdir()
	 * answered EPERM here on every boot. The cause was not the removal: a
	 * directory created by this driver had one sector of its cluster written
	 * and the rest left as the volume found them, so a scan that walked past
	 * the entries -- which eight files with long names make it do -- ran into
	 * old file data and called the directory non-empty. Now it is an
	 * assertion, and the errno is printed before it so that a failure names
	 * itself. */
	res = remove(dir);
	if (res != 0) {
		printk("fat_ops: rmdir(%s) failed with errno %d after deleting %d %s "
		       "files\n",
		    dir, errno, DIR_FILES, with_content ? "written" : "empty");
	}
	test_assert_zero(res);
}

TEST_CASE("files in a subdirectory are written, read back and deleted") {
	dir_case(FS_DIR "/many", 1);
}

TEST_CASE("empty files in a subdirectory are created and deleted") {
	dir_case(FS_DIR "/empties", 0);
}

/* ------------------------------------------------------------------ *
 * A mount that fails must leave the tree as it found it.
 *
 * kmount() allocates the superblock, walks the volume making an inode
 * per file, and only then asks the mount table to take it. Every early return
 * between those steps used to leave something behind -- a superblock, or a
 * whole subtree of inodes hanging off a root nobody can reach, which the next
 * path walk finds. The record calls it "worth its own look before anything
 * ships that mounts a volume it does not control", and an update does exactly
 * that.
 *
 * The loop is the point: a single leak is invisible, eight of them empty a
 * pool, and the mount at the end is what notices.
 */
TEST_CASE("a mount that fails leaves the tree as it was") {
	struct stat st;
	int i;

	/* The lookup fails AFTER the superblock has been allocated, which is the
	 * shape that leaks one. The pool holds 32, so forty attempts empty it if
	 * nothing is given back -- and one attempt would show nothing at all. */
	for (i = 0; i < 40; i++) {
		test_assert_not_zero(mount(FS_DEV, "/no/such/place", FS_NAME, 0, NULL));
	}

	/* The tree still resolves and the mounted volume is still there. */
	test_assert_zero(stat("/", &st));
	test_assert_zero(stat(FS_DIR, &st));
	test_assert_zero(access(FILE_A, F_OK));

	/* And this is where a leak shows: a mount needs a superblock, and forty
	 * failed ones must not have spent them. */
	test_assert_zero(umount(FS_DIR));
	test_assert_zero(mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL));
	test_assert_zero(access(FILE_A, F_OK));
}

/* The root directory is not a cluster chain.
 *
 * The root directory of a FAT12/16 volume is a fixed run of sectors --
 * 512 entries is 32 sectors here, where a cluster is 4 -- and it is not a
 * cluster chain. A driver that walks it by following the FAT leaves the root
 * after the first cluster and carries on reading whichever data cluster that
 * FAT entry happens to name.
 *
 * On an empty volume the data area is zeroes, so the walk stops there and
 * nothing looks wrong. Once anything has been written, the walk reads file
 * data as directory entries. That is why this case remounts: the in-memory
 * tree answers open() correctly no matter what is on the disk, so only a walk
 * of the volume itself asks the question. */
TEST_CASE("the root directory holds more than one cluster of entries") {
	enum { ROOT_FILES = 96 };
	char path[64];
	int i, found = 0, missing = -1;
	unsigned file_denied = fat_pool_use.file_denied;
	unsigned dirinfo_denied = fat_pool_use.dirinfo_denied;

	for (i = 0; i < ROOT_FILES; i++) {
		int fd;

		snprintf(path, sizeof(path), FS_DIR "/r%03d.bin", i);
		fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (fd < 0) {
			printk("fat_ops: creating root file %d failed, errno %d\n", i,
			    errno);
		}
		test_assert(fd >= 0);
		test_assert_equal(write(fd, &i, sizeof(i)), (ssize_t)sizeof(i));
		test_assert_zero(close(fd));
	}

	test_assert_zero(umount(FS_DIR));
	test_assert_zero(mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL));

	for (i = 0; i < ROOT_FILES; i++) {
		int fd, got = -1;

		snprintf(path, sizeof(path), FS_DIR "/r%03d.bin", i);
		fd = open(path, O_RDONLY);
		if (fd < 0) {
			if (missing < 0) {
				missing = i;
			}
			continue;
		}
		if (sizeof(got) == read(fd, &got, sizeof(got)) && got == i) {
			found++;
		}
		else if (missing < 0) {
			missing = i;
		}
		close(fd);
	}
	if (found != ROOT_FILES) {
		printk("fat_ops: %d of %d root files came back after the remount, "
		       "first one lost is %d\n",
		    found, ROOT_FILES, missing);
	}

	/* A pool that ran out during the walk is a different failure from a walk
	 * that went to the wrong sectors, and from here the two look identical:
	 * entries that are on the disk are not in the tree. Telling those two
	 * apart once took a matrix of negative controls. Said out loud, so that
	 * the next one does not. */
	if (fat_pool_use.file_denied != file_denied
	    || fat_pool_use.dirinfo_denied != dirinfo_denied) {
		printk("fat_ops: the FAT pools refused %u file(s) and %u dirinfo(s) "
		       "during this case; live %u/%u and %u/%u, peak %u and %u\n",
		    fat_pool_use.file_denied - file_denied,
		    fat_pool_use.dirinfo_denied - dirinfo_denied,
		    fat_pool_use.file_live, fat_pool_use.file_max,
		    fat_pool_use.dirinfo_live, fat_pool_use.dirinfo_max,
		    fat_pool_use.file_peak, fat_pool_use.dirinfo_peak);
	}
	test_assert_equal(fat_pool_use.file_denied, file_denied);
	test_assert_equal(fat_pool_use.dirinfo_denied, dirinfo_denied);

	test_assert_equal(found, ROOT_FILES);

	for (i = 0; i < ROOT_FILES; i++) {
		snprintf(path, sizeof(path), FS_DIR "/r%03d.bin", i);
		test_assert_zero(remove(path));
	}
}
