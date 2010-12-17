/**
 * @file
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */

#include <hal/arch.h>
#include <asm/multiboot.h>
#include <drivers/vga.h> //workaround
#include <kernel/prom_printf.h>
#include <kernel/panic.h>
#include <stdlib.h>
#include <asm/regs.h>
#include <asm/traps.h>

/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))

/**
 * Check if MAGIC is valid and print the Multiboot information structure
 * pointed by ADDR.
 */
void multiboot_check(unsigned long magic, unsigned long addr) {
	multiboot_info_t *mbi;

	vga_console_init(80, 25);

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		panic("Invalid magic number: 0x%x\n", (unsigned)magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *)addr;
#if 0
	/* Print out the flags.  */
	prom_printf("flags = 0x%x\n", (unsigned)mbi->flags);

	/* Are mem_* valid?  */
	if (CHECK_FLAG(mbi->flags, 0)) {
		prom_printf("mem_lower = %uKB, mem_upper = %uKB\n",
			(unsigned)mbi->mem_lower, (unsigned)mbi->mem_upper);
	}

	/* Is boot_device valid?  */
	if (CHECK_FLAG (mbi->flags, 1)) {
		prom_printf("boot_device = 0x%x\n", (unsigned)mbi->boot_device);
	}

	/* Is the command line passed?  */
	if (CHECK_FLAG (mbi->flags, 2)) {
		prom_printf("cmdline = %s\n", (char *) mbi->cmdline);
	}

	/* Are mods_* valid?  */
	if (CHECK_FLAG(mbi->flags, 3)) {
		module_t *mod;
		int i;

		prom_printf("mods_count = %d, mods_addr = 0x%x\n",
			(int) mbi->mods_count, (int) mbi->mods_addr);
		for (i = 0, mod = (module_t *)mbi->mods_addr;
			i < mbi->mods_count; i++, mod += sizeof(module_t)) {
			prom_printf(" mod_start = 0x%x, mod_end = 0x%x, string = %s\n",
				(unsigned) mod->mod_start,
				(unsigned) mod->mod_end, (char *) mod->string);
		}
	}
#endif
	/* Bits 4 and 5 are mutually exclusive!  */
	if (CHECK_FLAG(mbi->flags, 4) && CHECK_FLAG(mbi->flags, 5)) {
		panic("Both bits 4 and 5 are set.\n");
		return;
	}
#if 0
	/* Is the symbol table of a.out valid?  */
	if (CHECK_FLAG(mbi->flags, 4)) {
		aout_symbol_table_t *aout_sym = &(mbi->u.aout_sym);

		prom_printf("aout_symbol_table: tabsize = 0x%0x, "
			"strsize = 0x%x, addr = 0x%x\n",
			(unsigned) aout_sym->tabsize,
			(unsigned) aout_sym->strsize,
			(unsigned) aout_sym->addr);
	}

	/* Is the section header table of ELF valid?  */
	if (CHECK_FLAG(mbi->flags, 5)) {
		elf_section_header_table_t *elf_sec = &(mbi->u.elf_sec);

		prom_printf("elf_sec: num = %u, size = 0x%x,"
			" addr = 0x%x, shndx = 0x%x\n",
			(unsigned) elf_sec->num, (unsigned) elf_sec->size,
			(unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
        }

	/* Are mmap_* valid?  */
	if (CHECK_FLAG(mbi->flags, 6)) {
		memory_map_t *mmap;

		prom_printf("mmap_addr = 0x%x, mmap_length = 0x%x\n",
			(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		for (mmap = (memory_map_t *)mbi->mmap_addr;
		    (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
			mmap = (memory_map_t *)((unsigned long) mmap
			     + mmap->size + sizeof(mmap->size)))
			prom_printf(" size = 0x%x, base_addr = 0x%x%x,"
				" length = 0x%x%x, type = 0x%x\n",
				(unsigned) mmap->size,
				(unsigned) mmap->base_addr_high,
				(unsigned) mmap->base_addr_low,
				(unsigned) mmap->length_high,
				(unsigned) mmap->length_low,
				(unsigned) mmap->type);
	}
#endif
}

static x86_tss_t         base_tss;
static x86_gate_t        base_idt[IDT_SIZE];
static x86_desc_t        base_gdt[GDT_SIZE];
extern gate_init_entry_t boot_idt_inittab[];

static void      handle_dbf(void);
static char      dbf_stack[2048];
static x86_tss_t dbf_tss = {
	0/*back_link*/,
	0/*esp0*/, 0/*ss0*/, 0/*esp1*/, 0/*ss1*/, 0/*esp2*/, 0/*ss2*/,
	0/*cr3*/,
	(uint32_t) handle_dbf/*eip*/, 0x00000082/*eflags*/,
	0/*eax*/, 0/*ecx*/, 0/*edx*/, 0/*ebx*/,
	(uint32_t) dbf_stack + sizeof(dbf_stack)/*esp*/,
	0/*ebp*/, 0/*esi*/, 0/*edi*/,
	KERNEL_DS/*es*/, KERNEL_CS/*cs*/, KERNEL_DS/*ss*/,
	KERNEL_DS/*ds*/, KERNEL_DS/*fs*/, KERNEL_DS/*gs*/,
	0/*ldt*/, 0/*trace_trap*/, 0x8000/*io_bit_map_offset*/
};

static inline void fill_descriptor(x86_desc_t *desc, uint32_t base,
		uint32_t limit, uint8_t access, uint8_t sizebits) {
	if (limit > 0xfffff) {
		limit >>= 12;
		sizebits |= SZ_G;
	}
	desc->limit_low   = limit & 0xffff;
	desc->base_low    = base & 0xffff;
	desc->base_med    = (base >> 16) & 0xff;
	desc->access      = access | ACC_P;
	desc->limit_high  = limit >> 16;
	desc->granularity = sizebits;
	desc->base_high   = base >> 24;
}

void arch_init(void) {
	gate_init_entry_t *src = boot_idt_inittab;
	ps_desc_t pdesc;
	unsigned vec;
	/* Initialize IDT */
	while (src->entrypoint) {
		vec = src->vector;
		if ((src->type & 0x1f) == 0x05) {
			/* task gate */
			base_idt[vec].offset_low  = 0;
			base_idt[vec].selector    = src->entrypoint;
			base_idt[vec].offset_high = 0;

		} else {
			/* interrupt gate */
			base_idt[vec].offset_low  = src->entrypoint & 0xffff;
			base_idt[vec].selector    = KERNEL_CS;
			base_idt[vec].offset_high = (src->entrypoint >> 16) & 0xffff;
		}
		base_idt[vec].access     = src->type | ACC_P;
		base_idt[vec].word_count = 0;
		src++;
	}
	/* Initialize GDT */
	/* Initialize the base TSS descriptor. */
	fill_descriptor(&base_gdt[BASE_TSS / 8], (uint32_t *) &base_tss,
			sizeof(base_tss) - 1, ACC_PL_K | ACC_TSS, 0);
	/* Initialize the TSS descriptor for the double fault handler */
	fill_descriptor(&base_gdt[DBF_TSS / 8], (uint32_t *) &dbf_tss,
			sizeof(dbf_tss) - 1, ACC_PL_K | ACC_TSS, 0);
	/* Initialize the 32-bit kernel code and data segment descriptors
	   to point to the base of the kernel linear space region. */
	fill_descriptor(&base_gdt[KERNEL_CS / 8], 0, 0xffffffff,
			ACC_PL_K | ACC_CODE_R, SZ_32);
	fill_descriptor(&base_gdt[KERNEL_DS / 8], 0, 0xffffffff,
			ACC_PL_K | ACC_DATA_W, SZ_32);
	/* Load IDT (Interrupt Descriptor Table) */
	pdesc.limit = sizeof(base_idt) - 1;
	pdesc.linear_base = (uint32_t *) &base_idt;
	SET_IDT(&pdesc);
	/* Load GDT (Global Descriptor Table) */
	pdesc.limit = sizeof(base_gdt) - 1;
	pdesc.linear_base = (uint32_t *) &base_gdt;
	SET_GDT(&pdesc);
	__asm__ __volatile__(
		"ljmp  %0,$1f ;  1:"
		: : "i" (KERNEL_CS)
	);
	__asm__ __volatile__(
		"movw %w0,%%ds"
		: : "r" (KERNEL_DS)
	);
	__asm__ __volatile__(
		"movw %w0,%%es"
		: : "r" (KERNEL_DS)
	);
	__asm__ __volatile__(
                "movw %w0,%%ss"
                : : "r" (KERNEL_DS)
        );
        __asm__ __volatile__(
                "movw %w0,%%fs"
                : : "r" (0)
        );
        __asm__ __volatile__(
                "movw %w0,%%gs"
                : : "r" (0)
        );
        /* Load TSS (Task State Segment) */
        base_gdt[BASE_TSS / 8].access &= ~ACC_TSS_BUSY;
        SET_TR(BASE_TSS);
}

void arch_idle(void) {
}

void __attribute__ ((noreturn)) arch_shutdown(arch_shutdown_mode_t mode) {
	while (1) {}
}

void bad_trap_handler(pt_regs_t *st) {
	panic("BAD TRAP!\nEAX=%08x ECX=%08x ECX=%08x EDX=%08x\n",
		st->eax, st->ebx, st->ecx, st->edx);
}

static void handle_dbf() {
	panic("Unexpected DOUBLE FAULT!");
}
