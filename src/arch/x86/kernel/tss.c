/**
 * @file
 * @brief
 *
 * @date 04.02.2013
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <string.h>

#include <asm/tss.h>
#include <asm/gdt.h>

#include <kernel/thread.h>

tss_entry_t tss_entry;

EMBOX_UNIT_INIT(tss_init);

static inline void tss_fill(void) {
	uint32_t base;
	uint32_t limit;

	/* Firstly, let's compute the base and limit of our entry into the GDT. */
	base = (uint32_t) &tss_entry;
	limit = base + sizeof(tss_entry);

	/* Now, add our TSS descriptor's address to the GDT. */
	gdt_set_gate(GDT_ENTRY_TSS, base, limit, 0xE9, 0x00);

	/* Ensure the descriptor is initially zero. */
	memset(&tss_entry, 0, sizeof(tss_entry));

	tss_entry.ss0  = __KERNEL_DS;  /* Set the kernel stack segment. */
	tss_entry.esp0 = 0;            /* Set the kernel stack pointer. */

	/*
	 * Here we set the cs, ss, ds, es, fs and gs entries in the TSS. These
	 * specify what segments should be loaded when the processor switches to
	 * kernel mode. Therefore they are just our normal kernel code/data
	 * segments, but with the last two bits set. The setting of these bits
	 * sets the RPL (requested privilege level) to 3, meaning that this TSS
	 * can be used to switch to kernel mode from ring 3.
	 */
	tss_entry.cs = __KERNEL_CS | 0x3;
	tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = __KERNEL_DS | 0x3;
}

static void tss_flush(void) {
	__asm__ __volatile__ ("ltr %%ax": : "a" __TSS);
}

static int tss_init(void) {
	tss_fill();
	tss_flush();

	return 0;
}

void tss_set_kernel_stack(void) {
	struct thread *t = thread_self();

	/*
	 * NOTE: stack and stack_sz of bootstrap thread equals 0, and we
	 *       consider that it isn't executed in usermode.
	 */
	tss_entry.esp0 = (uint32_t)(thread_stack_get(t) + thread_stack_get_size(t));
}
