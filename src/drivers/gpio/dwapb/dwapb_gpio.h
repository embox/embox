/**
 * @file
 *
 * @date 21.01.2017
 * @author Anton Bondarev
 */

#ifndef DWAPB_GPIO_H_
#define DWAPB_GPIO_H_

#include <stdint.h>

typedef uint32_t __gpio_mask_t;

struct gpio {
	uint32_t port_id;
};


#endif /* DWAPB_GPIO_H_ */
