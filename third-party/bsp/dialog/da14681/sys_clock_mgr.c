/*
 * sys_clock_mgr.c
 *
 *  Created on: May 10, 2020
 *      Author: anton
 */

#include <config/custom_config_qspi.h>

#include <hw_cpm.h>

sys_clk_t cm_sysclk;
ahb_div_t cm_ahbclk;
