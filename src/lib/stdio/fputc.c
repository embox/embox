/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>
#include <kernel/driver.h>

#include <kernel/printk.h> /* only for debug */

#ifdef CONFIG_DRIVER_SUBSYSTEM
int fputc(FILE f, int c) {
#if 0
	printk("try fputc. device_desc: %d, code of char: %d, char: %c\n",f,c,c);
#endif
	char tmp = c;
	return device_write(f, &tmp, sizeof(char));
}
#endif

