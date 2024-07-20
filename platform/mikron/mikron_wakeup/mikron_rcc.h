/**
 * @file
 *
 * @author Anton Bondarev
 *
 * @date 08.07.2024
 */

#ifndef DRIVERS_CLK_MICRON_WAKEUP_H
#define DRIVERS_CLK_MICRON_WAKEUP_H

#include <stdint.h>

#define PCC_OSCILLATORTYPE_NONE    0b0000
#define PCC_OSCILLATORTYPE_HSI32M  0b0001
#define PCC_OSCILLATORTYPE_OSC32M  0b0010
#define PCC_OSCILLATORTYPE_LSI32K  0b0100
#define PCC_OSCILLATORTYPE_OSC32K  0b1000
#define PCC_OSCILLATORTYPE_ALL     0b1111

extern void mikron_wakeup_init(uint32_t osc);

#endif /* DRIVERS_CLK_MICRON_WAKEUP_H */
