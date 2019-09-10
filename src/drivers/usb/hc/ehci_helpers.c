/**
 * @file ehci_helpers.c
 * @brief Helper functions for EHCI (e.g. register dump)
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 10.09.2019
 */
#include <inttypes.h>

#include <drivers/usb/usb.h>
#include <util/log.h>

#include "ehci.h"

void ehci_caps_dump(struct ehci_hcd *ehci) {
	if (ehci == NULL) {
		log_debug("Can't dump registers: ehci is NULL!");
		return;
	}

	if (ehci->ehci_caps == NULL) {
		log_debug("Can't dump registers: ehci_caps is NULL!");
		return;
	}

	log_debug("EHCI caps registers:");
	log_debug("hc_capsbase = %08"PRIX32, ehci->ehci_caps->hc_capbase);
	log_debug("hcs_params  = %08"PRIX32, ehci->ehci_caps->hcs_params);
	log_debug("hcc_params  = %08"PRIX32, ehci->ehci_caps->hcc_params);
	log_debug("portroute   = [%02"PRIX8" %02"PRIX8" %02"PRIX8" %02"PRIX8
			" %02"PRIX8" %02"PRIX8" %02"PRIX8" %02"PRIX8"]",
			ehci->ehci_caps->portroute[0],
			ehci->ehci_caps->portroute[1],
			ehci->ehci_caps->portroute[2],
			ehci->ehci_caps->portroute[3],
			ehci->ehci_caps->portroute[4],
			ehci->ehci_caps->portroute[5],
			ehci->ehci_caps->portroute[6],
			ehci->ehci_caps->portroute[7]);
}

void ehci_regs_dump(struct ehci_hcd *ehci) {
	if (ehci == NULL) {
		log_debug("Can't dump registers: ehci is NULL!");
		return;
	}

	if (ehci->ehci_caps == NULL) {
		log_debug("Can't dump registers: ehci_caps is NULL!");
		return;
	}

	/* Note: reserved fields and port status are not printed */
	log_debug("EHCI operational registers:");
	log_debug("command         = %08"PRIX32, ehci->ehci_regs->command);
	log_debug("status          = %08"PRIX32, ehci->ehci_regs->status);
	log_debug("intr_enable     = %08"PRIX32, ehci->ehci_regs->intr_enable);
	log_debug("frame_index     = %08"PRIX32, ehci->ehci_regs->frame_index);
	log_debug("segment         = %08"PRIX32, ehci->ehci_regs->segment);
	log_debug("frame_list      = %08"PRIX32, ehci->ehci_regs->frame_list);
	log_debug("async_next      = %08"PRIX32, ehci->ehci_regs->async_next);
	log_debug("txfill_tuning   = %08"PRIX32, ehci->ehci_regs->txfill_tuning);
	log_debug("configured_flag = %08"PRIX32, ehci->ehci_regs->configured_flag);
}
