/**
 * @file
 *
 * @date Apr 3, 2023
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <stdint.h>

#include <hal/reg.h>

#include <drivers/common/memory.h>
#include <framework/mod/options.h>

#define MGPIO0_BASE    OPTION_GET(NUMBER, base_addr)

#define GPIO_SWITCH_SOURCE 0x024

void rcm_mgpio_init(void) {
//	uint32_t mgpio_base = MGPIO0_BASE;

	REG32_STORE(MGPIO0_BASE          + GPIO_SWITCH_SOURCE, 0x00000000);
	REG32_STORE(MGPIO0_BASE + 0x1000 + GPIO_SWITCH_SOURCE, 0x00000000);
}

PERIPH_MEMORY_DEFINE(mgpio0, MGPIO0_BASE, 2 * 0x1000);
