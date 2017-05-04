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

void _reg_dump(char *s) {
	uint32_t val;

	printf("-------------------------\n");
	printf("DEBUG REGS %s\n", s);

	val = cp14_get_dbgdidr();
	printf("DIDR = %x\n", val);

	val = cp15_get_scr();
	printf("SCR  = 0x%08x\n", val);

	val = cp15_get_sder();
	printf("SDER = 0x%08x\n", val);

	val = cp14_get_dbgbvr(0);
	printf("BVR  = 0x%08x\n", val);

	val = cp14_get_dbgbcr(0);
	printf("BCR  = 0x%08x\n", val);

	printf("-------------------------\n");
}

int main(int argc, char *argv[]) {
	_reg_dump("before");
	cp15_set_sder(3);

	printf("1\n");
	printf("2\n");

	uint32_t val = 1 << 15;
	asm volatile("MCR p14,0,%[val],c0,c2,2" : : [val] "r" (val));

	cp14_set_dbgbvr(0, 0);
	cp14_set_dbgbcr((1 << 0) | (4 << 20), 0);

	_reg_dump("after");



	return 0;
}
