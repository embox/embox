/**
 * @file
 *
 * @date Jul 1, 2017
 * author Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <fs/index_descriptor.h>
#include <fs/idesc.h>
#include <kernel/task/resource/idesc_table.h>

#include <module/embox/fs/syslib/idesc_mmap_api.h>

void *
idesc_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	struct idesc *idesc;

	if (!idesc_index_valid(fd)
			|| (NULL == (idesc = index_descriptor_get(fd)))
			|| (!(idesc->idesc_amode & S_IROTH))) {
		return SET_ERRNO(EBADF), NULL;
	}

	assert(idesc->idesc_ops);

	if (!idesc->idesc_ops->idesc_mmap) {
		return MAP_FAILED;
	}

	return idesc->idesc_ops->idesc_mmap(idesc, addr, len, prot, flags, fd, off);
}
