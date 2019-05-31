/**
 * @file
 *
 * @date Oct 5, 2012
 * @author: Anton Bondarev
 */
#include <util/log.h>

#include <stdint.h>
#include <sys/mman.h>
#include <mem/phymem.h>
#include <mem/page.h>
#include <util/binalign.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(phymem_mapping_init);

static int phymem_mapping_init(void) {
	char *const phymem_alloc_start = phymem_allocated_start();
	char *const phymem_alloc_end = phymem_allocated_end();
	const size_t mem_len = phymem_alloc_end - phymem_alloc_start;
	void *va;

	log_boot_start();
	log_boot("start=%p end=%p size=%zu\n", phymem_alloc_start, phymem_alloc_end, mem_len);

	va = mmap_device_memory(phymem_alloc_start,
			mem_len,
			PROT_WRITE | PROT_READ,
			MAP_FIXED,
			(uint64_t)((uintptr_t) phymem_alloc_start));

	log_boot_stop();
	return phymem_alloc_start == va ? 0 : -EIO;
}
