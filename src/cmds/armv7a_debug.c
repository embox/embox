/**
 * @file
 * @brief
 *
 * @author  Alex Kalmuk
 * @date    03.05.2017
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <asm/cp14.h>
#include <asm/cp15.h>
#include <asm/hal/reg.h>

#define TIMEOUT 0xffffff

extern uint32_t GLOBAL_LR1;
extern uint32_t GLOBAL_SP1;
extern uint32_t GLOBAL_CPSR1;
extern uint32_t GLOBAL_SPSR1;

extern uint32_t GLOBAL_LR2;
extern uint32_t GLOBAL_SP2;
extern uint32_t GLOBAL_CPSR2;
extern uint32_t GLOBAL_SPSR2;

void _test_func(void) {
	printf("This is a test function\n");
}

void _reg_dump(char *s) {
	printf("-------------------------\n");
	printf("ARM debug registers %s\n", s);

	printf("GLOBAL_LR1 = %x\n", GLOBAL_LR1);
	printf("GLOBAL_SP1 = %x\n", GLOBAL_SP1);
	printf("GLOBAL_CPSR1 = %x\n", GLOBAL_CPSR1);
	printf("GLOBAL_SPSR1 = %x\n", GLOBAL_SPSR1);

	printf("\n");

	printf("GLOBAL_LR2 = %x\n", GLOBAL_LR2);
	printf("GLOBAL_SP2 = %x\n", GLOBAL_SP2);
	printf("GLOBAL_CPSR2 = %x\n", GLOBAL_CPSR2);
	printf("GLOBAL_SPSR2 = %x\n", GLOBAL_SPSR2);


	printf("SCR  = 0x%08x\n", cp15_get_scr());
	printf("SDER = 0x%08x\n", cp15_get_sder());

	printf("DIDR = 0x%08x\n", cp14_get_dbgdidr());
	printf("DSCR = 0x%08x\n", cp14_get_dbgdscr());
	printf("BVR  = 0x%08x\n", cp14_get_dbgbvr(0));
	printf("BCR  = 0x%08x\n", cp14_get_dbgbcr(0));

	printf("-------------------------\n");
}

extern void exit_monitor_mode(void);

int main(int argc, char *argv[]) {
	uint32_t val;

	val = get_cpsr();
	printf("cpsr = %x\n", val);

	__asm__ __volatile__ (
		"dmb\n\t"
		"isb\n\t"
		"SMC #0" : : :);

	_reg_dump("before");

	val = 1 << 14;
	cp14_set_dbgdscr(val);

	val = TIMEOUT;
	printf("wait..\n");
	while(val--);

	cp14_set_dbgbvr((uint32_t) _test_func, 0);

	cp14_set_dbgbcr((1 << 0) | (0 << 20), 0);


	val = TIMEOUT;
	printf("wait..\n");
	while(val--);

	_test_func(); /* This should trigger the breakpoint */

	_reg_dump("after");



	return 0;
}
