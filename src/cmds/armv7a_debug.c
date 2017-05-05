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

#define TIMEOUT 0xffffff

void _test_func(void) {
	printf("This is a test function\n");
}

void _reg_dump(char *s) {

	printf("-------------------------\n");
	printf("ARM debug registers %s\n", s);

	printf("SCR  = 0x%08x\n", cp15_get_scr());
	printf("SDER = 0x%08x\n", cp15_get_sder());

	printf("DIDR = 0x%08x\n", cp14_get_dbgdidr());
	printf("DSCR = 0x%08x\n", cp14_get_dbgdscr());
	printf("BVR  = 0x%08x\n", cp14_get_dbgbvr(0));
	printf("BCR  = 0x%08x\n", cp14_get_dbgbcr(0));

	printf("-------------------------\n");
}

int main(int argc, char *argv[]) {
	uint32_t val;

	_reg_dump("before");

	cp15_set_sder(3);

	val = cp14_get_dbgdscr();

	val |= (1 >> 13) | (1 << 14);
	val = (val & (~(3 << 20))) | (1 << 20);

	cp14_set_dbgdscr(val);

	val = TIMEOUT;
	printf("wait..\n");
	while(val--);

	cp14_set_dbgbvr((uint32_t) _test_func, 0);
	cp14_set_dbgbcr((0xf << 5) | (1 << 0) | (0 << 20) | 2 | 4 | (1 << 13), 0);

	val = TIMEOUT;
	printf("wait..\n");
	while(val--);

	_test_func(); /* This should trigger the breakpoint */

	_reg_dump("after");



	return 0;
}
