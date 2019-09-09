/**
 * @file
 *
 * @date Aug 13, 2019
 * @author Anton Bondrev
 */
#include <util/log.h>

#include <errno.h>
#include <stdint.h>
#include <sys/mman.h>

#include <kernel/irq.h>

#include <mem/misc/pool.h>

#include <drivers/usb/usb.h>

#include "ehci.h"

POOL_DEF(ehci_hcd_pool, struct ehci_hcd, USB_MAX_HCD);
POOL_DEF(ehci_qh_pool, struct ehci_qh, USB_MAX_ENDP);

static inline struct ehci_qh *endp_to_qh(struct usb_endp *endp) {
	return endp->hci_specific;
}

static inline void udelay(volatile int usec) {
	usec *= 100;
	while (--usec);
}

static int ehci_handshake(struct ehci_hcd *ehci,
		void *ptr, uint32_t mask, uint32_t done, int usec) {
	uint32_t result;

	do {
		result = ehci_read(ehci, ptr);
		if (result == ~(uint32_t)0) {/* card removed */
			return -ENODEV;
		}
		result &= mask;
		if (result == done) {
			return 0;
		}
		udelay(1);
		usec--;
	}while (usec > 0);
	return -ETIMEDOUT;
}

static uint32_t ehci_port_reset(struct ehci_hcd *ehci, int i) {
	uint32_t pstatus;

	pstatus = ehci_read(ehci, &ehci->ehci_regs->port_status[i]);
	ehci_write(ehci, pstatus | EHCI_PORT_POWER, &ehci->ehci_regs->port_status[i]);
	udelay(100);
	ehci_write(ehci, pstatus | EHCI_PORT_RESET, &ehci->ehci_regs->port_status[i]);
	udelay(100);
	ehci_write(ehci, pstatus & ~EHCI_PORT_RESET, &ehci->ehci_regs->port_status[i]);

	udelay(1000);

	pstatus = ehci_read(ehci, &ehci->ehci_regs->port_status[i]);
	if (EHCI_PORT_USB11(pstatus)) {
		ehci_write(ehci, pstatus | EHCI_PORT_OWNER, &ehci->ehci_regs->port_status[i]);
	}
	if (pstatus & EHCI_PORT_PE) {
		log_error("port[%d] enabled", i);
	}

	return pstatus;
}

static void ehci_port_probe(struct ehci_hcd *ehci, int i) {
	uint32_t status;
	status = ehci_read(ehci, &ehci->ehci_regs->port_status[i]);
	if ( !(status & EHCI_PORT_PE) && (status & EHCI_PORT_CONNECT) ) {
		status = ehci_port_reset(ehci, i);
	}
}

/*
 * Reset a non-running (STS_HALT == 1) controller.
 * Must be called with interrupts enabled and the lock not held.
 */
static int ehci_reset(struct ehci_hcd *ehci) {
	int retval;
	uint32_t command = ehci_read(ehci, &ehci->ehci_regs->command);

	command |= EHCI_CMD_RESET;
	log_debug("comand reset %x", command);
	ehci_write(ehci, command, &ehci->ehci_regs->command);
	retval = ehci_handshake(ehci, &ehci->ehci_regs->command,
			EHCI_CMD_RESET, 0, 250 * 10);

	return retval;
}

/*
 * Force HC to halt state from unknown (EHCI spec section 2.3).
 * Must be called with interrupts enabled and the lock not held.
 */
static int ehci_halt (struct ehci_hcd *ehci) {
	uint32_t temp;

	/* disable any irqs left enabled by previous code */
	ehci_write(ehci, 0, &ehci->ehci_regs->intr_enable);

	/*
	 * This routine gets called during probe before ehci->command
	 * has been initialized, so we can't rely on its value.
	 */
	ehci->command &= ~EHCI_CMD_RUN;
	temp = ehci_read(ehci, &ehci->ehci_regs->command);
	temp &= ~(EHCI_CMD_RUN | EHCI_CMD_IAAD);
	ehci_write(ehci, temp, &ehci->ehci_regs->command);

	return ehci_handshake(ehci, &ehci->ehci_regs->status,
			EHCI_STS_HALT, EHCI_STS_HALT, 16 * 125);
}

/* start HC running; it's halted, ehci_init() has been run (once) */
static inline int ehci_run(struct ehci_hcd *ehci) {
	uint32_t hcc_params;

	ehci_write(ehci, 0, &ehci->ehci_regs->frame_index);
	/* EHCI spec section 4.1 */

	ehci_write(ehci, ehci->periodic_dma, &ehci->ehci_regs->frame_list);
	ehci_write(ehci, ehci->async->qh_dma, &ehci->ehci_regs->async_next);

	/*
	 * hcc_params controls whether ehci->regs->segment must (!!!)
	 * be used; it constrains QH/ITD/SITD and QTD locations.
	 * pci_pool consistent memory always uses segment zero.
	 * streaming mappings for I/O buffers, like pci_map_single(),
	 * can return segments above 4GB, if the device allows.
	 *
	 * NOTE:  the dma mask is visible through dev->dma_mask, so
	 * drivers can pass this info along ... like NETIF_F_HIGHDMA,
	 * Scsi_Host.highmem_io, and so forth.  It's readonly to all
	 * host side drivers though.
	 */
	hcc_params = ehci_read(ehci, &ehci->ehci_caps->hcc_params);
	if (EHCI_HCC_64BIT_ADDR(hcc_params)) {
		ehci_write(ehci, 0, &ehci->ehci_regs->segment);
#if 0
// this is deeply broken on almost all architectures
		if (!dma_set_mask(hcd->self.controller, DMA_BIT_MASK(64)))
			ehci_info(ehci, "enabled 64bit DMA\n");
#endif
	} else {
		ehci_write(ehci, 0, &ehci->ehci_regs->segment);
	}


	/* Philips, Intel, and maybe others need CMD_RUN before the
	 root hub will detect new devices (why?); NEC doesn't
	 */
	ehci->command &= ~(EHCI_CMD_LRESET |
			EHCI_CMD_IAAD | EHCI_CMD_PSE | EHCI_CMD_ASE | EHCI_CMD_RESET);
	ehci->command |= EHCI_CMD_RUN;
	ehci_write(ehci, ehci->command, &ehci->ehci_regs->command);
	log_debug("command init %x", ehci->command);

	/*
	 * Start, enabling full USB 2.0 functionality ... usb 1.1 devices
	 * are explicitly handed to companion controller(s), so no TT is
	 * involved with the root hub.  (Except where one is integrated,
	 * and there's no companion controller unless maybe for USB OTG.)
	 *
	 * Turning on the CF flag will transfer ownership of all ports
	 * from the companions to the EHCI controller.  If any of the
	 * companions are in the middle of a port reset at the time, it
	 * could cause trouble.  Write-locking ehci_cf_port_reset_rwsem
	 * guarantees that no resets are in progress.  After we set CF,
	 * a short delay lets the hardware catch up; new resets shouldn't
	 * be started before the port switching actions could complete.
	 */
	ehci_write(ehci, EHCI_CF_DONE, &ehci->ehci_regs->configured_flag);
	ehci_read(ehci, &ehci->ehci_regs->command);	/* unblock posted writes */

	udelay(10000);

	/* Turn On Interrupts */
	ehci_write(ehci, EHCI_INTR_MASK, &ehci->ehci_regs->intr_enable);

	return 0;
}

static int echi_hcd_init(struct ehci_hcd *ehci_hcd) {
	uint32_t hcc_params;
	struct ehci_qh_hw *hw;
	int ret;
	int log2_irq_thresh = 1;
	uint32_t temp;

	ehci_hcd->command = 0;

	ehci_hcd->periodic_size = 512;
	hcc_params = ehci_read(ehci_hcd, &ehci_hcd->ehci_caps->hcc_params);
	if (EHCI_HCC_PGM_FRAMELISTLEN(hcc_params)) {
		/* periodic schedule size can be smaller than default */
		ehci_hcd->periodic_size = 512;
	}
	if ((ret = ehci_mem_init(ehci_hcd)) < 0)
		return ret;

	/* controllers may cache some of the periodic schedule ... */
	if (EHCI_HCC_ISOC_CACHE(hcc_params)) {
		ehci_hcd->i_thresh = 0;
	} else {
		ehci_hcd->i_thresh = 2 + EHCI_HCC_ISOC_THRES(hcc_params);
	}

	/*
	 * dedicate a qh for the async ring head, since we couldn't unlink
	 * a 'real' qh without stopping the async schedule [4.8].  use it
	 * as the 'reclamation list head' too.
	 * its dummy is used in hw_alt_next of many tds, to prevent the qh
	 * from automatically advancing to the next td after short reads.
	 */
	ehci_hcd->async->qh_next.qh = NULL;
	hw = ehci_hcd->async->hw;
	hw->hw_next = EHCI_QH_NEXT(ehci_hcd, ehci_hcd->async->qh_dma);
	hw->hw_info1 = EHCI_QH_HEAD;
	hw->hw_token = EHCI_QTD_STS_HALT;
	hw->hw_qtd_next = EHCI_LIST_END(ehci_hcd);
	hw->hw_alt_next = 0;

	/* clear interrupt enables, set irq latency */
	if (log2_irq_thresh < 0 || log2_irq_thresh > 6) {
		log2_irq_thresh = 0;
	}
	temp = 1 << (16 + log2_irq_thresh);
	if (EHCI_HCC_PER_PORT_CHANGE_EVENT(hcc_params)) {
		ehci_hcd->has_ppcd = 1;
		log_error("enable per-port change event");
		temp |= EHCI_CMD_PPCEE;
	}

	ehci_hcd->command = temp;

	return 0;
}

static void *ehci_hcd_alloc(struct usb_hcd *hcd, void *args) {
	struct ehci_hcd *ehci_hcd;
	int cap_len;
	uint32_t rh_port_n;

	ehci_hcd = pool_alloc(&ehci_hcd_pool);
	if (!ehci_hcd) {
		return NULL;
	}
	ehci_hcd->ehci_caps = args;
	cap_len = EHCI_HC_LENGTH(ehci_hcd->ehci_caps->hc_capbase);
	ehci_hcd->ehci_regs = args + cap_len;

	rh_port_n = EHCI_HCS_N_PORTS(ehci_hcd->ehci_caps->hcs_params);
	hcd->root_hub = usb_hub_alloc(hcd, rh_port_n);

	ehci_hcd->usb_hcd = hcd;

	echi_hcd_init(ehci_hcd);

	return ehci_hcd;
}

static void ehci_hcd_free(struct usb_hcd *hcd, void *spec) {
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);

	assert(ehci == spec);

	pool_free(&ehci_hcd_pool, ehci);
}

static void *ehci_ed_alloc(struct usb_endp *ep) {
	struct ehci_qh *ed;

	ed = ehci_qh_alloc(hcd_to_ehci(ep->dev->hcd));

	return ed;
}

static void ehci_ed_free(struct usb_endp *ep, void *spec) {
	log_error("ep %p", ep);
}

static int ehci_start(struct usb_hcd *hcd) {
	struct ehci_hcd *ehci_hcd;

	ehci_hcd = hcd_to_ehci(hcd);

	ehci_halt(ehci_hcd);
	ehci_reset(ehci_hcd);

	ehci_run(ehci_hcd);

	return 0;
}

static int ehci_stop(struct usb_hcd *hcd) {
	return 0;
}

static int ehci_rh_ctrl(struct usb_hub_port *port, enum usb_hub_request req,
			unsigned short value) {
	struct ehci_hcd *ehci;

	ehci = hcd_to_ehci(port->hub->hcd);

	ehci_port_probe(ehci, port->idx);

	return 0;
}

static void ehci_rh_get_port_stat(struct ehci_hcd *ehci) {
	unsigned i = EHCI_HCS_N_PORTS (ehci_read(ehci, &ehci->ehci_caps->hcs_params));
	struct usb_hub *rhub;

	rhub = ehci_to_hcd(ehci)->root_hub;

	while (i--) {
		int pstatus;
		pstatus = ehci_read(ehci, &ehci->ehci_regs->port_status[i]);

		if (pstatus & EHCI_PORT_CSC) {
			ehci_write(ehci, pstatus & ~EHCI_PORT_CSC, &ehci->ehci_regs->port_status[i]);
			if (!(rhub->ports[i].status & USB_HUB_PORT_ENABLE)) {
				rhub->ports[i].status = USB_HUB_PORT_CONNECT;
				rhub->ports[i].changed = 1;
			}
		}
	}
}

static irq_return_t ehci_irq(unsigned int irq_nr, void *data) {
	struct usb_hcd *hcd;
	struct ehci_hcd *ehci;
	uint32_t status;
	uint32_t masked_status;
	uint32_t cmd;

	hcd = data;
	ehci = hcd_to_ehci(hcd);

	status = ehci_read(ehci, &ehci->ehci_regs->status);

	/* e.g. cardbus physical eject */
	if (status == ~(uint32_t) 0) {
		log_debug("device removed");
		goto dead;
	}

	log_debug("irq status %x", status);
	/*
	 * We don't use STS_FLR, but some controllers don't like it to
	 * remain on, so mask it out along with the other status bits.
	 */
	masked_status = status & (EHCI_INTR_MASK | EHCI_STS_FLR);

	/* clear (just) interrupts */
	ehci_write(ehci, masked_status, &ehci->ehci_regs->status);
	cmd = ehci_read(ehci, &ehci->ehci_regs->command);

	/* complete the unlinking of some qh [4.15.2.3] */
	if (status & EHCI_STS_IAA) {

		/* guard against (alleged) silicon errata */
		if (cmd & EHCI_CMD_IAAD) {
			log_debug("IAA with IAAD still set?");
		}
		scan_async(ehci);
	}

	/* remote wakeup [4.3.1] */
	if (status & EHCI_STS_PCD) {
		ehci_rh_get_port_stat(ehci);

		usb_rh_nofity(hcd);
	}

	if (status & EHCI_STS_FATAL) {
		log_error("PCI error. EHCI halted");
		ehci_write(ehci, cmd & ~(EHCI_CMD_ASE | EHCI_CMD_RUN), &ehci->ehci_regs->command);
		ehci_write(ehci, 0, &ehci->ehci_regs->intr_enable);
	}
	return IRQ_HANDLED;

dead:
	return IRQ_HANDLED;
}

static void echi_qh_insert_async(struct ehci_hcd *ehci, struct ehci_qh *qh) {
	uint32_t command;
	struct ehci_qh *head;

	head = ehci->async;
	qh->qh_next = head->qh_next;
	head->qh_next.qh = qh;

	if (!qh->hw->hw_next) {
		/* splice right after start */
		qh->hw->hw_next = head->hw->hw_next;
		//wmb ();
		head->hw->hw_next = EHCI_QH_NEXT(ehci, qh->hw);
	}
	command = ehci_read(ehci, &ehci->ehci_regs->command); /* unblock posted writes */
	ehci_write(ehci, command | EHCI_CMD_ASE | EHCI_CMD_IAAD,  &ehci->ehci_regs->command);	/* unblock posted writes */
}

static void ehci_qh_sched(struct ehci_hcd *ehci, struct ehci_qh *qh) {
	echi_qh_insert_async(ehci, qh);
}

static void ehci_transfer(struct ehci_qh *qh, uint32_t token, void *buf,
		size_t len, struct usb_request *req) {
	qh->req = req;
}

static void ehci_qh_fill(struct ehci_hcd *ehci,struct usb_request *req,struct ehci_qh *qh, struct ehci_qtd_hw *qtd) {
	struct ehci_qh_hw *hw;
	struct usb_endp *ep;

	ep = req->endp;
	qh->qdt = qtd;

	hw = qh->hw;

	hw->hw_info1 = EHCI_QH_HIGH_SPEED |
			EHCI_QH_TOGGLE_CTL |
			(ep->address << 8) |
			(ep->dev->bus_idx) |
			(ep->max_packet_size << 16); /* max packet len */
	hw->hw_info2 = 1 << 30; /* mult = 1 one transaction */

	hw->hw_qtd_next = (uintptr_t)qtd;
	hw->hw_token &= ~EHCI_QTD_STS_HALT;
}

static int ehci_request(struct usb_request *req) {
	struct ehci_hcd *ehci;
	struct ehci_qh *new_qh;

	uint32_t token;
	int cnt = 0;

	log_debug("token %d req->len %d", req->token, req->len);

	ehci = hcd_to_ehci(req->endp->dev->hcd);

	token = 0;
	if (req->token & USB_TOKEN_SETUP) {
		token = EHCI_QTD_PID_SETUP << 8;
		cnt++;
	}
	if (req->token & USB_TOKEN_IN) {
		/* Use bufferRounding = 1 to read less than was requested if needed. */
		token = EHCI_QTD_PID_IN << 8;
		cnt++;
	}
	if ((req->token & USB_TOKEN_OUT) && !(req->token & USB_TOKEN_SETUP)) {
		token = EHCI_QTD_PID_OUT << 8;
		cnt++;
	}

	assert(cnt == 1);

	{
		struct ehci_qtd_hw *qtd;

		token |= EHCI_QTD_STS_ACTIVE | (3 << 10);
		qtd = ehci_qtd_alloc(ehci);
		ehci_qtd_fill(ehci, qtd, (uintptr_t)req->buf, req->len , token, 0);

		if (req->endp->hci_specific) {
			new_qh = req->endp->hci_specific;
		} else {
			new_qh = ehci_qh_alloc(ehci);
		}
		ehci_qh_fill(ehci, req, new_qh, qtd);
	}

	ehci_transfer(new_qh, token, req->buf, req->len, req);

	ehci_qh_sched(ehci, new_qh);

	return 0;
}

static const struct usb_hcd_ops ehci_hcd_ops = {
	.hcd_hci_alloc = ehci_hcd_alloc,
	.hcd_hci_free = ehci_hcd_free,
	.endp_hci_alloc = ehci_ed_alloc,
	.endp_hci_free = ehci_ed_free,
	.hcd_start = ehci_start,
	.hcd_stop = ehci_stop,
	.rhub_ctrl = ehci_rh_ctrl,
	.request = ehci_request,
};

int ehci_hcd_register(void * base, unsigned int irq) {
	struct usb_hcd *hcd;
	int ret;

	hcd = usb_hcd_alloc((struct usb_hcd_ops *)&ehci_hcd_ops, base);
	if (!hcd) {
		return -ENOMEM;
	}

	ret = irq_attach(irq, ehci_irq, IF_SHARESUP, hcd, "ehci irq");
	if (0 != ret) {
		return ret;
	}

	return usb_hcd_register(hcd);
}
