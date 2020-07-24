#include <mem/vmem.h>
#include <mem/phymem.h>
#include <util/binalign.h>
#include <struct_shm.h>

void * shm_get_phy(struct shm *shm) {
	return shm->inode.i_data;
}

size_t shm_get_len(struct shm *shm) {
	return shm->inode.length;
}

int shm_truncate(struct inode *inode, size_t len) {
	void *phy;
	len = binalign_bound(len, VMEM_PAGE_SIZE);

	phy = inode->i_data = phymem_alloc(len / VMEM_PAGE_SIZE);
	if (phy == NULL)
		return EINVAL;
	inode->length = len;
	return 0;
}
