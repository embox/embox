/**
 * @file
 * @brief
 *
 * @author  Vadim Deryabkin
 * @date    27.01.2020
 */

#include <assert.h>
#include <stdint.h>

#include <hal/platform.h>
#include <hal/system.h>

#include <drivers/clk/mikron_pm.h>

void platform_init() {
    mikron_pm_init();
}
