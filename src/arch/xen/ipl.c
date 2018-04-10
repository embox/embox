/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.03.2018
 */

#include <xen/event.h>
#include <hal/ipl.h>

extern shared_info_t xen_shared_info;

void ipl_init(void) {
	xen_shared_info.vcpu_info[0].evtchn_upcall_mask	= 0;
}

ipl_t ipl_save(void) {
	ipl_t old = xen_shared_info.vcpu_info[0].evtchn_upcall_mask;
	xen_shared_info.vcpu_info[0].evtchn_upcall_mask = 1;
	return old;
}

void ipl_restore(ipl_t ipl) {
	xen_shared_info.vcpu_info[0].evtchn_upcall_mask = ipl;
	/* FIXME handle pending */
}
