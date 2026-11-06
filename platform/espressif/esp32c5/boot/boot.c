/*
 * Minimal ESP32-C5 SPI boot. Register definitions and ROM entry points:
 * ESP-IDF components/soc/esp32c5 and components/esp_rom/esp32c5.
 * Keep the ROM-selected flash mode and clock for the first bring-up.
 */

#include <stdint.h>

/* The standalone linker script places the loader in boot SRAM. */
#define REG32(addr) (*(volatile uint32_t *)(uintptr_t)(addr))

#define LP_WDT_BASE      0x600b1c00U
#define LP_WDT_KEY       0x50d83aa1U
#define TG0_BASE         0x60008000U
#define TG_WDT_CONFIG0   0x48U
#define TG_WDT_WPROTECT  0x64U
#define TG_WDT_KEY       0x50d83aa1U
#define TG_WDT_EN        (1U << 31)
#define TG_WDT_FLASHBOOT (1U << 14)
#define CACHE_CTRL       0x600c8004U
#define FLASH_VADDR      0x42000000U
#define FLASH_OFFSET     0x00010000U
#define MMU_PAGE_SIZE    0x00010000U
#define FLASH_SIZE       0x01600000U

/* Bound by boot.lds.S; these calls always execute from mask ROM. */
extern int esp32c5_rom_printf(const char *fmt, ...);
extern void esp32c5_rom_cache_init(void);
extern uint32_t esp32c5_rom_cache_disable(void);
extern void esp32c5_rom_cache_enable(uint32_t autoload);
extern int esp32c5_rom_mmu_set(uint32_t sensitive, uint32_t ext_ram, uint32_t vaddr,
    uint32_t paddr, uint32_t page_kb, uint32_t pages, uint32_t fixed);

static const char boot_message[] = "\r\nEmbox ESP32-C5: SRAM boot\r\n";
static const char map_message[] = "Embox ESP32-C5: flash mapped, entering "
                                  "_start\r\n";
static const char fail_message[] = "Embox ESP32-C5: MMU setup failed (%d)\r\n";

void __attribute__((noreturn)) esp32c5_boot(void) {
	int ret;

	/* Disable the ROM boot watchdog; leave the super watchdog auto-fed. */
	REG32(LP_WDT_BASE + 0x18) = LP_WDT_KEY;
	REG32(LP_WDT_BASE) &= ~((1U << 31) | (1U << 12));
	REG32(LP_WDT_BASE + 0x18) = 0;
	REG32(LP_WDT_BASE + 0x20) = LP_WDT_KEY;
	REG32(LP_WDT_BASE + 0x1c) |= 1U << 18;
	REG32(LP_WDT_BASE + 0x20) = 0;

	/* TG0 flash-boot watchdog is independent of the LP watchdog above.
	 * Embox does not service it; disable both normal and flash-boot modes. */
	REG32(TG0_BASE + TG_WDT_WPROTECT) = TG_WDT_KEY;
	REG32(TG0_BASE + TG_WDT_CONFIG0) &= ~(TG_WDT_EN | TG_WDT_FLASHBOOT);
	REG32(TG0_BASE + TG_WDT_WPROTECT) = 0;

	esp32c5_rom_printf(boot_message);

	esp32c5_rom_cache_init();
	esp32c5_rom_cache_disable();
	ret = esp32c5_rom_mmu_set(0, 0, FLASH_VADDR, FLASH_OFFSET,
	    MMU_PAGE_SIZE / 1024, FLASH_SIZE / MMU_PAGE_SIZE, 0);
	if (ret) {
		esp32c5_rom_printf(fail_message, ret);
		for (;;) {
			__asm__ volatile("wfi");
		}
	}

	/* C5 has one cache and a shared instruction/data address window. */
	REG32(CACHE_CTRL) &= ~3U;
	esp32c5_rom_cache_enable(0);
	__asm__ volatile("fence.i" ::: "memory");
	esp32c5_rom_printf(map_message);

	/* Fixed image ABI: _start is at FLASH_VADDR. It installs its own stack
	 * before reclaiming any loader/ROM SRAM. No ROM calls after this point. */
	__asm__ volatile("jr %0" ::"r"(FLASH_VADDR) : "memory");
	__builtin_unreachable();
}
