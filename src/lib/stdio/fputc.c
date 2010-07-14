/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>
#include <kernel/uart.h>
#include <driver.h>

#ifdef CONFIG_DRIVER_SUBSYSTEM
int fputc(FILE f, int c) {
	return device_write( f , (char*)&c , sizeof(char));
}
#endif

