/**
 * @file getauxval.c
 * @brief Stub implementation of getauxval / __getauxval for Embox.
 *
 * GCC 10+ on aarch64-linux-gnu includes lse-init.o in libgcc.a which
 * calls __getauxval() at startup to detect LSE atomic instructions
 * via the ELF auxiliary vector. This symbol is normally provided by
 * glibc and does not exist in Embox.
 *
 * We return 0 for all types, which causes lse-init.o to conservatively
 * assume no LSE support - correct and safe for Embox on any hardware.
 *
 * See: https://github.com/embox/embox/issues/3653
 */

#include <sys/auxv.h>

unsigned long __getauxval(unsigned long type) {
	(void)type;
	return 0UL;
}

unsigned long getauxval(unsigned long type) {
	return __getauxval(type);
}
