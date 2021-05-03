/**
 * @file
 *
 * @date Apr 30, 2021
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_INPUT_PL050_PL050_H_
#define SRC_DRIVERS_INPUT_PL050_PL050_H_

#include <stdint.h>

struct pl050 {
	uint32_t cr;     /* control register (rw) */
	uint32_t stat;   /* status register (r) */
	uint32_t data;   /* data register (rw) */
	uint32_t clkdiv; /* clock divisor register (rw) */
	uint32_t ir;     /* interrupt control register (r) */
};

extern int pl050_init(struct pl050 *dev);

#endif /* SRC_DRIVERS_INPUT_PL050_PL050_H_ */
