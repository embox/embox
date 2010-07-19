/**
 * @file
 * @author Fedor Burdun
 * @date 19.07.2010
 * @description terminal's driver
 */

#include <driver.h>
#include <kernel/printk.h>
#include <stdio.h>
#include <drivers/iterminal.h>

int iterminal_main( device_t *dev ) {

	int in,out;
	in = ((iterminal_private_t*)(dev->private))->in;
	out = ((iterminal_private_t*)(dev->private))->out;
	while (true) {
		char ch = '\0';
		if (device_read( in , &ch , 1 )>=0) {
			device_write( out , &ch , 1 );
			printk("iterminal: %d\n", (int)ch);
		} else {
		}
	}

	return 0;
}

