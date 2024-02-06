#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>

#include <mem/vmem.h>
#include <mem/phymem.h>
#include <util/binalign.h>
#include <lib/libds/dlist.h>

#include <fs/inode.h>
#include <kernel/task/resource/idesc.h>
#include <fs/inode_operation.h>
#include <fs/file_desc.h>

#include <kernel/task/resource/idesc_table.h>
#include <mem/mmap.h>
#include <mem/misc/pool.h>

#define MODOPS_AMOUNT_SHMEM_OBJS OPTION_GET(NUMBER, amount_shmem_objs)

struct shm {
	struct file_desc fd;
	struct inode inode;
	struct dlist_head lnk;
	char name[NAME_MAX + 1];
};


POOL_DEF(shm_pool, struct shm, MODOPS_AMOUNT_SHMEM_OBJS);

static DLIST_DEFINE(shm_list_d);
struct dlist_head *shm_list = &shm_list_d;

static void * shm_mmap(struct idesc *idesc, void *addr, size_t len, int prot,
			int flags, int fd, off_t off);
static void shm_close(struct idesc *idesc) {} /* Stub function required to pass assert */

static struct idesc_ops shm_d_ops = {
		.close = &shm_close,
		.idesc_mmap = &shm_mmap,
};

static void *shm_get_phy(struct shm *shm) {
	return inode_priv(&shm->inode);
}

static inline size_t shm_get_len(struct shm *shm) {
	return inode_size(&shm->inode);
}

int shm_truncate(struct inode *inode, off_t len) {
	void *phy;
	len = binalign_bound(len, VMEM_PAGE_SIZE);

	phy = phymem_alloc(len / VMEM_PAGE_SIZE);
	if (phy == NULL) {
		return EINVAL;
	}
	inode_priv_set(inode, phy);
	inode_size_set(inode, len);

	return 0;
}


static char *shm_get_name(struct shm *shm) {
	return shm->name;
}

static void shm_free(struct shm *shm) {
	ipl_t sp;

	sp = ipl_save();
	dlist_del_init_entry(shm, lnk);
	pool_free((struct pool*)&shm_pool, shm);
	ipl_restore(sp);
}

static struct inode_operations shm_i_ops = {
	.ino_truncate = &shm_truncate
};

static struct shm *shm_create(const char *name) {
	ipl_t ipl;
	struct shm *shm;
	size_t namelen;

	if ((namelen = strlen(name)) > NAME_MAX) {
		return NULL;
	}

	shm = pool_alloc((struct pool*)&shm_pool);
	if (shm == NULL) {
		return NULL;
	}
	memcpy(shm->name, name, strlen(name) + 1);

	shm->inode.i_ops = &shm_i_ops;
	shm->fd.f_idesc.idesc_ops = &shm_d_ops;
	shm->fd.f_inode = &shm->inode;

	ipl = ipl_save();
	dlist_add_prev_entry(shm, shm_list, lnk);
	ipl_restore(ipl);

	return shm;
}


static void * shm_mmap(struct idesc *idesc, void *addr, size_t len, int prot,
			int flags, int fd, off_t off) {
	struct shm *shm;
	void *phy;

	shm = (struct shm *)idesc;
	phy = shm_get_phy(shm);
	if (phy == NULL) {
		return NULL;
	}
	phy = (unsigned char *)phy + off;

	return mmap_ph(addr, len, prot, flags, fd, off, phy);
}

struct shm * shm_lookup(const char *name) {
	struct shm *shm;

	dlist_foreach_entry_safe(shm, shm_list, lnk) {
		if (strcmp(name, shm_get_name(shm)) == 0) {
			return shm;
		}
	}

	return NULL;
}

//TODO: support oflag and mode
int shm_open(const char *name, int oflag, mode_t mode) {
	int fd = 0;
	struct shm *shm;
	struct idesc_table *it;

	shm = shm_lookup(name);
	if (shm == NULL) {
		if ((shm = shm_create(name)) == NULL) {
			return -1;
		}
	}

	it = task_resource_idesc_table(task_self());
	fd = idesc_table_add(it, (struct idesc *)shm, 0);

	return fd;
}

int shm_unlink(const char *name) {
	struct shm *shm;

	shm = shm_lookup(name);
	if (shm == NULL) {
		return -1;
	}
	shm_free(shm);

	return 0;
}

