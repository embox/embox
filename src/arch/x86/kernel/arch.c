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
#include <asm/cpu.h>

/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))

/**
 * Check if MAGIC is valid and print the Multiboot information structure
 * pointed by ADDR.
 */
void multiboot_check(unsigned long magic, unsigned long addr) {
	multiboot_info_t *mbi;

	//vga_console_init(80, 25);

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		panic("Invalid magic number: 0x%x\n", (unsigned) magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;
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
		for (i = 0, mod = (module_t *) mbi->mods_addr;
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
		for (mmap = (memory_map_t *) mbi->mmap_addr;
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


void arch_init(void) {
	idt_init();
	gdt_init();
	irq_enable();
}

void arch_idle(void) {
}

void __attribute__ ((noreturn)) arch_shutdown(arch_shutdown_mode_t mode) {
	while (1) {}
}

