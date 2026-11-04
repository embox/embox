/**
 * @file
 * @brief ESP32-C5 peripheral clock and reset control.
 *
 * @date Oct 31, 2026
 * @author Dmitrii Piliuk
 */

#ifndef DRIVERS_CLK_ESP32C5_PCR_H_
#define DRIVERS_CLK_ESP32C5_PCR_H_

#include <stdint.h>

#include <drivers/clk/esp32c5_clk.h>

/* Integer divider: 1..256. Configure preserves the clock enable state.
 * The caller must stop transfers before reconfiguring or disabling a clock.
 * Enable is idempotent; there is no reference counting for shared consumers.
 */
extern int esp32_clk_configure(unsigned int id, unsigned int source,
    unsigned int divider);
extern int esp32_clk_enable(unsigned int id);
extern int esp32_clk_disable(unsigned int id);

/* Rates are in Hz. get_rate reports the configured rate, even when gated.
 * Only XTAL, PLL80M and PLL160M are supported; RC_FAST needs calibration.
 */
extern int esp32_clk_get_source_rate(unsigned int source, uint32_t *rate);
extern int esp32_clk_get_rate(unsigned int id, uint32_t *rate);

/* Explicit reset requires an enabled bus clock; waits for READY where present.
 * Reconfiguring or enabling a clock never resets the peripheral.
 */
extern int esp32_periph_reset(unsigned int id);

#endif /* DRIVERS_CLK_ESP32C5_PCR_H_ */
