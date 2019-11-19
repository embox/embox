/*
 * @file
 *
 * @date Aug 29, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_CLK_CCM_IMX6_CCM_IMX6_H_
#define SRC_DRIVERS_CLK_CCM_IMX6_CCM_IMX6_H_

extern int clk_enable(char *clk_name);
extern int clk_disable(char *clk_name);

extern void ccm_analog_usb_init(int port);

#endif /* SRC_DRIVERS_CLK_CCM_IMX6_CCM_IMX6_H_ */
