/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>
#include <kernel/driver.h>

#ifdef CONFIG_DRIVER_SUBSYSTEM

int fgetc(FILE f) {
	char ch;
	int ret;
	if (!(ret = device_read(f, &ch, sizeof(char)))) {
		return ch;
	} else {
		return -1;
	}
}

int fungetc(FILE f, int ch) {
	return device_write(f, &ch, sizeof(char));
}
#endif
