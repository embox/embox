/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 22.05.23
 */
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <mem/mmap.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/mmap.h>
#include <util/dlist.h>

#include "gdb_priv.h"

void gdb_read_mem(struct gdb_packet *pkt) {
	struct marea *marea;
	struct emmap *emmap;
	char *endptr;
	uintptr_t addr;
	size_t len;

	emmap = task_resource_mmap(task_kernel_task());
	addr = strtoul(&pkt->buf[2], &endptr, 16);

	dlist_foreach_entry(marea, &emmap->marea_list, mmap_link) {
		if ((marea->start <= addr) && (addr < marea->start + marea->size) &&
		    (marea->flags & PROT_WRITE)) {
			len = strtoul(endptr + 1, NULL, 16);
			gdb_pack_mem(pkt, (void *)addr, len);
			return;
		}
	}
	gdb_pack_str(pkt, "E01");
}
