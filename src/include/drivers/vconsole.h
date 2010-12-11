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

	uint32_t cl_cnt;
	uint32_t cl_cur;
	uint8_t cl_buff[TTY_RXBUFF_SIZE];
} vconsole_t;

extern vconsole_t const *sys_console;
extern vconsole_t *cur_console;

extern int vconsole_activate( vconsole_t *con );
extern int vconsole_deactivate( vconsole_t *con );

#endif /* VCONSOLE_H_ */
