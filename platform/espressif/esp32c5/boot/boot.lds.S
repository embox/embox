/* The packer reserves the upper 4 KiB of BOOT for a temporary stack. */
__esp32c5_boot_stack_top = 0x40849000;

/* ESP-IDF esp32c5.rom.ld ROM ABI, not application implementations. */
esp32c5_rom_printf = 0x40000024;
esp32c5_rom_cache_init = 0x40000644;
esp32c5_rom_cache_disable = 0x400006ac;
esp32c5_rom_cache_enable = 0x400006b0;
esp32c5_rom_mmu_set = 0x400006d4;

