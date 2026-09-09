/**
 * @file
 * @brief Writes an 8 MiB FAT file and reads it back before and after a remount.
 */

#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>

#include <drivers/block_dev.h>
#include <fs/fat_pool_use.h>
#include <drivers/block_dev/ramdisk/ramdisk.h>
#include <embox/test.h>
#include <fs/fsop.h>
#include <kernel/printk.h>
#include <util/err.h>

EMBOX_TEST_SUITE_NOAUTO("an 8 MiB file through the allocator");

TEST_SETUP_SUITE(setup_suite);
TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME    "vfat"
#define FS_DEV_NAME "ramdisk_big"
#define FS_DEV     "/dev/" FS_DEV_NAME
/* 40 MiB volume for an 8 MiB file and its FAT. In bytes, not pages: the
 * page size varies by board. */
#define FS_BYTES   (40 * 1024 * 1024)
#define FS_DIR     "/tmp_big"
#define BIG_FILE   FS_DIR "/big.bin"

#define BIG_SIZE   (8 * 1024 * 1024)
#define CHUNK      (64 * 1024)

static char chunk[CHUNK];

/* Byte i depends on i alone, so a mismatch names its offset. */
static void chunk_fill(char *buf, size_t len, size_t off) {
	size_t i;

	for (i = 0; i < len; i++) {
		size_t at = off + i;

		buf[i] = (char)((at * 31u + (at >> 9) * 7u + (at >> 17)) & 0xff);
	}
}

/* Dumps the root directory straight off the block device, with geometry
 * computed as fat_get_volinfo() does, to tell an on-disk fault from a
 * mount-walk fault. */
static void dump_root(const char *when) {
	static char sec[512];
	struct block_dev *bdev = block_dev_find(FS_DEV_NAME);
	unsigned bytepersec, secperclus, reserved, numfats, secperfat, rootentries;
	unsigned fat1, rootdir, dataarea, rootsecs, numsecs, s, blkpersec;

	if (bdev == NULL) {
		printk("fat_big: %s, no block device\n", when);
		return;
	}
	if (512 != block_dev_read(bdev, sec, 512, 0)) {
		printk("fat_big: %s, sector 0 unreadable\n", when);
		return;
	}
	bytepersec = (unsigned char)sec[11] | ((unsigned char)sec[12] << 8);
	secperclus = (unsigned char)sec[13];
	reserved = (unsigned char)sec[14] | ((unsigned char)sec[15] << 8);
	numfats = (unsigned char)sec[16];
	rootentries = (unsigned char)sec[17] | ((unsigned char)sec[18] << 8);
	secperfat = (unsigned char)sec[22] | ((unsigned char)sec[23] << 8);
	if (secperfat == 0) {
		/* Fat_get_volinfo() reads it from the FAT32 EBPB here. */
		secperfat = (unsigned char)sec[36] | ((unsigned char)sec[37] << 8)
		            | ((unsigned char)sec[38] << 16)
		            | ((unsigned char)sec[39] << 24);
	}
	numsecs = (unsigned char)sec[19] | ((unsigned char)sec[20] << 8);
	if (numsecs == 0) {
		numsecs = (unsigned char)sec[32] | ((unsigned char)sec[33] << 8)
		          | ((unsigned char)sec[34] << 16)
		          | ((unsigned char)sec[35] << 24);
	}
	if (bytepersec == 0 || secperclus == 0 || bdev->block_size == 0) {
		printk("fat_big: %s, BPB says bytepersec %u secperclus %u\n", when,
		    bytepersec, secperclus);
		return;
	}
	blkpersec = bytepersec / bdev->block_size;
	fat1 = reserved;
	rootsecs = (rootentries * 32 + bytepersec - 1) / bytepersec;
	rootdir = fat1 + secperfat * 2;
	dataarea = rootdir + rootsecs;
	printk("fat_big: %s, bps %u spc %u reserved %u nfats %u secperfat %u "
	       "rootents %u numsecs %u -> fat1 %u root %u..%u data %u clusters "
	       "%u\n",
	    when, bytepersec, secperclus, reserved, numfats, secperfat,
	    rootentries, numsecs, fat1, rootdir, rootdir + rootsecs - 1, dataarea,
	    (numsecs - dataarea) / secperclus);

	/* The root directory is walked as pseudo-cluster rootdir/secperclus, so
	 * its FAT entry decides where the walk goes next. */
	{
		unsigned pseudo = rootdir / secperclus;
		unsigned off = pseudo * 2;

		if (512 == block_dev_read(bdev, sec, 512,
		               (fat1 + off / bytepersec) * blkpersec)) {
			unsigned at = off % bytepersec;

			printk("fat_big: %s, root pseudo-cluster %u, FAT[%u] = %u, "
			       "FAT[%u] = %u\n",
			    when, pseudo, pseudo,
			    (unsigned)((unsigned char)sec[at]
			               | ((unsigned char)sec[at + 1] << 8)),
			    pseudo + 1,
			    (unsigned)((unsigned char)sec[at + 2]
			               | ((unsigned char)sec[at + 3] << 8)));
		}
	}

	for (s = 0; s < rootsecs; s++) {
		int e;

		if (512
		    != block_dev_read(bdev, sec, 512, (rootdir + s) * blkpersec)) {
			printk("fat_big: %s, root sector %u unreadable\n", when,
			    rootdir + s);
			return;
		}
		for (e = 0; e < 512 / 32; e++) {
			char name[12];
			char *de = sec + e * 32;
			int i;

			if (de[0] == 0x00) {
				continue;
			}
			for (i = 0; i < 11; i++) {
				name[i] = (de[i] >= 0x20 && de[i] < 0x7f) ? de[i] : '.';
			}
			name[11] = '\0';
			printk("fat_big: %s, on disk at sector %u entry %d: \"%s\" "
			       "attr %02x clus %u size %u\n",
			    when, rootdir + s, e, name, (unsigned char)de[11],
			    (unsigned)((unsigned char)de[26]
			               | ((unsigned char)de[27] << 8)),
			    (unsigned)((unsigned char)de[28]
			               | ((unsigned char)de[29] << 8)
			               | ((unsigned char)de[30] << 16)
			               | ((unsigned char)de[31] << 24)));
		}
	}
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


/* Compares the file with the pattern via a fresh descriptor. */
static void verify_content(const char *when) {
	size_t off;
	int fd;

	fd = open(BIG_FILE, O_RDONLY);
	if (fd < 0) {
		struct stat st;
		DIR *d;

		printk("fat_big: %s, open(%s) failed with errno %d; access %d, "
		       "stat %d\n",
		    when, BIG_FILE, errno, access(BIG_FILE, F_OK),
		    stat(BIG_FILE, &st));

		d = opendir(FS_DIR);
		if (d != NULL) {
			struct dirent *e;

			while (NULL != (e = readdir(d))) {
				printk("fat_big:   entry \"%s\"\n", e->d_name);
			}
			closedir(d);
		}
	}
	test_assert(fd >= 0);

	for (off = 0; off < BIG_SIZE; off += CHUNK) {
		static char got[CHUNK];
		size_t i;
		int n;

		n = read(fd, got, CHUNK);
		if (n != CHUNK) {
			printk("fat_big: %s, read at %u returned %d, errno %d\n", when,
			    (unsigned)off, n, errno);
		}
		test_assert_equal(n, CHUNK);

		chunk_fill(chunk, CHUNK, off);
		for (i = 0; i < CHUNK; i++) {
			if (got[i] != chunk[i]) {
				printk("fat_big: %s, first difference at byte %u: %02x, "
				       "wanted %02x\n",
				    when, (unsigned)(off + i), (unsigned char)got[i],
				    (unsigned char)chunk[i]);
				break;
			}
		}
		test_assert_equal(i, (size_t)CHUNK);
	}

	test_assert_zero(close(fd));
}

/* One small file across a remount, apart from the 8 MiB case. */
TEST_CASE("a small file survives a remount") {
	int fd;
	char buf[8];

	fd = open(FS_DIR "/tiny.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
	test_assert(fd >= 0);
	test_assert_equal(write(fd, "tiny", 4), 4);
	test_assert_zero(close(fd));

	test_assert_zero(umount(FS_DIR));
	test_assert_zero(mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL));

	fd = open(FS_DIR "/tiny.txt", O_RDONLY);
	if (fd < 0) {
		printk("fat_big: a small file did not survive the remount either "
		       "(errno %d)\n", errno);
	}
	test_assert(fd >= 0);
	test_assert_equal(read(fd, buf, 4), 4);
	test_assert_zero(memcmp(buf, "tiny", 4));
	test_assert_zero(close(fd));
}

/* Does not assert, so the caller can dump diagnostics first. */
static int file_survives_remount(void) {
	int fd = open(BIG_FILE, O_RDONLY);

	if (fd < 0) {
		return 0;
	}
	close(fd);
	return 1;
}

TEST_CASE("8 MiB written through the allocator comes back byte for byte") {
	struct stat st;
	size_t off;
	int fd;

	fd = open(BIG_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	test_assert(fd >= 0);

	for (off = 0; off < BIG_SIZE; off += CHUNK) {
		int written;

		chunk_fill(chunk, CHUNK, off);
		written = write(fd, chunk, CHUNK);
		if (written != CHUNK) {
			printk("fat_big: write at %u returned %d, errno %d\n",
			    (unsigned)off, written, errno);
		}
		test_assert_equal(written, CHUNK);
	}

	test_assert_zero(close(fd));

	/* Size separately from content: a short file and a wrong file are
	 * different failures. */
	test_assert_zero(stat(BIG_FILE, &st));
	if (st.st_size != BIG_SIZE) {
		printk("fat_big: the file is %u bytes, %u were written\n",
		    (unsigned)st.st_size, (unsigned)BIG_SIZE);
	}
	test_assert_equal((size_t)st.st_size, (size_t)BIG_SIZE);

	/* Same mount first, to check the allocator apart from the remount. */
	verify_content("on the same mount");

	/* Listed before the remount too, so a miss after it is the remount's. */
	{
		DIR *d = opendir(FS_DIR);
		struct dirent *e;
		int seen = 0;

		test_assert_not_null(d);
		while (NULL != (e = readdir(d))) {
			if (0 == strcmp(e->d_name, "big.bin")) {
				seen = 1;
			}
		}
		closedir(d);
		test_assert_true(seen);
	}

	/* Then after a remount: a correct cache does not prove a correct disk. */
	test_assert_zero(umount(FS_DIR));
	test_assert_zero(mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL));

	/* On failure, print what the mount walk sees and what is on the disk, to
	 * tell a walk fault from a disk fault. */
	if (!file_survives_remount()) {
		DIR *d = opendir(FS_DIR);
		struct dirent *e;

		printk("fat_big: the file is not there after the remount; what the "
		       "mount walk found:\n");
		if (d != NULL) {
			while (NULL != (e = readdir(d))) {
				printk("fat_big:   entry \"%s\"\n", e->d_name);
			}
			closedir(d);
		}
		printk("fat_big: and what is actually on the volume:\n");
		dump_root("after the remount");
		/* On-disk entries also go missing when the FAT pools run out. */
		printk("fat_big: FAT pools: file %u/%u peak %u refused %u, "
		       "dirinfo %u/%u peak %u refused %u\n",
		    fat_pool_use.file_live, fat_pool_use.file_max,
		    fat_pool_use.file_peak, fat_pool_use.file_denied,
		    fat_pool_use.dirinfo_live, fat_pool_use.dirinfo_max,
		    fat_pool_use.dirinfo_peak, fat_pool_use.dirinfo_denied);
	}
	test_assert_true(file_survives_remount());

	verify_content("after the remount");

	printk("fat_big: %u MiB written, read back byte for byte on the mount "
	       "that wrote it and again after a remount\n",
	    (unsigned)(BIG_SIZE / (1024 * 1024)));

	test_assert_zero(remove(BIG_FILE));
}
