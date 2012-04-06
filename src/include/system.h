/**
 * @file
 *
 * @brief
 *
 * @date 06.04.2012
 * @author Anton Bondarev
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <embox/unit.h>
#include <module/embox/arch/system.h>
#define SYS_CLOCK     OPTION_MODULE_GET(embox__arch__system,NUMBER,core_freq)

#endif /* SYSTEM_H_ */
