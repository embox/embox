/**
 * @file
 *
 * @date 15.05.2016
 * @author Anton Bondarev
 */
#include <stdint.h>

#include <asm/io.h>

static uint32_t ak4531_base_addr;

void ak4531_write(uint8_t idx, uint8_t data) {
	out16(ak4531_base_addr, (uint16_t)idx << 8 | data);
}

int ak4531_init(uint32_t base_addr) {
	ak4531_base_addr = base_addr;

	return 0;
}
