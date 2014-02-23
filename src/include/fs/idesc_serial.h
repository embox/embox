/**
 * @file
 *
 * @date Dec 18, 2013
 * @author: Anton Bondarev
 */

#ifndef IDESC_SERIAL_H_
#define IDESC_SERIAL_H_

#include <fs/idesc.h>
#include <drivers/uart_device.h>
#include <fs/file_desc.h>
#if 0
struct idesc_serial {
	//struct idesc idesc;
	struct file_desc *fdesc;
	struct uart *uart;
};
#endif
extern struct idesc *idesc_serial_create(struct file_desc *fdesc,
		struct uart *uart, idesc_access_mode_t mod) ;

#endif /* IDESC_SERIAL_H_ */
