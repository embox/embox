/**
 * @file
 *
 * @date 06.04.2012
 * @author Anton Bondarev
 */

#ifndef HAL_SYSTEM_H_
#define HAL_SYSTEM_H_

#include <stdint.h>

#include <framework/mod/options.h>

#include <module/embox/arch/system.h>

typedef uint64_t freq64_t;

#define SYS_CLOCK OPTION_MODULE_GET(embox__arch__system, NUMBER, core_freq)

#endif /* HAL_SYSTEM_H_ */
