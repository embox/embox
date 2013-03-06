/**
 * @file
 *
 * @date Mar 6, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <stdio.h>


#include <drivers/uart_device.h>
#include <util/indexator.h>
#include <drivers/tty.h>

INDEX_DEF(serial_indexator, 0, 32);

int serial_register(struct uart_device *dev) {
	int idx;
	char dev_name[] = "ttyS00";

	idx = index_alloc(&serial_indexator, INDEX_ALLOC_MIN);
	if(idx < 0) {
		return -EBUSY;
	}
	sprintf(dev_name, "ttyS%d", idx);

	tty_register(dev_name, &dev->tty, dev->fops);

	return ENOERR;
}

