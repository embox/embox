/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.10.25
 */

#ifndef DRIVERS_CLK_H_
#define DRIVERS_CLK_H_

#include <module/embox/driver/clk/core.h>

extern int clk_enable(char *clk_name);

extern int clk_disable(char *clk_name);

extern int clk_altfunc_enable(int port, int pin_num, int func);

#endif /* DRIVERS_CLK_H_ */
