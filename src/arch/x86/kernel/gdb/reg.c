/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 22.05.23
 */
#include <stddef.h>
#include <string.h>

size_t gdb_get_reg(void *regs, unsigned int regnum, void *regval) {
	size_t regsize;

	if (regnum < 10) {
		regsize = 4;
		memcpy(regval, regs + regnum * regsize, regsize);
	}
	else {
		regsize = 0;
	}
	return regsize;
}
