/**
 * @file k210_gpio.c
 * @brief
 * @author sksat <sksat@sksat.net>
 * @version 0.1
 * @date 2020-08-15
 */

#include <assert.h>
#include <util/log.h>

#include <embox/unit.h>
#include <lib/libds/bit.h>
#include <lib/libds/array.h>

#include <drivers/gpio/k210/fpioa.h>
#include <drivers/clk/maix_bit_sysctl.h>
#include <config/board_config.h>

volatile fpioa_t* const fpioa = (volatile fpioa_t*) CONF_FPIOA_PORT_REGION_BASE;

// EMBOX_UNIT_INIT(k210_fpioa_init);

// // TODO: add sysctl driver

// // central clock enable(sysctl: 0x28)
// volatile sysctl_clock_enable_central* const clk_en_cent = (volatile sysctl_clock_enable_central*) CONF_SYSCTL_REGION_BASE + 0x28;
// volatile sysctl_clock_enable_peripheral* const clk_en_peri = (volatile sysctl_clock_enable_peripheral*) CONF_SYSCTL_REGION_BASE + 0x2c;

// static int k210_fpioa_init(void){

// 	// enable bus clock
// 	clk_en_cent->apb0 = 1;

// 	// enable device clock
// 	clk_en_peri->fpioa = 1;

// 	return 0;
// }

void k210_fpioa_set_func_impl(uint8_t num, k210_fpioa_func_t func){
	// TODO: other func
	assert(FN_GPIO0 <= func && func <= FN_GPIO7);

	if(FN_GPIO0 <= func && func <= FN_GPIO7){
		fpioa->io[num] = (const k210_fpioa_io_cfg_t){
			.channel = func,
			.div = 0xf,
			.out_enable = 1,
			.out_inv = 0,
			.data_out = 0,
			.data_inv = 0,
			.pullup = 0,
			.pulldown = 1,		// pull down
			.reserved = 0,
			.slew = 0,
			.in_enable = 1,
			.in_inv = 0,
			.data_in_inv = 0,
			.sch_trigger = 1,
			.reserved1 = 0,
			.pad_di = 0,
		};
	}
}

void k210_fpioa_set_func(uint8_t num, k210_fpioa_func_t func){
	assert(num <= CONF_FPIOA_PORT_WIDTH);
	assert(0 <= func && func < FN_MAX);

	if(func == FN_RESERVED0){
		k210_fpioa_set_func_impl(num, FN_RESERVED0);
		return;
	}

	for(int i=0;i<CONF_FPIOA_PORT_WIDTH;i++){
		if((fpioa->io[i].channel == func) && (i != num)){
			k210_fpioa_set_func_impl(num, FN_RESERVED0);
		}
	}

	k210_fpioa_set_func_impl(num, func);
}

void k210_fpioa_set_pull(uint8_t num, k210_fpioa_pull_t pull){
	// TODO
}
