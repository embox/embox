/**
 * @file
 *
 * @date Jul 25, 2014
 * @author: Anton Bondarev
 */
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/mmap.h>
#include <mem/mmap.h>
#include <mem/vmem.h>

static void print_usage(void) {
	printf("Usage: mmutrans [-h] virt_addr [-p pid] [-t]\n");
}

static int mmu_translate(pid_t pid, uintptr_t virt_addr) {
	struct emmap *emmap;
	uintptr_t paddr;
	int i;
	struct mmu_translate_info mmu_translate_info;

	if (pid != 0) {
		return 0;
	}

	emmap = task_resource_mmap(task_kernel_task());
	paddr = vmem_translate(emmap->ctx, virt_addr, &mmu_translate_info);

	printf("%" PRIxPTR " -> %" PRIxPTR "\n", virt_addr, paddr);
	printf("ctx (%" PRIxPTR ") \n", (uintptr_t)mmu_translate_info.ctx);
	printf("pgd (root_table): %" PRIxPTR " idx: %d entry (0x%" PRIxPTR ")\n",
			(uintptr_t) mmu_translate_info.mmu_entry.table[0],
			mmu_translate_info.mmu_entry.idx[0],
			mmu_translate_info.mmu_entry.entries[0]);
	for (i = 1; i < MMU_LEVELS; i ++) {
		printf("table_%d: %" PRIxPTR " idx: %d entry (0x%" PRIxPTR ")\n",
			i,
			(uintptr_t) mmu_translate_info.mmu_entry.table[i],
			mmu_translate_info.mmu_entry.idx[i],
			mmu_translate_info.mmu_entry.entries[i] );
	}
	printf("pte: %" PRIxPTR "\n", (uintptr_t)mmu_translate_info.pte);
	return 0;
}

int main(int argc, char **argv) {
	int opt;
	pid_t pid = 0;

	uintptr_t virt_addr;
	uintptr_t table_addr;

	if (argc < 2) {
		print_usage();
		return 0;
	}
	virt_addr = strtoul(argv[1], NULL, 0);
	while (-1 != (opt = getopt(argc, argv, "hp:t"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'p':
			/* pid if pid = 0 it's the kernel task */
			pid = strtoul(optarg, NULL, 0);
			break;
		case 't':
			table_addr = (uintptr_t)mmu_get_root(pid);
			printf("table address: 0x%" PRIXPTR "\n", table_addr);
			return 0;
		default:
			break;
		}
	}

	mmu_translate(pid, virt_addr);
	return 0;
}
