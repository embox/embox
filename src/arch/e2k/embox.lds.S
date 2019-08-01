#include <asm-generic/embox.lds.h>

OUTPUT_FORMAT("elf64-e2k", "elf64-e2k", "elf64-e2k")
OUTPUT_ARCH(e3s)
ENTRY(_start)

MEMORY {
	#include <regions.lds.h>
}

_ram_base = ORIGIN(RAM);
_ram_size = LENGTH(RAM);

SECTIONS {
	.bootinfo : {
		_bootinfo_start = .;
		/* . = 0x1000000 - 0x10000; */
		/* 0x00FF0000 */
		*(.x86_boot)
		. = _bootinfo_start + 0x10000;
		_bootinfo_end = .;
	}

	.text : {
		_start = .;
		 _t_entry = .;
		/* Interrupt handler */
		*(.ttable_entry0)
		. = _t_entry + 0x800;
		/* Syscall handler */
		*(.ttable_entry1)
		. = _t_entry + 0x1000;
		/* longjmp handler */
		*(.ttable_entry2)
		. = _t_entry + 0x1800;
		_t_entry_end = .;

		*(.e2k_entry)
		*(.cpu_idle)

		/* text */
	} SECTION_REGION(text)
}
