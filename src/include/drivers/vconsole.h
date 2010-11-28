/**
 * @file
 *
 * @brief Defines virtual console interface. A virtual console is an instance
 *        of the tty device.
 *
 * @date 23.11.2010
 * @author Anton Bondarev
 */

#ifndef VCONSOLE_H_
#define VCONSOLE_H_

#include <drivers/tty.h>

typedef struct vconsole {
	tty_device_t *tty;
	size_t heigth;
	size_t width;
	uint32_t mode;
	size_t lines;
	size_t colomns;
} vconsole_t;

extern vconsole_t const *sys_console;


#endif /* VCONSOLE_H_ */
