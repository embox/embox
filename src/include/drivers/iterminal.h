/**
 * @file
 * @author Fedor Burdun
 * @date 19.07.2010
 * @description /dev/iterminal
 */

#ifndef __ITERMINAL_H_
#define __ITERMINAL_H_

#include <kernel/driver.h>

#define ITERM_DC_SET_IO		0x0101
#define ITERM_DC_SET_IN		0x0102
#define ITERM_DC_SET_OUT	0x0103
#define ITERM_DC_GET_IN		0x0104
#define ITERM_DC_GET_OUT	0x0105

typedef struct iterminal_private {
	device_desc in,out;
} iterminal_private_t;

#endif /* __ITERMINAL_H_ */
