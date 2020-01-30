#include "hps_0.h"
#include "led.h"

#include <hal/reg.h>

#define alt_write_word(h2p_lw_led_addr, mask) \
	REG32_STORE(h2p_lw_led_addr, mask)

static const uintptr_t h2p_lw_led_addr = ALT_LWFPGASLVS_OFST + LED_PIO_BASE;

/* For every LED 1 means "light", 0 means "no light" */
void LEDR_LightCount(unsigned char cnt){
	uint32_t mask = 0xff >> (8 - cnt);
	alt_write_word(h2p_lw_led_addr, mask );
}

void LEDR_OffCount(unsigned char cnt){
	uint32_t mask = 0xff >> cnt;

	alt_write_word(h2p_lw_led_addr, mask );
}

void LEDR_AllOn(void) {
	alt_write_word(h2p_lw_led_addr, 0xff);
}

void LEDR_AllOff(void) {
	alt_write_word(h2p_lw_led_addr, 0x00);
}
