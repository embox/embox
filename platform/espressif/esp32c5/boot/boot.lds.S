OUTPUT_ARCH(riscv)
ENTRY(esp32c5_boot_entry)

MEMORY {
	BOOT (rwx) : ORIGIN = 0x40840000, LENGTH = 32K
}

__esp32c5_boot_stack_top = 0x40849000;

SECTIONS {
	.text : {
		KEEP(*(.text.boot_entry))
		*(.text .text.*)
		*(.rodata .rodata.* .srodata .srodata.*)
	} > BOOT

	/* The ROM loads code/rodata only; no C runtime initializes RAM. */
	.data : { *(.data .data.* .sdata .sdata.*) } > BOOT
	.bss (NOLOAD) : { *(.bss .bss.* .sbss .sbss.* COMMON) } > BOOT
	ASSERT(SIZEOF(.data) == 0, "bootstrap must not need initialized data")
	ASSERT(SIZEOF(.bss) == 0, "bootstrap must not need BSS initialization")
}

/* ESP-IDF esp32c5.rom.ld ROM ABI, not application implementations. */
esp32c5_rom_printf = 0x40000024;
esp32c5_rom_cache_init = 0x40000644;
esp32c5_rom_cache_disable = 0x400006ac;
esp32c5_rom_cache_enable = 0x400006b0;
esp32c5_rom_mmu_set = 0x400006d4;

