/**
 * @file
 *
 * @date Dec 12, 2019
 * @author Anton Bondarev
 */
#include <stddef.h>
#include <fcntl.h>

#include <fs/file_desc.h>
#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/fat.h>
#include <fs/fat_pool_use.h>
#include <string.h>
#include <util/log.h>

static size_t fat_read_unlocked(struct file_desc *desc, void *buf,
		size_t size) {
	size_t rezult;
	uint32_t bytecount;
	struct fat_file_info *fi;
	size_t pos;

	fi = file_get_inode_data(desc);
	/* And here too, not only in the wrapper. The wrapper's guard
	 * is the one that reports; this one is the one that cannot be skipped by
	 * a future caller that reaches this function directly. The board found
	 * the cost of not having it: a Data abort at FAR 0x2c, which is
	 * `pointer` in this very struct. */
	if (fi == NULL) {
		return 0;
	}
	pos = file_get_pos(desc);

	/* fi->cluster is a cursor on the inode and only the read/write
	 * loops maintain it. If the descriptor is not where the last operation
	 * left the pointer -- after a write, after a seek, after another
	 * descriptor on the same inode -- it has to be put back. */
	if (pos != fi->pointer) {
		if (fat_seek_cluster(fi, fat_sector_buff, pos) != DFS_OK) {
			return 0;
		}
	}
	fi->pointer = pos;

	rezult = fat_read_file(fi, fat_sector_buff, buf, &bytecount, size);
	/* DFS_EOF is what a read that runs into the end of the file
	 * returns, and the bytes it copied first are real. Returning 0 here made
	 * every read larger than what remained in the cluster look like an end
	 * of file. */
	if (DFS_OK == rezult || DFS_EOF == rezult) {
		return bytecount;
	}
	return 0;
}

static size_t fat_write_unlocked(struct file_desc *desc, void *buf,
		size_t size) {
	size_t rezult;
	uint32_t bytecount;
	struct fat_file_info *fi;
	size_t new_sz;
	int old_pos = file_get_pos(desc);

	fi = file_get_inode_data(desc);
	if (fi == NULL) {
		return 0;   /* see the note in fat_read */
	}
	/* Same cursor, same reason -- see fat_read above. */
	if ((size_t)old_pos != fi->pointer && fi->firstcluster >= 2) {
		if (fat_seek_cluster(fi, fat_sector_buff, old_pos) != DFS_OK) {
			return 0;
		}
	}
	fi->pointer = old_pos;
	new_sz = file_get_size(desc);
	fi->mode = O_RDWR; /* XXX */

	rezult = fat_write_file(fi, fat_sector_buff, (uint8_t *)buf,
			&bytecount, size, &new_sz);

	if (DFS_OK == rezult || DFS_EOF == rezult) {
		if (old_pos + bytecount > file_get_size(desc)) {
			file_set_size(desc, old_pos + bytecount);
			fi->filelen = old_pos + bytecount;
		}

		return bytecount;
	}

	return 0;
}

/* An operation on a file whose inode has no private data left.
 *
 * Both functions below begin by reading fi->pointer -- offset 44 -- so a NULL
 * fi is a data abort at address 0x2c with interrupts masked, which stops the
 * whole board and leaves the answer in a register nobody can read. Board
 * boots 60, 63 and 66 died exactly there, in the unlink loop.
 *
 * Refusing the operation instead is not a fix: something is still using a
 * file it has destroyed, and that is the defect. But a run that finishes and
 * names the file is worth more than a crash, and the count says whether it
 * happened once or constantly. See fs/fat_pool_use.h. */
static int fat_no_priv(struct file_desc *desc, int writing) {
	struct inode *node;
	const char *name;

	if (file_get_inode_data(desc)) {
		return 0;
	}
	if (writing) {
		fat_null_priv.writes++;
	}
	else {
		fat_null_priv.reads++;
	}
	node = desc ? desc->f_inode : NULL;
	name = node ? inode_name(node) : NULL;
	strncpy(fat_null_priv.last, name ? name : "(unnamed)",
	    sizeof(fat_null_priv.last) - 1);
	fat_null_priv.last[sizeof(fat_null_priv.last) - 1] = '\0';
	log_error("%s on \"%s\" whose inode has no private data; refusing",
	    writing ? "write" : "read", fat_null_priv.last);
	return 1;
}

/* The driver's global scratch buffer makes every one of these a critical
 * section against every other.
 *
 * THE GUARD IS INSIDE THE LOCK, and it was not always. Outside it, the check
 * answers a question whose answer changes before it is used: fat_lock() can
 * block, and what it blocks on is another thread holding the same lock --
 * which is any second writer -- a log follower, say -- running while
 * the suite deletes files. The thread that passed the check goes to sleep
 * with a good pointer, wakes after the inode it named has been destroyed,
 * and reads through a null one.
 *
 * The board found it: a Data abort, ESR 96000007 (translation fault, read),
 * FAR 0x2c -- which is `pointer` in struct fat_file_info -- with PC in
 * fat_write_unlocked at the cursor check. Four earlier runs had reported
 * "1 write on an inode with no private data" and been saved by the guard
 * catching it in time; this one lost the race. A check that can go stale
 * between asking and acting is not a guard. */
static size_t fat_read(struct file_desc *desc, void *buf, size_t size) {
	size_t res;

	fat_lock();
	if (fat_no_priv(desc, 0)) {
		fat_unlock();
		return 0;
	}
	res = fat_read_unlocked(desc, buf, size);
	fat_unlock();
	return res;
}

static size_t fat_write(struct file_desc *desc, void *buf, size_t size) {
	size_t res;

	fat_lock();
	if (fat_no_priv(desc, 1)) {
		fat_unlock();
		return 0;
	}
	res = fat_write_unlocked(desc, buf, size);
	fat_unlock();
	return res;
}

struct file_operations fat_fops = {
	.read = fat_read,
	.write = fat_write,
};
