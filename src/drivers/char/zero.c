/**
 * @file
 * @description /dev/zero char device
 */
#include <dev/char_device.h>

int getc(void) {
	return '0';
}

int putc(int v) {
	return 0;
}

/* ADD_CHAR_DEVICE(DEV_ZERO,getc,putc); */

