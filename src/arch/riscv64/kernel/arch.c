/**
 * @file
 *
 * @date June 17, 2020
 * @author: sksat
 */

#include <hal/arch.h>
#include <compiler.h>

#define UARTHS_ADDR	0x38000000

// Kendryte K210 UARTHS is compatible with SiFive UART
#define TXF_ADDR	(UARTHS_ADDR + 0x00)
#define RXF_ADDR	(UARTHS_ADDR + 0x04)

#define BUSY_LOOP(num)	for(volatile unsigned busy_loop_counter=0;busy_loop_counter<num;busy_loop_counter++){}

void arch_init(void) {
	volatile char *p = (char*)TXF_ADDR;
	while(1){
		while(*p < 0);
		*p = 'A';
//		BUSY_LOOP(100000);
	}
}

void arch_idle(void) {
}

void _NORETURN arch_shutdown(arch_shutdown_mode_t mode) {
	while (1) {}
}
