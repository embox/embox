/**
 * @file
 * 
 * @author Anton Bondarev
 * @date 04.04.2024
 */

#include <util/log.h>

#include <stdint.h>
#include <string.h>

#include <drivers/common/memory.h>

#include <system_k1921vg015.h>

#include <framework/mod/options.h>

#include <drivers/clk/k1921vg015_rcu.h>

int niiet_gpio_clock_setup(unsigned char port) {
	switch (port) {
		case 0:
			RCU->RCU_CGCFGAHB_reg |= RCU_CGCFGAHB_GPIOAEN;
			RCU->RCU_RSTDISAHB_reg |= RCU_RSTDISAHB_GPIOAEN;
		break;
		case 1:
			RCU->RCU_CGCFGAHB_reg |= RCU_CGCFGAHB_GPIOBEN;
			RCU->RCU_RSTDISAHB_reg |= RCU_RSTDISAHB_GPIOBEN;
		break;
#if defined (GPIOC)
		case 2:
			RCU->RCU_CGCFGAHB_reg |= RCU_CGCFGAHB_GPIOCEN;
			RCU->RCU_RSTDISAHB_reg |= RCU_RSTDISAHB_GPIOCEN;
		break;		
#endif /* defined (PORTC) */
		default:
			return -1;
	}

	return 0;
}