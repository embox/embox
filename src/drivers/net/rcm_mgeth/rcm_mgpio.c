/**
 * @file
 *
 * @date Apr 3, 2023
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <stdint.h>

#include <hal/reg.h>


void rcm_mgpio_init(uint32_t mgpio_base) {
#define GPIO_SWITCH_SOURCE 0x024
	REG32_STORE(mgpio_base          + GPIO_SWITCH_SOURCE, 0x00000000);
	REG32_STORE(mgpio_base + 0x1000 + GPIO_SWITCH_SOURCE, 0x00000000);
}
