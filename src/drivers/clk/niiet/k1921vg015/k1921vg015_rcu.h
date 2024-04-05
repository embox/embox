/**
 * @file
 *
 * @author Anton Bondarev
 *
 * @date 04.04.2024
 */

#ifndef DRIVERS_CLK_NIIET_K1921VG015_H
#define DRIVERS_CLK_NIIET_K1921VG015_H

extern int niiet_gpio_clock_setup(unsigned char port);
extern void niiet_uart_set_rcu(int uart_num);
extern void niiet_sysclk_init(void);

#endif
