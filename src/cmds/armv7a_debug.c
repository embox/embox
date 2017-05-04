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

int main(int argc, char *argv[]) {
	uint32_t val;

	val = cp14_get_dbgdidr();
	printf("DIDR = %x\n", val);

	val = cp15_get_scr();
	printf("SCR = %x\n", val);

	val = cp15_get_sder();
	printf("SDER = %x\n", val);

	return 0;
}
