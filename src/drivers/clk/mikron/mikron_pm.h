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


#endif /* DRIVERS_CLK_MICRON_PM_H */
