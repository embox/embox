/**
 * @file
 * @brief General Purpose Memory Controller
 *
 * @date 30.10.13
 * @author Alexander Kalmuk
 */

#ifndef DRIVERS_GPMC_H_
#define DRIVERS_GPMC_H_

#include <stdint.h>

extern int gpmc_cs_init(int cs, uint32_t *base, uint32_t size);

#endif /* DRIVERS_GPMC_H_ */
