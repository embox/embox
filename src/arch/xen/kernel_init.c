/**
 * @file
 * @brief 2nd level boot on Xen
 *
 * @date    04.11.2016
 * @author  Andrey Golikov
 * @author  Anton Kozlov
 */

#include <xen/features.h>
#include <stdint.h>
#include <xen/xen.h>
#include <xen_hypercall-x86_32.h>
#include <xen/event_channel.h>
#include <xen/io/console.h>
#include <xen/event.h>

/* Embox interface */
extern void kernel_start(void);

/* Xen interface */
uint8_t xen_features[XENFEAT_NR_SUBMAPS * 32];

extern shared_info_t xen_shared_info;

shared_info_t *HYPERVISOR_shared_info;
start_info_t * xen_start_info_global;

void xen_kernel_start(start_info_t * start_info) {
	HYPERVISOR_update_va_mapping((unsigned long) &xen_shared_info,
			__pte(start_info->shared_info | 7),
			UVMF_INVLPG);

	xen_start_info_global = start_info;
	HYPERVISOR_shared_info = &xen_shared_info;
	
	init_events();

	kernel_start();
}
