/**
 * @file
 * @brief Runs start script
 *
 * @date 04.10.11
 * @author Alexander Kalmuk
 */

#include <stdint.h>

#define FCPU 160000000

#define U0TXD_U_ADDR 0x60000818
#define U0TXD_U_PTR ((uint32_t*) U0TXD_U_ADDR)
#define U0TXD_U (*((volatile uint32_t*) U0TXD_U_ADDR))

#define GPIO_OUT_W1TS (*((volatile uint32_t*) 0x60000304))
#define GPIO_OUT_W1TC (*((volatile uint32_t*) 0x60000308))

#define GPIO_ENABLE_W1TS (*((volatile uint32_t*) 0x60000310))
#define GPIO_ENABLE_W1TC (*((volatile uint32_t*) 0x60000314))

static inline void set_function(uint32_t *control_register, uint8_t function) {
	uint32_t old_value = *control_register;
	uint32_t function_mask = ((function & 0b100) << 6) | ((function & 0b11) << 4);
	*control_register = (old_value & ~(0b100110000)) | function_mask;
}

static inline void gpio_enable(uint8_t pin_number) {
	GPIO_ENABLE_W1TS |= 1 << pin_number;
}

static inline void gpio_disable(uint8_t pin_number) {
	GPIO_ENABLE_W1TC |= 1 << pin_number;
}

static inline void gpio_set(uint8_t pin_number) {
	GPIO_OUT_W1TS |= 1 << pin_number;
}

static inline void gpio_clear(uint8_t pin_number) {
	GPIO_OUT_W1TC |= 1 << pin_number;
}

static inline void gpio_init() {
	U0TXD_U = 0b10000000;
	GPIO_OUT_W1TC = 0xFFFF;
	GPIO_ENABLE_W1TC = 0xFFFF;
}

//Be careful! This function is very imprecise
static inline void delay(uint32_t milliseconds) {
	uint32_t nops = ((uint64_t) milliseconds * FCPU) / 30000;
	while (nops > 1) {
		asm volatile ("nop");
		nops--;
	}
}

void blinky_test() {
	gpio_init();
	
	set_function(U0TXD_U_PTR, 0b11);
	
	gpio_enable(1);
	gpio_set(1);
	
	int status = 1;
	while (1) {
		delay(1000);
		if (status) {
			gpio_clear(1);
			status = 0;
		} else {
			gpio_set(1);
			status = 1;
		}
	}
}

int system_start(void) {
	blinky_test();
	return 0;
}
