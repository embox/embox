/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    14.08.2013
 */

#ifndef DRIVER_SERIAL_FSNODE_H_
#define DRIVER_SERIAL_FSNODE_H_

struct uart;

extern int serial_register_devfs(struct uart *dev);

#endif /* DRIVER_SERIAL_FSNODE_H_ */

