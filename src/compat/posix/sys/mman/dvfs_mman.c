#include <sys/types.h>

#include <mem/vmem.h>
#include <mem/phymem.h>
#include <util/binalign.h>
#include <struct_shm.h>

#include <fs/inode.h>

void * shm_get_phy(struct shm *shm) {
	return inode_priv(&shm->inode);
}

size_t shm_get_len(struct shm *shm) {
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
