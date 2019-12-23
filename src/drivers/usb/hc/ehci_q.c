/**
 * @file
 *
 * @date Aug 13, 2019
 * @author Anton Bondrev
 */
#include <util/log.h>

#include <stdint.h>

#include <drivers/usb/usb.h>

#include "ehci.h"

/* fill a qtd, returning how much of the buffer we were able to queue up */
int ehci_qtd_fill(struct ehci_hcd *ehci, struct ehci_qtd_hw *qtd, uintptr_t buf, size_t len, int token, int maxpacket) {
	int i, count;
	uint64_t addr = buf;

	assert(qtd);
	/* EHCI 3.5 */
	assert(len <= (5 * 4096));

	/* one buffer entry per 4K ... first might be short or unaligned */
	qtd->hw_buf[0] = (uint32_t) addr;
	qtd->hw_buf_hi[0] = (uint32_t) (addr >> 32);
	count = 0x1000 - (buf & 0x0fff);/* rest of that page */
	if (len < count) { /* ... iff needed */
		count = len;
	} else {
		buf +=  0x1000;
		buf &= ~0x0fff;

		/* per-qtd limit: from 16K to 20K (best alignment) */
		for (i = 1; count < len && i < 5; i++) {
			addr = buf;
			qtd->hw_buf[i] = (uint32_t) addr;
			qtd->hw_buf_hi[i] = (uint32_t) (addr >> 32);
			buf += 0x1000;
			if ((count + 0x1000) < len) {
				count += 0x1000;
			} else {
				count = len;
			}
		}

		/* short packets may only terminate transfers */
		if (count != len) {
			count -= (count % maxpacket);
		}
	}
	qtd->hw_token = (uint32_t) ((count << 16) | token);
	//qtd->length = count;

	return count;
}

static unsigned
qh_completions (struct ehci_hcd *ehci, struct ehci_qh *qh) {
	struct usb_request *req;
	struct ehci_qh_hw *hw;
	struct ehci_qtd_hw *qtd;
	uintptr_t hw_next;

	req = qh->req;

	ehci->async->qh_next.qh = NULL;

	hw = qh->hw;
	hw->hw_token |= EHCI_QTD_STS_HALT;

	log_debug("qh_completions:\n");
	log_debug("  List of qtds:\n");

	qtd = qh->qdt;
	while (1) {
		log_debug("   qtd=%p, buf=0x%x, token=0x%x, next=0x%x\n",
			qtd, qtd->hw_buf, qtd->hw_token, qtd->hw_next);
		hw_next = qtd->hw_next;
		ehci_qtd_free(ehci, qtd);

		if (hw_next & 0x1) {
			break;
		}

		qtd = (struct ehci_qtd_hw *) hw_next;
	}
	log_debug("\n");

	usb_request_complete(req);

	return 0;
}

void scan_async (struct ehci_hcd *ehci) {
	struct ehci_qh *qh;
	struct ehci_qh *qh_scan_next;

	qh_scan_next = ehci->async->qh_next.qh;

	while (qh_scan_next) {
		qh = qh_scan_next;
		qh_scan_next = qh->qh_next.qh;
		qh_completions(ehci, qh);
	}
}
