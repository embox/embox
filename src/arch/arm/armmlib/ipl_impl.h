/**
 * @file
 *
 * @date 01.07.10
 * @author Anton Kozlov
 */

#ifndef HAL_IPL_H_
# error "Do not include this file directly!"
#endif /* HAL_IPL_H_ */

#include <stdint.h>
#include <framework/mod/options.h>

#define NVIC_USE_PRIO \
	OPTION_MODULE_GET(embox__arch__arm__armmlib__interrupt, \
		BOOLEAN, nvic_use_prio)

#define NVIC_PRIO_SHIFT \
	OPTION_MODULE_GET(embox__arch__arm__armmlib__interrupt, \
		NUMBER, nvic_prio_shift)

#define NVIC_MIN_PRIO ((0xf << NVIC_PRIO_SHIFT) & 0xff)
#define NVIC_MAX_PRIO ((0x1 << NVIC_PRIO_SHIFT) & 0xff)

typedef uint32_t __ipl_t;

#if NVIC_USE_PRIO
#include "ipl_impl_prio.h"
#else
#include "ipl_impl_non_prio.h"
#endif
