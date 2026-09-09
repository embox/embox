/**
 * @file
 * @brief An 8 MiB file through the stock allocator, written and read back.
 *
 * Recorded defect: a stock-allocator write of 8 MiB into a fresh file
 * returned success for all 128 chunks and left 5.4 MiB on the medium, wrong
 * from cluster 857 on -- and it did not reproduce every time, which is worse
 * than a deterministic defect rather than better. It was worked around at
 * the time and never explained.
 *
 * The allocator has been changed several times since -- a lazy cursor, an
 * honest free count, unlink of a multi-cluster file -- and **no
 * re-measurement was ever recorded**. So it is not known whether the defect
 * is alive. This suite is that measurement:
 *
 *   - a regenerable pattern, so a mismatch names the offset rather than
 *     saying "the file differs";
 *   - every chunk's return value checked, because the original returned
 *     success for all of them;
 *   - the size checked separately from the content, because the original was
 *     short AND wrong, and those are two different failures;
 *   - read back through a fresh descriptor after a remount, because a
 *     correct file and a correct cache are not the same thing.
 *
 * Not run at boot: it writes 8 MiB through an emulated block device, which
 * costs seconds. Run it by name:
 *
 *   test -t fat_big
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
/* 40 MiB of volume for an 8 MiB file: room for the file, its chain, and the
 * slack a FAT volume needs, without pretending to be a card. */
/* Bytes, not pages. It was FS_PAGES * PAGE_SIZE(), which is 40 MiB only
 * where a page is 4 KiB: on a board configured with 64 KiB pages the same
 * expression asks for 640 MiB, more than the machine has. */
#define FS_BYTES   (40 * 1024 * 1024)
#define FS_DIR     "/tmp_big"
#define BIG_FILE   FS_DIR "/big.bin"

#define BIG_SIZE   (8 * 1024 * 1024)
#define CHUNK      (64 * 1024)

static char chunk[CHUNK];

/* Byte i of the file is a function of i alone, so a mismatch names the offset
 * it happened at rather than "the file differs". */
static void chunk_fill(char *buf, size_t len, size_t off) {
	size_t i;

	for (i = 0; i < len; i++) {
		size_t at = off + i;

		buf[i] = (char)((at * 31u + (at >> 9) * 7u + (at >> 17)) & 0xff);
	}
}

/* Reads the volume's root directory straight off the block device, so that
 * "the entry is on the disk" and "the mount finds it" are two answers and not
 * one. The geometry is computed exactly as fat_get_volinfo() computes it --
 * including its FAT32-EBPB fallback when bpb.secperfat is zero -- so that the
 * dump looks at the sectors the driver looks at and not at the ones a
 * textbook says it should. */
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
	 * that entry in the FAT decides where the walk goes after the first
	 * secperclus sectors. Print it, and the entry after it. */
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


/* Reads the whole file back through a fresh descriptor and compares it with
 * the pattern, naming the first byte that differs. */
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

/* Does anything at all survive a remount on a volume this driver formatted?
 * One small file, nothing else, so that the answer is about the mount and not
 * about the 8 MiB above. */
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

/* Answers whether the file is there at all after a remount, without asserting
 * -- see the note at the call site. */
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

	/* The size, separately from the content: the original was short AND
	 * wrong, and those are two different failures. */
	test_assert_zero(stat(BIG_FILE, &st));
	if (st.st_size != BIG_SIZE) {
		printk("fat_big: the file is %u bytes, %u were written\n",
		    (unsigned)st.st_size, (unsigned)BIG_SIZE);
	}
	test_assert_equal((size_t)st.st_size, (size_t)BIG_SIZE);

	/* Read it back on the same mount first: that is the allocator's own
	 * question, and it must not be answered together with anything else. */
	verify_content("on the same mount");

	/* The file has to be listed before the remount as well: if it is not,
	 * the remount is not the question. */
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

	/* Then through a remount, because a correct file and a correct cache are
	 * not the same thing. */
	test_assert_zero(umount(FS_DIR));
	test_assert_zero(mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL));

	/* The same file after a remount is a SEPARATE question from the one above,
	 * and for a long time it had a different answer -- the mount walk of the
	 * remounted volume came back with one entry, "root", and neither file. The
	 * disk was never the problem: dump_root() below reads the directory off
	 * the block device, and it held both entries the whole time.
	 *
	 * What the walk was doing is in fat_root_dir_record() and in
	 * fat_fetch_dir(): format wrote a directory entry for the root INTO the
	 * root, pointing at a cluster number that was really a sector address
	 * divided by the cluster size, and the walk descended into it and read
	 * the file's own data as directory entries. It only showed once
	 * something had been written over those sectors, which is why 8 MiB
	 * broke it and a 4-byte file did not.
	 *
	 * So it is asserted here. The dump runs only when the assertion is about
	 * to fail, because the answer to "is it the disk or the walk?" is the
	 * whole diagnosis and it should not have to be added by hand next
	 * time. */
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
		/* The other way for entries that are on the disk to be missing from
		 * the tree: the walk found them and had nothing to put them in. This
		 * is how the root-directory defect actually presented -- one stray
		 * entry took the last object in a pool of sixteen. */
		printk("fat_big: FAT pools: file %u/%u peak %u refused %u, "
		       "dirinfo %u/%u peak %u refused %u\n",
		    fat_pool_use.file_live, fat_pool_use.file_max,
		    fat_pool_use.file_peak, fat_pool_use.file_denied,
		    fat_pool_use.dirinfo_live, fat_pool_use.dirinfo_max,
		    fat_pool_use.dirinfo_peak, fat_pool_use.dirinfo_denied);
	}
	test_assert_true(file_survives_remount());

	/* And it is the same file: a name in a directory is not the content. */
	verify_content("after the remount");

	printk("fat_big: %u MiB written, read back byte for byte on the mount "
	       "that wrote it and again after a remount\n",
	    (unsigned)(BIG_SIZE / (1024 * 1024)));

	test_assert_zero(remove(BIG_FILE));
}
