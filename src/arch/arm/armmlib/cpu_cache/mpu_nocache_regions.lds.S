#include <arm/cpu_cache.h>
#include <framework/mod/options.h>

#define SRAM_NOCACHE_SIZE OPTION_GET(NUMBER, sram_nocache_section_size)

SECTIONS {
	/* MPU region must be aligned to its own size */
	.bss.sram_nocache : ALIGN(SRAM_NOCACHE_SIZE) {
		_sram_nocache_start = .;
		_sram_device_mem_start = .;
		*(.bss.sram_device_mem*)
		_sram_device_mem_stop = .;
		*(.bss.sram_nocache*)
		. = ALIGN(SRAM_NOCACHE_SIZE);
		_sram_nocache_end = .;

		/* Each MPU region size must be always aligned to power of 2 */
		ASSERT((0 == SRAM_NOCACHE_SIZE) || \
	           (SRAM_NOCACHE_SIZE == (1 << LOG2CEIL(SRAM_NOCACHE_SIZE))),
		       "Error: sram_nocache_section_size is not a power of 2.
		        Please change it in mods.conf");

		/* This oveflow check is used due to non-usual section alignment to its
		 * own size. */
		ASSERT((0 == _sram_nocache_size) || (SRAM_NOCACHE_SIZE == _sram_nocache_size),
		       "Error: sram_nocache_section_size overflow.
		        Please increase it in mods.conf");
	}
	_sram_nocache_size = _sram_nocache_end - _sram_nocache_start;
	_sram_nocache_log_size = LOG2CEIL(_sram_nocache_size);
}
