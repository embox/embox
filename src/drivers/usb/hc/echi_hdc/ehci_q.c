/**
 * @file
 *
 * @date Aug 13, 2019
 * @author Anton Bondrev
 */
#include <util/log.h>

#include <stdint.h>
#include <string.h>

#include <drivers/usb/usb.h>

#include "ehci.h"

void ehci_qtd_show(struct ehci_qtd_hw *qtd, int log_level) {
	if (LOG_LEVEL == LOG_NONE) {
		return;
	}

	if (LOG_LEVEL >= log_level) {
		log_raw(log_level, "\tqtd=%p, buf=0x%08x, token=0x%08x, next=0x%08x\n",
			qtd, qtd->hw_buf, qtd->hw_token, qtd->hw_next);
	}
}

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

	return count;
}

extern void dcache_inval(const void *p, size_t size);

static void async_handle_next(struct ehci_hcd *ehci) {
	struct ehci_req *ehci_req;

	if (!usb_queue_empty(&ehci->req_queue)) {
		ehci_req = member_cast_out(usb_queue_first(&ehci->req_queue),
			struct ehci_req, req_link);
		ehci_submit_async(ehci, ehci_req);
	}
}

static unsigned
qh_completions (struct ehci_hcd *ehci, struct ehci_qh *qh) {
	struct usb_request *req;
	struct ehci_qh_hw *hw;
	struct ehci_qtd_hw *qtd;
	uintptr_t hw_next;
	int nontransferred;

	req = qh->ehci_req->req;

	hw = qh->hw;
	hw->hw_token |= EHCI_QTD_STS_HALT;

	nontransferred = 0;
	qtd = qh->qdt;
	while (1) {
		ehci_qtd_show(qtd, LOG_DEBUG);

		hw_next = qtd->hw_next;

		nontransferred += (qtd->hw_token >> 16) & 0x7f;

		ehci_qtd_free(ehci, qtd);
		if (hw_next & 0x1) {
			break;
		}

		qtd = (struct ehci_qtd_hw *) hw_next;
	}
	if (nontransferred) {
		log_error("nontransferred = %d", nontransferred);
	}

	if (req->token & USB_TOKEN_IN) {
		//dcache_inval(req->buf, req->len);
		memcpy(req->buf, req->buffer, req->len);
	}

	req->req_stat = USB_REQ_NOERR;

	usb_queue_del(&ehci->req_queue, &qh->ehci_req->req_link);
	usb_request_complete(req);

	ehci_req_free(ehci, qh->ehci_req);

	/* Handle next request */
	async_handle_next(ehci);

	return 0;
}

void scan_async(struct ehci_hcd *ehci) {
	struct ehci_qh *qh;
	struct ehci_qh *qh_scan_next;
	int i;
	uint32_t command;

	i = 0;
	qh_scan_next = ehci->async->qh_next.qh;
	log_debug("****");
	command = ehci_read(ehci, &ehci->ehci_regs->command);
	ehci_write(ehci, command & ~EHCI_CMD_ASE,  &ehci->ehci_regs->command);

	while (0 != (ehci_read(ehci, &ehci->ehci_regs->status) & EHCI_STS_ASS));

	ehci->async->hw->hw_next = EHCI_QH_NEXT(ehci_hcd, ehci->async->qh_dma);
	ehci->async->hw->hw_info1 = EHCI_QH_HEAD;
	ehci->async->hw->hw_token = EHCI_QTD_STS_HALT;
	ehci->async->hw->hw_qtd_next = EHCI_LIST_END(ehci_hcd);
	ehci->async->hw->hw_alt_next = 0;

	ehci->async->qh_next.qh = NULL;

	while (qh_scan_next) {
		/* TODO Add support for multiple QHs. */
		assert(i++ == 0);
		qh = qh_scan_next;
		qh_scan_next = qh->qh_next.qh;
		qh_completions(ehci, qh);
	}
}

static unsigned
qh_handle_error (struct ehci_hcd *ehci, struct ehci_qh *qh) {
	struct usb_request *req;
	struct ehci_qh_hw *hw;
	struct ehci_qtd_hw *qtd;
	uintptr_t hw_next;

	req = qh->ehci_req->req;

	ehci->async->qh_next.qh = NULL;

	hw = qh->hw;
	hw->hw_token |= EHCI_QTD_STS_HALT;

	log_error("***List of qtds:***");

	qtd = qh->qdt;
	while (1) {
		ehci_qtd_show(qtd, LOG_ERROR);
		hw_next = qtd->hw_next;
		ehci_qtd_free(ehci, qtd);

		if (hw_next & 0x1) {
			break;
		}

		qtd = (struct ehci_qtd_hw *) hw_next;
	}
	log_error("*** endoflist***");

	req->req_stat = USB_REQ_STALL;
	usb_request_complete(req);

	return 0;
}

void handle_async_error (struct ehci_hcd *ehci) {
	struct ehci_qh *qh;
	struct ehci_qh *qh_scan_next;

	qh_scan_next = ehci->async->qh_next.qh;

	while (qh_scan_next) {
		qh = qh_scan_next;
		qh_scan_next = qh->qh_next.qh;
		qh_handle_error(ehci, qh);
	}
}
