/**
 * @file
 *
 * @date Aug 13, 2019
 * @author Anton Bondrev
 */
#include <util/log.h>

#include <hal/cache.h>
#include <errno.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

#include <kernel/irq.h>
#include <kernel/panic.h>

#include <mem/misc/pool.h>

#include <drivers/usb/usb.h>

#include "ehci.h"

#if !OPTION_MODULE_GET(embox__driver__usb__hc__ehci_hcd,NUMBER,tt_support)
#define tdi_in_host_mode(ehci) (1)
#define tdi_reset(ehci)

#else
#define USBMODE_SDIS    (1<<3) /* Stream disable */
#define USBMODE_BE      (1<<2) /* BE/LE endianness select */
#define USBMODE_CM_HC   (3<<0) /* host controller mode */
#define USBMODE_CM_IDLE (0<<0) /* idle state */

static inline int tdi_in_host_mode(struct ehci_hcd *ehci) {
	uint32_t tmp;

	tmp = ehci_read(ehci, &ehci->ehci_regs->usbmode);
	return (tmp & 3) == USBMODE_CM_HC;
}

static inline void tdi_reset(struct ehci_hcd *ehci) {
	uint32_t tmp;

	tmp = ehci_read(ehci, &ehci->ehci_regs->usbmode);
	tmp |= USBMODE_CM_HC;
#if 0
	/* The default byte access to MMR space is LE after
	 * controller reset. Set the required endian mode
	 * for transfer buffers to match the host microprocessor
	 */
	if (ehci_big_endian_mmio(ehci))
		tmp |= USBMODE_BE;
#endif
	ehci_write(ehci, tmp, &ehci->ehci_regs->usbmode);
}
#endif

POOL_DEF(ehci_hcd_pool, struct ehci_hcd, USB_MAX_HCD);

static int ehci_handshake(struct ehci_hcd *ehci,
		void *ptr, uint32_t mask, uint32_t done, int usec) {
	uint32_t result;

	assert(ehci);

	do {
		result = ehci_read(ehci, ptr);
		if (result == ~(uint32_t)0) {/* card removed */
			log_debug("-ENODEV");
			return -ENODEV;
		}
		log_debug("tick %x", result);
		result &= mask;
		if (result == done) {
			return 0;
		}

		usleep(10);
		usec--;
	}while (usec > 0);

	return -ETIMEDOUT;
}

static int ehci_port_power(struct ehci_hcd *ehci, int portnum, int enable) {
	uint32_t *status_reg = &ehci->ehci_regs->port_status[portnum];
	uint32_t temp = ehci_read(ehci, status_reg) & ~EHCI_PORT_RWC_BITS;

	if (enable) {
		ehci_write(ehci, temp | EHCI_PORT_POWER, status_reg);
	} else {
		ehci_write(ehci, temp & ~EHCI_PORT_POWER, status_reg);
	}

	return 0;
}

static uint32_t ehci_port_reset(struct ehci_hcd *ehci, int i) {
	uint32_t pstatus;

	assert(ehci);
	assert(ehci->ehci_regs);

	pstatus = ehci_read(ehci, &ehci->ehci_regs->port_status[i]);
	ehci_write(ehci, pstatus & ~EHCI_PORT_POWER, &ehci->ehci_regs->port_status[i]);
	usleep(100 * 1000);

	pstatus = ehci_read(ehci, &ehci->ehci_regs->port_status[i]);
	ehci_write(ehci, pstatus | EHCI_PORT_POWER, &ehci->ehci_regs->port_status[i]);
	usleep(100 * 1000);

	pstatus = ehci_read(ehci, &ehci->ehci_regs->port_status[i]);
	log_debug("pstatus before reset = 0x%08x", pstatus);
	ehci_write(ehci, pstatus | EHCI_PORT_RESET, &ehci->ehci_regs->port_status[i]);
	usleep(500 * 1000);
	pstatus = ehci_read(ehci, &ehci->ehci_regs->port_status[i]);
	log_debug("pstatus after reset wait = 0x%08x", pstatus);
	ehci_write(ehci, pstatus & ~EHCI_PORT_RESET, &ehci->ehci_regs->port_status[i]);
	usleep(1 * 1000);

	pstatus = ehci_read(ehci, &ehci->ehci_regs->port_status[i]);
	if (EHCI_PORT_USB11(pstatus)) {
		log_info("Low power device!");
		ehci_write(ehci, pstatus | EHCI_PORT_OWNER, &ehci->ehci_regs->port_status[i]);
	}
	if (pstatus & EHCI_PORT_PE) {
		log_info("port[%d] enabled, pstatus = 0x%08x", i, pstatus);
	}

	log_debug("Port reset finished");

	return pstatus;
}

/*
 * Reset a non-running (STS_HALT == 1) controller.
 * Must be called with interrupts enabled and the lock not held.
 */
static int ehci_reset(struct ehci_hcd *ehci) {
	int retval;
	uint32_t command;

	assert(ehci);
	assert(ehci->ehci_regs);

	command = ehci_read(ehci, &ehci->ehci_regs->command);
	command |= EHCI_CMD_RESET;
	log_debug("command reset %x", command);
	ehci_write(ehci, command, &ehci->ehci_regs->command);
	log_debug("command reset %x", command);
	retval = ehci_handshake(ehci, &ehci->ehci_regs->command,
			EHCI_CMD_RESET, 0, 250 * 10);

	usleep(500 * 1000);

	tdi_reset (ehci);
	log_debug("reset done %x", retval);
	return retval;
}

/*
 * Force HC to halt state from unknown (EHCI spec section 2.3).
 * Must be called with interrupts enabled and the lock not held.
 */
static int ehci_halt(struct ehci_hcd *ehci) {
	uint32_t temp;

	assert(ehci);
	assert(ehci->ehci_regs);

	/* disable any irqs left enabled by previous code */
	ehci_write(ehci, 0, &ehci->ehci_regs->intr_enable);

	if (!tdi_in_host_mode(ehci)) {
		return 0;
	}

	/*
	 * This routine gets called during probe before ehci->command
	 * has been initialized, so we can't rely on its value.
	 */
	ehci->command &= ~EHCI_CMD_RUN;
	temp = ehci_read(ehci, &ehci->ehci_regs->command);
	temp &= ~(EHCI_CMD_RUN | EHCI_CMD_IAAD);
	ehci_write(ehci, temp, &ehci->ehci_regs->command);
	log_debug("command halt %x", temp);
	return ehci_handshake(ehci, &ehci->ehci_regs->status,
			EHCI_STS_HALT, EHCI_STS_HALT, 16 * 125);
}

/* start HC running; it's halted, ehci_init() has been run (once) */
static inline int ehci_run(struct ehci_hcd *ehci) {
	uint32_t hcc_params;

	assert(ehci);
	assert(ehci->ehci_regs);
	assert(ehci->ehci_caps);

	ehci_write(ehci, 0, &ehci->ehci_regs->frame_index);
	/* EHCI spec section 4.1 */

	/* TODO we use only async queue now set EHCI_CMD_PSE
	 ehci_write(ehci, ehci->periodic_dma, &ehci->ehci_regs->frame_list);
	 */
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

	/* TODO we use only async queue now set EHCI_CMD_PSE*/
	/* Philips, Intel, and maybe others need CMD_RUN before the
	 root hub will detect new devices (why?); NEC doesn't
	 */
	ehci->command &= ~(EHCI_CMD_LRESET |
			EHCI_CMD_IAAD |/* EHCI_CMD_PSE | */ /*EHCI_CMD_ASE |*/ EHCI_CMD_RESET);
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
	//ehci_write(ehci, EHCI_CF_DONE, &ehci->ehci_regs->configured_flag);
	//ehci_read(ehci, &ehci->ehci_regs->command);	/* unblock posted writes */

	usleep(500 * 1000);

	/* Turn On Interrupts */
	ehci_write(ehci, EHCI_INTR_MASK, &ehci->ehci_regs->intr_enable);

	ehci_caps_dump(ehci);
	ehci_regs_dump(ehci);

	return 0;
}

static int echi_hcd_init(struct ehci_hcd *ehci_hcd) {
	struct ehci_qh_hw *hw;
	int ret;
	//int log2_irq_thresh = 1;
	//uint32_t temp;

	log_debug("");

	assert(ehci_hcd);
	assert(ehci_hcd->ehci_caps);

	ehci_hcd->command = 0;

	if ((ret = ehci_mem_init(ehci_hcd)) < 0)
		return ret;

	usb_queue_init(&ehci_hcd->req_queue);

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
	//if (log2_irq_thresh < 0 || log2_irq_thresh > 6) {
	//	log2_irq_thresh = 0;
	//}
	//temp = 1 << (16 + log2_irq_thresh);
	//if (EHCI_HCC_PER_PORT_CHANGE_EVENT(hcc_params)) {
	//	ehci_hcd->has_ppcd = 1;
	//	log_error("enable per-port change event");
	//	temp |= EHCI_CMD_PPCEE;
	//}

	//ehci_hcd->command = temp;

	return 0;
}

static void *ehci_hcd_alloc(struct usb_hcd *hcd, void *args) {
	struct ehci_hcd *ehci_hcd;
	int cap_len;

	assert(hcd);
	assert(args);

	ehci_hcd = pool_alloc(&ehci_hcd_pool);
	if (!ehci_hcd) {
		return NULL;
	}
	ehci_hcd->ehci_caps = args;
	cap_len = EHCI_HC_LENGTH(ehci_hcd->ehci_caps->hc_capbase);
	ehci_hcd->ehci_regs = args + cap_len;

	ehci_hcd->usb_hcd = hcd;

	echi_hcd_init(ehci_hcd);

	return ehci_hcd;
}

static void ehci_hcd_free(struct usb_hcd *hcd, void *spec) {
	struct ehci_hcd *ehci;

	assert(hcd);

	ehci = hcd_to_ehci(hcd);

	assert(ehci == spec);

	pool_free(&ehci_hcd_pool, ehci);
}

static void *ehci_ed_alloc(struct usb_endp *ep) {
	struct ehci_qh *ed;

	assert(ep);

	ed = ehci_qh_alloc(hcd_to_ehci(ep->dev->hcd));

	return ed;
}

static void ehci_ed_free(struct usb_endp *ep, void *spec) {
	log_error("ep %p", ep);
}

static int ehci_start(struct usb_hcd *hcd) {
	struct ehci_hcd *ehci_hcd;
	struct usb_dev *udev;
	int i;

	assert(hcd);

	ehci_hcd = hcd_to_ehci(hcd);

	log_debug("Starting EHCI...");
	ehci_caps_dump(ehci_hcd);
	ehci_regs_dump(ehci_hcd);

	ehci_halt(ehci_hcd);
	ehci_reset(ehci_hcd);

	ehci_run(ehci_hcd);
	log_debug("EHCI started!");

	for (i = 0; i < EHCI_HCS_N_PORTS(ehci_hcd->ehci_caps->hcs_params); i++) {
		ehci_port_power(ehci_hcd, i, 1);
	}

	/* Create root hub */
	udev = usb_new_device(NULL, hcd, 0);
	if (!udev) {
		log_error("usb_new_device failed");
		return -1;
	}

	return 0;
}

static int ehci_stop(struct usb_hcd *hcd) {
	return 0;
}

static void ehci_get_hub_descriptor(struct ehci_hcd *ehci,
		struct usb_desc_hub *desc) {
	desc->b_desc_length = 7;
	desc->b_desc_type = USB_DT_HUB;
	desc->b_nbr_ports = EHCI_HCS_N_PORTS(ehci->ehci_caps->hcs_params);
}

static uint32_t ehci_roothub_portstatus(struct ehci_hcd *ehci, unsigned port) {
	uint32_t status;

	status = ehci_read(ehci, &ehci->ehci_regs->port_status[port]);
	log_debug("port status = 0x%08x", status);

	/* set wChange bits */
	if (status & EHCI_PORT_CSC) {
		log_debug("USB_PORT_STAT_CONNECTION");
		status |= USB_PORT_STAT_CONNECTION << 16;
	}
	/* Reset is completed by ehci_port_reset(), so return
	 * claim reset state was changed. */
	status |= USB_PORT_STAT_RESET << 16;
	return status;
}

static int ehci_root_hub_control(struct usb_request *req) {
	struct ehci_hcd *ehci = hcd_to_ehci(req->endp->dev->hcd);
	struct usb_control_header *ctrl = &req->ctrl_header;
	uint32_t type_req;
	uint32_t wval;
	unsigned port;

	type_req = (ctrl->bm_request_type << 8) | ctrl->b_request;

	port = ctrl->w_index - 1;

	switch (type_req) {
	case USB_GET_HUB_DESCRIPTOR:
		ehci_get_hub_descriptor(ehci, (struct usb_desc_hub *) req->buf);
		break;
	case USB_GET_PORT_STATUS:
		*(uint32_t *)req->buf = ehci_roothub_portstatus(ehci, port);
		break;
	case USB_SET_PORT_FEATURE:
		switch (ctrl->w_value) {
		case USB_PORT_FEATURE_RESET:
			ehci_port_reset(ehci, port);
			break;
		case USB_PORT_FEATURE_POWER:
			ehci_port_power(ehci, port, true);
			break;
		default:
			log_error("Unknown port set feature: 0x%x\n", ctrl->w_value);
			return -1;
		}
		break;
	case USB_CLEAR_PORT_FEATURE:
		switch (ctrl->w_value) {
		case USB_PORT_FEATURE_POWER:
			ehci_port_power(ehci, port, false);
			break;
		case USB_PORT_FEATURE_C_CONNECTION:
			wval = ehci_read(ehci, &ehci->ehci_regs->port_status[port]);
			ehci_write(ehci, wval | EHCI_PORT_CSC,
				&ehci->ehci_regs->port_status[port]);
			break;
		case USB_PORT_FEATURE_C_RESET:
			/* do nothing */
			break;
		default:
			log_error("Unknown port clear feature: 0x%x\n", ctrl->w_value);
			return -1;
		}
		break;
	default:
		//panic("ehci_root_hub_control: Unknown req_type=0x%x, request=0x%x\n",
		//	ctrl->bm_request_type, ctrl->b_request);
		break;
	}

	req->req_stat = USB_REQ_NOERR;
	usb_request_complete(req);

	return 0;
}

static irq_return_t ehci_irq(unsigned int irq_nr, void *data) {
	struct usb_hcd *hcd;
	struct ehci_hcd *ehci;
	uint32_t status;
	uint32_t masked_status;
	uint32_t cmd;

	assert(data);

	hcd = data;
	ehci = hcd_to_ehci(hcd);

	assert(ehci->ehci_regs);

	status = ehci_read(ehci, &ehci->ehci_regs->status);

	/* e.g. cardbus physical eject */
	if (status == ~(uint32_t) 0) {
		log_debug("device removed");
		goto dead;
	}

	log_debug("irq status 0x%08x", status);
	/*
	 * We don't use STS_FLR, but some controllers don't like it to
	 * remain on, so mask it out along with the other status bits.
	 */
	masked_status = status & (EHCI_INTR_MASK | EHCI_STS_FLR);

	/* clear (just) interrupts */
	ehci_write(ehci, masked_status, &ehci->ehci_regs->status);
	cmd = ehci_read(ehci, &ehci->ehci_regs->command);

	if (status & EHCI_STS_ERR) {
		log_error("irq error status 0x%08x", status);
		handle_async_error(ehci);
	} else if (status & EHCI_STS_INT) {
		scan_async(ehci);
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
extern void ehci_qtd_show(struct ehci_qtd_hw *qtd, int log_level);
void ehci_asinc_show(struct ehci_hcd *ehci, int log_level) {
	if (LOG_LEVEL == LOG_NONE) {
		return;
	}

	if (LOG_LEVEL >= log_level) {
		struct ehci_qh_hw *qh_hw;
		struct ehci_qh_hw *qh_hw_head;

		qh_hw_head = qh_hw = (void *) (uintptr_t)ehci_read(ehci, &ehci->ehci_regs->async_next);
		log_raw(log_level, "Start asinc list %p:\n", qh_hw_head);
		do {
			struct ehci_qtd_hw *qtd;

			log_raw(log_level, "qh_hw=%p, hw_info1=0x%08x, hw_info2=0x%08x, token=0x%08x, next=0x%08x\n qtd list:\n",
					qh_hw, qh_hw->hw_info1, qh_hw->hw_info2, qh_hw->hw_token, qh_hw->hw_next);

			for (qtd = (void *) (uintptr_t)qh_hw->hw_qtd_next; 0 == (((uintptr_t)qtd) & 0x1); qtd = (void *) (uintptr_t)qtd->hw_next) {
				ehci_qtd_show(qtd, log_level);
			}
			log_raw(log_level, " qdt list end qh_hw=%p\n", qh_hw);
			qh_hw = (void *)(uintptr_t)(qh_hw->hw_next & ~0xF);
		} while (qh_hw_head != qh_hw);
		log_raw(log_level, "End asinc list %p:\n", qh_hw_head);
	}
}

static void echi_qh_insert_async(struct ehci_hcd *ehci, struct ehci_qh *qh) {
	uint32_t command;
	struct ehci_qh *head;

	assert(ehci);
	assert(ehci->ehci_regs);

	log_debug("ehci %p qh %p", ehci, qh);

	head = ehci->async;
	qh->qh_next = head->qh_next;
	head->qh_next.qh = qh;

//	if (!qh->hw->hw_next) {
		/* splice right after start */
		qh->hw->hw_next = head->hw->hw_next;
		//wmb ();
		head->hw->hw_next = EHCI_QH_NEXT(ehci, qh->hw);
//	}

	ehci->async->hw->hw_token &= ~EHCI_QTD_STS_HALT;
	qh->hw->hw_token &= ~EHCI_QTD_STS_HALT;

	ehci_write(ehci, ehci->async->qh_dma, &ehci->ehci_regs->async_next);

	ehci_asinc_show(ehci, LOG_DEBUG);

	command = ehci_read(ehci, &ehci->ehci_regs->command); /* unblock posted writes */
	ehci_write(ehci, command | EHCI_CMD_ASE,  &ehci->ehci_regs->command);	/* unblock posted writes */
}

static void ehci_qh_sched(struct ehci_hcd *ehci, struct ehci_qh *qh) {
	assert(ehci);
	assert(qh);

	echi_qh_insert_async(ehci, qh);
}

static void ehci_qh_fill(struct ehci_hcd *ehci, struct ehci_req *ehci_req,
	    struct ehci_qh *qh, struct ehci_qtd_hw *qtd) {
	struct ehci_qh_hw *hw;
	struct usb_endp *ep;
	struct usb_request *req;

	assert(ehci_req);
	assert(qh);

	req = ehci_req->req;

	ep = req->endp;
	assert(ep);

	qh->ehci_req = ehci_req;
	qh->qdt = qtd;

	hw = qh->hw;
	assert(hw);

	hw->hw_info1 = EHCI_QH_HIGH_SPEED |
			(ep->address << 8) |
			(ep->dev->addr) |
			(ep->max_packet_size << 16); /* max packet len */

	if (ep->type == USB_COMM_CONTROL) {
		hw->hw_info1 |= EHCI_QH_TOGGLE_CTL;
	}

	hw->hw_info2 = 1 << 30; /* mult = 1 one transaction */

	hw->hw_qtd_next = (uintptr_t)qtd;
}

void ehci_submit_async(struct ehci_hcd *ehci, struct ehci_req *ehci_req) {
	struct ehci_qh *endp_qh;
	struct usb_request *req;

	req = ehci_req->req;
	assert(req);

	endp_qh = req->endp->hci_specific;
	assert(endp_qh);

	ehci_qh_fill(ehci, ehci_req, endp_qh, ehci_req->qtds_head);
	ehci_qh_sched(ehci, endp_qh);
}

static int ehci_request(struct usb_request *req) {
	struct ehci_hcd *ehci;
	struct ehci_qtd_hw *qtd, *qtd_prev, *qtd_first;
	struct ehci_req *ehci_req;
	uint32_t token;

	assert(req);

	log_debug("endp->type(%d) token(%d) req->len(%d)", req->endp->type, req->token, req->len);

	ehci = hcd_to_ehci(req->endp->dev->hcd);

	switch (req->endp->type) {
	case USB_COMM_CONTROL:
		/* Setup stage */
		qtd = ehci_qtd_alloc(ehci);
		qtd_first = qtd;
		token = EHCI_QTD_PID_SETUP << 8;
		token |= EHCI_QTD_STS_ACTIVE | (2 << EHCI_QTD_CERR_SHIFT);

		ehci_qtd_fill(ehci, qtd, (uintptr_t)&req->ctrl_header,
			sizeof req->ctrl_header, token, 0);
		//log_debug("!!! first qtd = %p, token = 0x%08x", qtd, qtd->hw_token);
		qtd_prev = qtd;
		qtd = ehci_qtd_alloc(ehci);
		/* Data stage (optional) */
		if (req->len > 0) {
			if (req->token & USB_TOKEN_OUT)  {
				dcache_flush(req->buf, req->len);
				token = EHCI_QTD_PID_OUT << 8;
			} else {
				token = EHCI_QTD_PID_IN << 8;
			}
			token |= EHCI_QTD_STS_ACTIVE | (2 << EHCI_QTD_CERR_SHIFT);
			token |= EHCI_QTD_TOGGLE;

			if (req->token & USB_TOKEN_OUT)  {
				ehci_qtd_fill(ehci, qtd, (uintptr_t) req->buf, req->len,
					token, 0);
			} else {
				assert(req->len <= sizeof req->buffer);
				memset(req->buffer, 0x55, req->len);
				ehci_qtd_fill(ehci, qtd, (uintptr_t) req->buffer, req->len,
					token, 0);
			}
			//log_debug("!!! second qtd = %p, token = 0x%08x, len=%d",
			//	qtd, qtd->hw_token, req->len);
			qtd_prev->hw_next = (uint32_t) qtd;
			qtd_prev = qtd;
		}
		/* Status stage */
		qtd = ehci_qtd_alloc(ehci);
		if (req->token & USB_TOKEN_OUT)  {
			token = EHCI_QTD_PID_IN << 8;
		} else {
			token = EHCI_QTD_PID_OUT << 8;
		}
		/* Interrupt when all stages are processed. */
		token |= EHCI_QTD_STS_ACTIVE | (2 << EHCI_QTD_CERR_SHIFT) | EHCI_QTD_IOC;
		token |= EHCI_QTD_TOGGLE;

		ehci_qtd_fill(ehci, qtd, 0, 0, token, 0);
		//log_debug("!!! third qtd = %p, token = 0x%08x", qtd, qtd->hw_token);
		qtd_prev->hw_next = (uint32_t) qtd;
		break;
	case USB_COMM_BULK:
	case USB_COMM_INTERRUPT:
	case USB_COMM_ISOCHRON:
		log_debug("bulk %s", req->token & USB_TOKEN_OUT ? "out" : "in");
		qtd = ehci_qtd_alloc(ehci);
		qtd_first = qtd;
		if (req->token & USB_TOKEN_OUT)  {
			dcache_flush(req->buf, req->len);
			token = EHCI_QTD_PID_OUT << 8;
		} else {
			token = EHCI_QTD_PID_IN << 8;
		}
		token |= EHCI_QTD_STS_ACTIVE | (2 << EHCI_QTD_CERR_SHIFT) | EHCI_QTD_IOC;

		if (req->token & USB_TOKEN_OUT)  {
			ehci_qtd_fill(ehci, qtd, (uintptr_t) req->buf, req->len,
				token, 0);
		} else {
			assert(req->len <= sizeof req->buffer);
			memset(req->buffer, 0x55, req->len);
			ehci_qtd_fill(ehci, qtd, (uintptr_t) req->buffer, req->len,
				token, 0);
		}
		break;
	default:
		panic("ehci_request: Unsupported enpd type %d", req->endp->type);
	}

	ehci_req = ehci_req_alloc(ehci);
	assert(ehci_req);
	ehci_req->req = req;
	ehci_req->qtds_head = qtd_first;

	if (usb_queue_empty(&ehci->req_queue)) {
		usb_queue_add(&ehci->req_queue, &ehci_req->req_link);
		ehci_submit_async(ehci, ehci_req);
	} else {
		usb_queue_add(&ehci->req_queue, &ehci_req->req_link);
	}

	return 0;
}

static const struct usb_hcd_ops ehci_hcd_ops = {
	.hcd_hci_alloc = ehci_hcd_alloc,
	.hcd_hci_free = ehci_hcd_free,
	.endp_hci_alloc = ehci_ed_alloc,
	.endp_hci_free = ehci_ed_free,
	.hcd_start = ehci_start,
	.hcd_stop = ehci_stop,
	.root_hub_control = ehci_root_hub_control,
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
