/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    14.09.2012
 */
#include <hal/arch.h>
#include <hal/reg.h>

#define SYSKEY  0xbf80f230
#define OSCCON  0xbf80f000
#define SYSKEY1 0xaa996655
#define SYSKEY2 0x556699aa

static void __delay(int c) {
	volatile int cnt = c;

	while (--cnt) {}
}

void platform_init(void) {
	REG_STORE(SYSKEY, 0);
	REG_STORE(SYSKEY, SYSKEY1);
	REG_STORE(SYSKEY, SYSKEY2);

	REG_ANDIN(OSCCON, ~(0x3 << 19));
	// REG_ORIN(OSCCON, 0x2 << 19);

	REG_STORE(SYSKEY, 0);

	__delay(1000000);
}
