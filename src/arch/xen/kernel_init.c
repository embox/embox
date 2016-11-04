/**
 * @file
 * @brief 2nd level boot on Xen
 *
 * @date    04.11.2016
 * @author  Andrey Golikov
 * @author  Anton Kozlov
 */

#include <hal/arch.h>
#include <hal/ipl.h>
#include <drivers/diag.h>
#include <embox/runlevel.h>
#include <kernel/printk.h>

#include <xen/features.h>
#include <stdint.h>
#include <xen/xen.h>
#include <hypercall-x86_32.h>
#include <xen/event_channel.h>
#include <xen/io/console.h>
#include "event.h"

void kernel_start(void);

uint8_t xen_features[XENFEAT_NR_SUBMAPS * 32];

extern shared_info_t shared_info;
extern void handle_input(evtchn_port_t port, struct pt_regs * regs);

shared_info_t *HYPERVISOR_shared_info;
start_info_t * start_info_global;

void kernel_start_xen(start_info_t * start_info) {
	HYPERVISOR_update_va_mapping((unsigned long) &shared_info, 
			__pte(start_info->shared_info | 7),
			UVMF_INVLPG);

	start_info_global = start_info;
	HYPERVISOR_shared_info = &shared_info;
	
	init_events();

	kernel_start();
}
