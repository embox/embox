/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    14.09.2012
 */

#include <hal/arch.h>
#include <hal/reg.h>
#include <compiler.h>

#define SYSKEY 0xBF80F230

#define OSCCON 0xBF80F000

#define SYSKEY1 0xAA996655
#define SYSKEY2 0x556699AA

extern void __blink(void);
extern void __delay(int cnt);

void mips_delay(int c) {
	volatile int cnt = c;
	while (--cnt);
}

void arch_init(void) {
	REG_STORE(SYSKEY, 0);
	REG_STORE(SYSKEY, SYSKEY1);
	REG_STORE(SYSKEY, SYSKEY2);

	REG_ANDIN(OSCCON, ~(0x3 << 19));
	/*REG_ORIN(OSCCON, 0x2 << 19);*/


	REG_STORE(SYSKEY, 0);
	mips_delay(1000000);

}

void arch_idle(void) {

}

void _NORETURN arch_shutdown(arch_shutdown_mode_t mode) {
	while (1) {}
}
