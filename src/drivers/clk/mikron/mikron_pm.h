/**
 * @file
 *
 * @author Anton Bondarev
 *
 * @date 08.07.2024
 */

#ifndef DRIVERS_CLK_MICRON_PM_H
#define DRIVERS_CLK_MICRON_PM_H

#include <stdint.h>

extern void mikron_pm_init(void);

extern int clk_enable(char *clk_name);

extern void mikron_pm_set_ahb_div(uint32_t div);
extern void mikron_pm_set_apbm_div(uint32_t div);
extern void mikron_pm_set_apbp_div(uint32_t div);


extern void mikron_pm_set_ahb_force_mux(uint32_t mux_type);
extern void mikron_pm_set_ahb_clk_mux_osc32m(void);
extern void mikron_pm_set_cpu_rtc_clk_mux_osc32k(void);
extern uint32_t mikron_pm_get_freq_status(uint32_t freq_status);
extern int clk_enable(char *clk_name);
extern int mikron_pm_wait_freq_osc32k(void);
extern int mikron_pm_wait_freq_lsi32k(void);
extern int mikron_pm_wait_freq_osc32m(void);


#endif /* DRIVERS_CLK_MICRON_PM_H */
