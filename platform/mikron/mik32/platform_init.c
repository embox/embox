/**
 * @file
 * @brief
 *
 * @author Anton Bondarev
 * @date   09.07.2024
 */

#include <assert.h>
#include <stdint.h>

#include <hal/platform.h>
#include <hal/system.h>

#include <drivers/clk/mikron_pm.h>
#include <drivers/clk/mikron_rcc.h>

void platform_init() {
    
    mikron_wakeup_init(PCC_OSCILLATORTYPE_OSC32M | PCC_OSCILLATORTYPE_OSC32K);

    mikron_pm_init();
    
}
