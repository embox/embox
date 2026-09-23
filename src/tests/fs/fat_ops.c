/**
 * @file
 * @brief FAT driver tests on a ramdisk, checked byte for byte.
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
/* In bytes, so the volume is the same on every board; large enough for
 * sixteen multi-cluster files. */
#define FS_BYTES   (8 * 1024 * 1024)
#define FS_DIR    "/tmp_fat"

#define FILE_A    FS_DIR "/a.bin"
#define FILE_B    FS_DIR "/b.bin"

/* Static: too large for a kernel-context stack. */
#define DATA_SZ   (32 * 1024)
static char pattern[DATA_SZ];
static char readback[DATA_SZ];

/* Byte i depends only on i and the seed, so a mismatch names its offset. */
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
		/* Name the failing call: it tells a defect from a full volume. */
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
 * the first wrong byte, -1 if all is correct, or -2 if the file ended early. */
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

/* ------------------------------------------------------------------ *
 * The volume read from the device rather than the driver: only the bytes
 * tell a correct volume from a correct cache. */

struct volume {
	unsigned bytepersec, secperclus, reserved, numfats, rootentries;
	unsigned secperfat, numsecs, rootsecs, dataarea, clusters, bits;
	unsigned rootstart;      /* first root SECTOR (FAT12/16) */
	unsigned rootclus;       /* root CLUSTER (FAT32) */
	const uint8_t *sysid;
	uint8_t sec[512];
};

static struct block_dev *fat_bdev(void) {
	return block_dev_find("ramdisk_fat");
}

/* Fill a whole block device with one byte. See setup_suite for why. */
static void poison_media(const char *dev, size_t bytes, uint8_t fill) {
	struct block_dev *bdev = block_dev_find(dev + sizeof("/dev/") - 1);
	uint8_t buf[512];
	size_t off;

	if (bdev == NULL) {
		printk("fat_ops: %s not found, media left as it was\n", dev);
		return;
	}
	memset(buf, fill, sizeof(buf));
	for (off = 0; off + sizeof(buf) <= bytes; off += sizeof(buf)) {
		if ((int)sizeof(buf) != block_dev_write(bdev, (char *)buf, sizeof(buf),
		        off / bdev->block_size)) {
			printk("fat_ops: could not poison %s at offset %u\n", dev,
			    (unsigned)off);
			return;
		}
	}
}

/* Parse the boot sector. Returns 0, or -1 if the device cannot be read. */
static int volume_read(struct block_dev *bdev, struct volume *v) {
	uint32_t fatsize32;

	if (bdev == NULL) {
		printk("fat_ops: volume_read: no device\n");
		return -1;
	}
	memset(v, 0, sizeof(*v));
	if ((int)sizeof(v->sec)
	    != block_dev_read(bdev, (char *)v->sec, sizeof(v->sec), 0)) {
		printk("fat_ops: volume_read: sector 0 of %s could not be read\n",
		    block_dev_name(bdev));
		return -1;
	}

	v->bytepersec  = v->sec[11] | (v->sec[12] << 8);
	v->secperclus  = v->sec[13];
	v->reserved    = v->sec[14] | (v->sec[15] << 8);
	v->numfats     = v->sec[16];
	v->rootentries = v->sec[17] | (v->sec[18] << 8);
	v->numsecs     = v->sec[19] | (v->sec[20] << 8);
	v->secperfat   = v->sec[22] | (v->sec[23] << 8);
	if (v->numsecs == 0) {
		v->numsecs = v->sec[32] | (v->sec[33] << 8)
		             | (v->sec[34] << 16) | (v->sec[35] << 24);
	}
	fatsize32 = v->sec[36] | (v->sec[37] << 8)
	            | (v->sec[38] << 16) | (v->sec[39] << 24);
	v->rootclus = v->sec[44] | (v->sec[45] << 8)
	              | (v->sec[46] << 16) | (v->sec[47] << 24);

	if (v->bytepersec == 0 || v->secperclus == 0) {
		printk("fat_ops: volume_read: %s has no boot sector -- first bytes "
		       "%02x %02x %02x %02x, %u B/sec, %u sec/clus\n",
		    block_dev_name(bdev), v->sec[0], v->sec[1], v->sec[2], v->sec[3],
		    v->bytepersec, v->secperclus);
		return -1;
	}
	v->rootsecs = (v->rootentries * 32 + v->bytepersec - 1) / v->bytepersec;
	v->rootstart = v->reserved
	               + v->numfats * (v->secperfat ? v->secperfat : fatsize32);
	v->dataarea = v->rootstart + v->rootsecs;
	if (v->numsecs <= v->dataarea) {
		printk("fat_ops: volume_read: %s says %u sectors but its data starts "
		       "at %u (%u reserved, %u FAT(s) of %u, %u root sectors)\n",
		    block_dev_name(bdev), v->numsecs, v->dataarea, v->reserved,
		    v->numfats, v->secperfat, v->rootsecs);
		return -1;
	}
	v->clusters = (v->numsecs - v->dataarea) / v->secperclus;
	v->bits = (v->clusters < 4085) ? 12 : (v->clusters < 65525) ? 16 : 32;
	v->sysid = (v->bits == 32) ? &v->sec[82] : &v->sec[54];
	if (!v->secperfat) {
		v->secperfat = fatsize32;
	}
	return 0;
}

static void volume_print(const struct volume *v, const char *what) {
	printk("fat_ops: %s: %u sectors of %u, %u/clus, %u reserved, %u FAT(s) "
	       "of %u, %u root entries at sector %u, data at %u, %u clusters -> "
	       "FAT%u, system \"%.8s\"\n",
	    what, v->numsecs, v->bytepersec, v->secperclus, v->reserved,
	    v->numfats, v->secperfat, v->rootentries, v->rootstart, v->dataarea,
	    v->clusters, v->bits, (const char *)v->sysid);
}

/* The root directory read off the device: tells a write that never landed
 * from a walk that stopped early. Prints each anomaly; returns the number of
 * live 8.3 entries. */
static int root_dump(struct block_dev *bdev, const struct volume *v,
    int expect) {
	uint8_t buf[512];
	unsigned s, e;
	int live = 0, deleted = 0, lfn = 0;
	int first_free = -1, after_free = 0;

	if (bdev == NULL || v->rootentries == 0) {
		return -1;
	}

	for (s = 0; s < v->rootsecs; s++) {
		if ((int)sizeof(buf) != block_dev_read(bdev, (char *)buf, sizeof(buf),
		        (v->rootstart + s) * (v->bytepersec / bdev->block_size))) {
			printk("fat_ops: root sector %u could not be read\n",
			    v->rootstart + s);
			return -1;
		}
		for (e = 0; e < sizeof(buf) / 32; e++) {
			const uint8_t *d = buf + e * 32;
			int idx = (int)(s * (sizeof(buf) / 32) + e);

			if (d[0] == 0x00) {
				if (first_free < 0) {
					first_free = idx;
				}
				continue;
			}
			if (first_free >= 0) {
				after_free++;   /* a live entry past the end-of-directory mark */
			}
			if (d[0] == 0xe5) {
				deleted++;
				continue;
			}
			if ((d[11] & 0x0f) == 0x0f) {
				lfn++;
				continue;
			}
			live++;
		}
	}

	printk("fat_ops: the root area holds %d live entries, %d deleted, %d "
	       "long-name; first free entry at %d, %d live entries AFTER it "
	       "(expected %d files)\n",
	    live, deleted, lfn, first_free, after_free, expect);
	if (after_free) {
		printk("fat_ops: entries past the first free slot are unreachable: a "
		       "walk stops at the first 0x00 entry, so those files are on the "
		       "media and not in any listing\n");
	}
	return live;
}

/* The boot sector mkfs wrote must describe the volume mkfs made:
 *   FAT12/16  rootentries != 0, FAT size in the BPB
 *   FAT32     rootentries == 0, BPB FAT size 0, size in the EBPB, root a chain
 * Readers take the type from the cluster count. Read from the device. */
TEST_CASE("the boot sector describes the volume that was made") {
	struct block_dev *bdev = fat_bdev();
	struct volume v;
	uint8_t fat[512];

	test_assert_not_null(bdev);
	test_assert_zero(volume_read(bdev, &v));
	volume_print(&v, "boot sector");

	test_assert_equal(0x55, v.sec[510]);
	test_assert_equal(0xaa, v.sec[511]);
	test_assert_equal(512u, v.bytepersec);
	test_assert(v.reserved > 0);
	test_assert(v.numfats > 0);

	if (v.bits == 32) {
		test_assert_equal(0u, v.rootentries);
		/* A zero BPB FAT size tells a reader to take the FAT32 EBPB's. */
		test_assert_equal(0u, (unsigned)(v.sec[22] | (v.sec[23] << 8)));
		test_assert(v.secperfat > 0);
		test_assert(v.rootclus >= 2);
		test_assert_zero(memcmp(v.sysid, "FAT32   ", 8));
	}
	else {
		/* FAT12/16: a fixed root area, and the FAT size in the BPB. */
		test_assert(v.rootentries > 0);
		test_assert(v.sec[22] != 0 || v.sec[23] != 0);
		test_assert_zero(
		    memcmp(v.sysid, v.bits == 12 ? "FAT12   " : "FAT16   ", 8));
	}

	/* Entry 0 is the media descriptor, entry 1 end-of-chain; a zero there lets
	 * the allocator hand out cluster 0 or 1. On FAT32 the root's cluster must
	 * be claimed too. */
	test_assert_equal((int)sizeof(fat),
	    block_dev_read(bdev, (char *)fat, sizeof(fat),
	        v.reserved * (v.bytepersec / bdev->block_size)));
	test_assert_equal(fat[0], 0xf8);
	test_assert(fat[1] != 0 || fat[2] != 0);
	if (v.bits == 32) {
		test_assert(fat[8] != 0 || fat[9] != 0 || fat[10] != 0 || fat[11] != 0);
	}
}

/* FAT32 needs at least 65 525 clusters: of 4 sectors of 512 bytes that is
 * 134 MiB, and 140 keeps off the boundary. Only the formatter is exercised. A
 * failed assertion longjmps out, so the ramdisk is deleted before the first. */
TEST_CASE("a volume big enough for FAT32 is formatted as one") {
	enum { F32_BYTES = 140 * 1024 * 1024 };
	struct block_dev *bdev;
	struct volume v;
	uint8_t fat[512], root[512];
	int res, made = 0, formatted = -1;
	unsigned rootsec = 0;

	res = ptr2err(ramdisk_create("/dev/ramdisk_f32", F32_BYTES));
	if (res != 0) {
		/* Not a failure: a board with less memory cannot hold a FAT32
		 * volume. */
		printk("fat_ops: no %u MiB ramdisk for the FAT32 case (%d) -- skipped\n",
		    (unsigned)(F32_BYTES / (1024 * 1024)), res);
		return;
	}
	made = 1;
	memset(&v, 0, sizeof(v));
	memset(fat, 0, sizeof(fat));
	memset(root, 0, sizeof(root));

	/* Only the metadata end: 8 MiB covers the reserved sector, both FATs and
	 * the first data clusters; all 140 MiB would be slow. */
	poison_media("/dev/ramdisk_f32", 8 * 1024 * 1024, 0xf6);

	formatted = format("/dev/ramdisk_f32", FS_NAME);
	bdev = block_dev_find("ramdisk_f32");
	if (formatted != 0 || bdev == NULL) {
		printk("fat_ops: FAT32 volume: format returned %d, device %sfound\n",
		    formatted, bdev ? "" : "NOT ");
	}
	if (formatted == 0 && bdev != NULL && volume_read(bdev, &v) == 0) {
		volume_print(&v, "the FAT32 volume");
		block_dev_read(bdev, (char *)fat, sizeof(fat),
		    v.reserved * (v.bytepersec / bdev->block_size));
		if (v.rootclus >= 2) {
			rootsec = v.dataarea + (v.rootclus - 2) * v.secperclus;
			block_dev_read(bdev, (char *)root, sizeof(root),
			    rootsec * (v.bytepersec / bdev->block_size));
		}
	}

	if (made) {
		ramdisk_delete("/dev/ramdisk_f32");
	}

	/* Nothing below here touches the device. */
	test_assert_zero(formatted);
	test_assert(v.clusters >= 65525);
	test_assert_equal(32u, v.bits);

	/* The FAT32 shape, in full: no root area, the BPB's FAT size zero so a
	 * reader takes the EBPB's, and a root cluster to start the chain at. */
	test_assert_equal(0u, v.rootentries);
	test_assert_equal(0u, (unsigned)(v.sec[22] | (v.sec[23] << 8)));
	test_assert(v.secperfat > 0);
	test_assert(v.rootclus >= 2);
	test_assert_zero(memcmp(v.sysid, "FAT32   ", 8));

	/* Entries 0 and 1 reserved, and the root's own cluster claimed: a zero
	 * there is a free cluster, handed to the first file written. */
	test_assert_equal(fat[0], 0xf8);
	test_assert(fat[1] != 0 || fat[2] != 0 || fat[3] != 0);
	test_assert(fat[8] != 0 || fat[9] != 0 || fat[10] != 0 || fat[11] != 0);

	/* And empty: the media was 0xf6, so an uncleared root cluster holds
	 * entries nobody wrote. */
	test_assert(rootsec != 0);
	test_assert_zero(root[0]);
	test_assert_zero(memcmp(root, root + 1, sizeof(root) - 1));
}

TEST_CASE("a read longer than one sector returns the whole thing") {
	test_assert_zero(write_file(FILE_A, DATA_SZ, 1));

	/* One sector, then sizes the driver's buffer does not cover in one go. */
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

	/* No close in between: a read in the same session as the write. */
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

	/* Backwards across a cluster boundary, which a cursor that only
	 * advances gets wrong. */
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

	/* Rebuild the pattern as written: the same block repeated. */
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

	/* More than one cluster, so a chain is freed. */
	test_assert_zero(write_file(FILE_A, DATA_SZ, 11));
	test_assert_zero(remove(FILE_A));
	test_assert_true(0 > open(FILE_A, O_RDONLY));

	/* The clusters come back: rewriting the file must not run the volume out
	 * of space. */
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
		printk("fat_ops: setup: ramdisk_create(%s) failed, %d\n", FS_DEV, res);
		return res;
	}

	/* Format over non-blank media: ramdisk pages are not cleared, nor are
	 * cards. 0xf6 is neither a free (0x00) nor a deleted (0xE5) directory
	 * entry, and is an in-use FAT entry. */
	poison_media(FS_DEV, FS_BYTES, 0xf6);

	if (0 != (res = format(FS_DEV, FS_NAME))) {
		printk("fat_ops: setup: format(%s) failed, %d\n", FS_DEV, res);
		return res;
	}
	/* FS_DIR comes from the module's @InitFS entry: the initfs root is
	 * read-only. */
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
 * A directory whose files have all been deleted can be removed. A directory
 * left behind holds one of the driver's sixteen inodes.
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

	/* Eight long names push the scan past the first sector, where an uncleared
	 * directory cluster reads as non-empty. */
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
 * A mount that fails leaves the tree as it found it: no superblock and no
 * inodes behind. Repeated, since one leak is invisible and several empty a
 * pool.
 */
TEST_CASE("a mount that fails leaves the tree as it was") {
	struct stat st;
	int i;

	/* Fails after the superblock is allocated. The pool holds 32, so forty
	 * attempts empty it if nothing is given back. */
	for (i = 0; i < 40; i++) {
		test_assert_not_zero(mount(FS_DEV, "/no/such/place", FS_NAME, 0, NULL));
	}

	/* The tree still resolves and the mounted volume is still there. */
	test_assert_zero(stat("/", &st));
	test_assert_zero(stat(FS_DIR, &st));
	test_assert_zero(access(FILE_A, F_OK));

	/* A mount needs a superblock, so a leak shows here. */
	test_assert_zero(umount(FS_DIR));
	test_assert_zero(mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL));
	test_assert_zero(access(FILE_A, F_OK));
}

/* The FAT12/16 root directory is a fixed run of sectors (32 here; a cluster
 * is 4), not a cluster chain. The case remounts: the in-memory tree answers
 * open() whatever is on the disk, and only a walk of the volume tests it. */
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
		struct volume v;
		int j, run;

		printk("fat_ops: %d of %d root files came back after the remount, "
		       "first one lost is %d\n",
		    found, ROOT_FILES, missing);

		/* Missing names as runs: a walk that stopped and one that lost
		 * scattered entries are different defects. */
		printk("fat_ops: missing:");
		for (j = 0; j < ROOT_FILES; j = run) {
			int fd;

			snprintf(path, sizeof(path), FS_DIR "/r%03d.bin", j);
			fd = open(path, O_RDONLY);
			if (fd >= 0) {
				close(fd);
				run = j + 1;
				continue;
			}
			for (run = j; run < ROOT_FILES; run++) {
				snprintf(path, sizeof(path), FS_DIR "/r%03d.bin", run);
				fd = open(path, O_RDONLY);
				if (fd >= 0) {
					close(fd);
					break;
				}
			}
			printk(" %d-%d", j, run - 1);
		}
		printk("\n");

		/* Are those names on the media? If so the write worked and the walk
		 * is wrong; if not, the write never landed. */
		if (volume_read(fat_bdev(), &v) == 0) {
			volume_print(&v, "the volume they were written to");
			root_dump(fat_bdev(), &v, ROOT_FILES);
		}
	}

	/* Pool exhaustion and a walk of the wrong sectors look the same here:
	 * entries on the disk are missing from the tree. Say which. */
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

/* The live 8.3 entries of the root, off the media: how many, and whether
 * each one's first cluster and write date are ones a reader can believe. */
static int root_live_entries(int *bad_clus, int *bad_date) {
	struct block_dev *bdev = fat_bdev();
	struct volume v;
	uint8_t buf[512];
	unsigned s, e;
	int live = 0;

	*bad_clus = *bad_date = 0;
	if (bdev == NULL || volume_read(bdev, &v) != 0 || v.rootentries == 0) {
		return -1;
	}
	for (s = 0; s < v.rootsecs; s++) {
		if ((int)sizeof(buf) != block_dev_read(bdev, (char *)buf, sizeof(buf),
		        (v.rootstart + s) * (v.bytepersec / bdev->block_size))) {
			return -1;
		}
		for (e = 0; e < sizeof(buf) / 32; e++) {
			const uint8_t *d = buf + e * 32;
			unsigned clus, date, month, day;

			if (d[0] == 0x00 || d[0] == 0xe5 || (d[11] & 0x0f) == 0x0f
			    || (d[11] & 0x08)) {   /* free, deleted, long name, label */
				continue;
			}
			live++;
			clus = d[26] | (d[27] << 8);
			if (clus != 0 && (clus < 2 || clus >= v.clusters + 2)) {
				printk("fat_ops: \"%.11s\" starts at cluster %#x, outside "
				       "the volume's 2..%u\n", (const char *)d, clus,
				    v.clusters + 1);
				(*bad_clus)++;
			}
			date = d[24] | (d[25] << 8);
			month = (date >> 5) & 0xf;
			day = date & 0x1f;
			if (month < 1 || month > 12 || day < 1) {
				printk("fat_ops: \"%.11s\" is dated %u-%02u-%02u\n",
				    (const char *)d, 1980 + (date >> 9), month, day);
				(*bad_date)++;
			}
		}
	}
	return live;
}

/* The entries' dates are checked too. */
TEST_CASE("a file on a full volume is refused with ENOSPC, and leaves nothing") {
	int fd, n, bad_clus, bad_date;
	ssize_t w;

	size_t total = 0;

	pattern_fill(pattern, DATA_SZ, 40);
	fd = open(FILE_A, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	test_assert(fd >= 0);
	/* Bounded by the volume: a write past it is going round a closed chain. */
	do {
		w = write(fd, pattern, DATA_SZ);
		if (w > 0) {
			total += (size_t)w;
		}
	} while (w == DATA_SZ && total <= FS_BYTES);
	test_assert_zero(close(fd));
	if (total > FS_BYTES) {
		printk("fat_ops: %u bytes written to a %u-byte volume\n",
		    (unsigned)total, (unsigned)FS_BYTES);
	}
	test_assert(total <= FS_BYTES);

	fd = open(FILE_B, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd >= 0) {
		close(fd);
		printk("fat_ops: a file was created on a full volume\n");
	}
	test_assert(fd < 0);
	test_assert_equal(ENOSPC, errno);

	n = root_live_entries(&bad_clus, &bad_date);
	test_assert_equal(1, n);
	test_assert_zero(bad_clus);
	test_assert_zero(bad_date);

	/* Freed, the volume takes a file again, and a remount sees it. */
	test_assert_zero(remove(FILE_A));
	test_assert_zero(write_file(FILE_B, DATA_SZ, 41));
	test_assert_zero(umount(FS_DIR));
	test_assert_zero(mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL));
	pattern_fill(pattern, DATA_SZ, 41);
	test_assert_equal(-1, check_file(FILE_B, DATA_SZ, 4096));
	test_assert(0 > open(FILE_A, O_RDONLY));
	test_assert_zero(remove(FILE_B));
}
