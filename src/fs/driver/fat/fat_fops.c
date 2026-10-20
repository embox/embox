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
	/* Also checked here, for callers that bypass the locked wrapper. */
	if (fi == NULL) {
		return 0;
	}
	pos = file_get_pos(desc);

	/* fi->cluster is a per-inode cursor kept by the read/write loops;
	 * reposition it when this descriptor is elsewhere. */
	if (pos != fi->pointer) {
		if (fat_seek_cluster(fi, fat_sector_buff, pos) != DFS_OK) {
			return 0;
		}
	}
	fi->pointer = pos;

	rezult = fat_read_file(fi, fat_sector_buff, buf, &bytecount, size);
	/* DFS_EOF still carries the bytes copied before the end of file. */
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
	/* Same cursor as in fat_read_unlocked(). */
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

/* Refuse I/O on a file whose inode has no private data left and record it
 * (see fs/fat_pool_use.h). Such a use is a defect elsewhere; this keeps it
 * from becoming a NULL dereference. */
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

/* The driver's global scratch buffer serialises all of these. The NULL-fi
 * check is under the lock: fat_lock() can block while another thread
 * destroys the inode. */
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
