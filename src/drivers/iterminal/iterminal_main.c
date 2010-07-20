/**
 * @file
 * @author Fedor Burdun
 * @date 19.07.2010
 * @brief terminal's driver
 */

#include <kernel/driver.h>
#include <kernel/printk.h>
#include <stdio.h>
#include <drivers/iterminal.h>

#define IN (((iterminal_private_t*)(dev->private))->in)
#define OUT (((iterminal_private_t*)(dev->private))->out)

inline void iterminal_main_init( device_t *dev ) {
	return ;
}

inline device_desc in( device_t *dev ) {
	/* add check changing device description */
	return IN;
	/* if changed run iterminal_main_init */
}

inline device_desc out( device_t *dev ) {
	/* add check changing device description */
	return OUT;
	/* if changed run iterminal_main_init */
}

int iterminal_main( device_t *dev ) {

	printk("iterminal_main\n");

	shell_start();

#if 0
	while (true) {
		char ch = '\0';
		if (device_read( in(dev) , &ch , 1 ) || 1) {
			device_write( out(dev) , &ch , 1 );
			printk("iterminal: %d\n", (int)ch);
		} else {
		}
	}
#endif

	return 0;
}

