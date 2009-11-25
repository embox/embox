/**
 * @file mb_irq_ctrl.c
 * @brief Low level functions for interrupt controller
 * @date 23.11.2009
 * @author: Anton Bondarev
 */

#include "autoconf.h"
#include "hal/irq_ctrl.h"

int irqc_init(){
	return 0;
}
int irqc_enable_irq(irq_num_t irq_num){
	return 0;
}

int irqc_disable_irq(irq_num_t irq_num){
	return 0;
}

int irqc_get_status(irq_num_t irq_num){
	return 0;
}
irq_mask_t irqc_set_mask(irq_mask_t mask){
	return 0;
}

irq_mask_t irqc_get_mask(){
	return 0;
}

int irqc_disable_all(){
	return 0;
}

int irqc_force(irq_num_t irq_num){
	return 0;
}

int irqc_clear(irq_num_t irq_num){
	return 0;
}
