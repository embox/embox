/**
 * @file
 * @brief
 *
 * @author  Vadim Deryabkin
 * @date    07.02.2020
 */

#include <stdint.h>

#include <hal/platform.h>

typedef struct {
	volatile uint32_t CPUID;
	volatile uint32_t ICSR;
	volatile uint32_t RES1;
	volatile uint32_t AIRCR;
	volatile uint32_t SCR;
	volatile uint32_t CCR;
	volatile uint32_t RES2;
	volatile uint32_t SHPR2;
	volatile uint32_t SHPR3;
} scb_struct; // Doc ID 022979 Rev 1.

// Doc ID 022979 Rev 1, 77/91.
#define SCB ((scb_struct *)0xE000ED00)

__attribute__((always_inline)) static inline void dsb() {
	__asm("DSB");
}

void platform_shutdown(shutdown_mode_t mode) {
	switch (mode) {
	case SHUTDOWN_MODE_HALT:
	case SHUTDOWN_MODE_REBOOT:
	case SHUTDOWN_MODE_ABORT:
	default:
		dsb();
	
		// Doc ID 022979 Rev 1, 80/91.
		SCB->AIRCR = (0x5FA << 16) | (1 << 2);
		break;
	}
	
	while (1) {}
}
