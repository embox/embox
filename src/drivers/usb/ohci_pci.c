/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.10.2013
 */

#include <errno.h>
#include <string.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/time/ktime.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <drivers/usb.h>
#include <mem/misc/pool.h>

#include <kernel/printk.h>

#include "ohci.h"

#define OHCI_MAX_REQUESTS 32

#define OHCI_WRITE_STATE(ohcd, state) \
	OHCI_WRITE(ohcd, &ohcd->base->hc_control, \
			(OHCI_READ(ohcd, &ohcd->base->hc_control) & \
				~OHCI_CTRL_FUNC_STATE_MASK) | state)

/* OHCI private stuff */
/* HCCAs are in separate pool to prevent huge padding */
POOL_DEF(ohci_hccas, struct ohci_hcca, USB_MAX_HCD);
POOL_DEF(ohci_hcds, struct ohci_hcd, USB_MAX_HCD);
POOL_DEF(ohci_eds, struct ohci_ed, USB_MAX_ENDP);
POOL_DEF(ohci_tds, struct ohci_td, OHCI_MAX_REQUESTS);

static void *ohci_hcd_alloc(struct usb_hcd *hcd, void *args) {
	struct ohci_hcd *ohcd = pool_alloc(&ohci_hcds);
	struct ohci_hcca *hcca = pool_alloc(&ohci_hccas);
	unsigned int rh_port_n;

	if (!ohcd) {
		assert(!hcca);
		return NULL;
	}
	assert(hcca);

	memset(hcca, 0, sizeof(struct ohci_hcca));

	ohcd->base = (struct ohci_reg *) args;
	ohcd->hcca = hcca;

	rh_port_n = OHCI_READ(ohcd, &ohcd->base->hc_rh_desc_a)
		& OHCI_RH_DESC_A_N_DOWNP_M;
	hcd->root_hub = usb_hub_alloc(hcd, rh_port_n);

	ohcd->hcd = hcd;

	return ohcd;
}

static void ohci_hcd_free(struct usb_hcd *hcd) {
	struct ohci_hcd *ohcd = hcd2ohci(hcd);

	pool_free(&ohci_hcds, ohcd->hcca);
	pool_free(&ohci_hccas, ohcd);
}

static struct ohci_td *ohci_td_alloc(void) {
	return pool_alloc(&ohci_tds);
}

static void ohci_td_free(struct ohci_td *td) {
	pool_free(&ohci_tds, td);
}

static void *ohci_ed_alloc(struct usb_endp *ep) {
	struct ohci_td *sentinel_td = ohci_td_alloc();
	struct ohci_ed *ed = pool_alloc(&ohci_eds);

	memset(ed, 0, sizeof(struct ohci_ed));

	ed->head_td = (uint32_t) sentinel_td;
	ed->tail_td = (uint32_t) sentinel_td;
	ed->next_ed = 0;
	ed->flags = 64 << 16;

	return ed;
}

static void ohci_ed_free(struct usb_endp *ep) {
	pool_free(&ohci_eds, ep->hci_specific);
}
/* === end === */

static struct ohci_td *ohci_td_fill(struct ohci_td *td, uint32_t flags,
		void *buf, size_t size, struct usb_request *req) {

	if (!td) {
		return NULL;
	}

	memset(td, 0, sizeof(struct ohci_td));
	td->flags = flags;
	td->buf_p = (uint32_t) buf;
	td->buf_end = td->buf_p + size - 1;
	td->req = req;

	return td;
}

static inline struct ohci_td *ohci_td_next(struct ohci_td *td) {
	return (struct ohci_td *) REG_LOAD(&td->next_td);
}

static struct ohci_td *ohci_ed_get_tail_td(struct ohci_ed *ed) {
	return (void *) REG_LOAD(&ed->tail_td);
}

static int ohci_td_enque_tail(struct ohci_ed *ed,
		struct ohci_td *placeholder_td) {
	struct ohci_td *td = ohci_ed_get_tail_td(ed);

	REG_STORE(&td->next_td, (unsigned long) placeholder_td);
	REG_STORE(&ed->tail_td, (unsigned long) placeholder_td);

	return 0;
}

static void ohci_ed_fill(struct ohci_ed *ed, struct usb_endp *endp) {
	uint32_t flags = REG_LOAD(&ed->flags);

	REG_ORIN(&ed->flags, OHCI_ED_K);

	flags = (flags & ~OHCI_ED_FUNC_ADDRESS_MASK) |
		(endp->dev->bus_idx << OHCI_ED_FUNC_ADDRESS_OFFS);
	flags = (flags & ~OHCI_ED_ENDP_ADDRESS_MASK) |
		(endp->num << OHCI_ED_ENDP_ADDRESS_OFFS);

	REG_STORE(&ed->flags, flags);
}

static int ohci_start(struct usb_hcd *hcd) {
	struct ohci_hcd *ohcd;
	bool need_wait;
	uint32_t hc_ctrl;
	uint32_t hc_state;
	uint32_t backed_fm_interval;

	ohcd = hcd2ohci(hcd);

	hc_ctrl = OHCI_READ(ohcd, &ohcd->base->hc_control);
	hc_state = hc_ctrl & OHCI_CTRL_FUNC_STATE_MASK;
	need_wait = false;

	if (hc_ctrl & OHCI_CTRL_INT_ROUT) {
		/* SMM active */
		OHCI_WRITE(ohcd, &ohcd->base->hc_control, OHCI_CMD_OWNER_CHAGE);

		/* poll */
		while ((hc_ctrl = OHCI_READ(ohcd, &ohcd->base->hc_control))
				& OHCI_CTRL_INT_ROUT) {

		}
	} else if (hc_state != OHCI_CTRL_FUNC_STATE_RST) {
		/* BIOS active */
		if (hc_state != OHCI_CTRL_FUNC_STATE_OPRT) {
			OHCI_WRITE_STATE(ohcd, OHCI_CTRL_FUNC_STATE_RESM);
			need_wait = true;
		}
	} else {
		need_wait = true;
	}

	if (need_wait) {
		ksleep(100);
		/* delay */
	}

	backed_fm_interval = OHCI_READ(ohcd, &ohcd->base->hc_fm_interval);
	OHCI_WRITE(ohcd, &ohcd->base->hc_cmdstat,
			OHCI_READ(ohcd, &ohcd->base->hc_cmdstat) |
				OHCI_CMD_RESET);

	/* poll */
	while (OHCI_READ(ohcd, &ohcd->base->hc_cmdstat) & OHCI_CMD_RESET) {

	}

	OHCI_WRITE(ohcd, &ohcd->base->hc_fm_interval, backed_fm_interval);


	OHCI_WRITE(ohcd, &ohcd->base->hc_period_cur_ed, 	0);
	OHCI_WRITE(ohcd, &ohcd->base->hc_ctrl_head_ed, 	0);
	OHCI_WRITE(ohcd, &ohcd->base->hc_ctrl_cur_ed, 	0);
	OHCI_WRITE(ohcd, &ohcd->base->hc_bulk_head_ed, 	0);
	OHCI_WRITE(ohcd, &ohcd->base->hc_bulk_cur_ed, 	0);
	OHCI_WRITE(ohcd, &ohcd->base->hc_done_head, 		0);

	OHCI_WRITE(ohcd, &ohcd->base->hc_hcca, ohcd->hcca);
	OHCI_WRITE(ohcd, &ohcd->base->hc_inten, OHCI_INTERRUPT_ALL_BUTSOF);

	OHCI_WRITE(ohcd, &ohcd->base->hc_control,
			OHCI_READ(ohcd, &ohcd->base->hc_control)
				| OHCI_CTRL_PERIOD_EN
				| OHCI_CTRL_ISOCHR_EN
				| OHCI_CTRL_CTRL_EN
				| OHCI_CTRL_BULK_EN);

	OHCI_WRITE(ohcd, &ohcd->base->hc_period_start,
			(9 * (OHCI_READ(ohcd, &ohcd->base->hc_fm_interval)
				       	& OHCI_FM_INTERVAL_FI_MASK)) / 10);

	OHCI_WRITE_STATE(ohcd, OHCI_CTRL_FUNC_STATE_OPRT);

	return 0;
}

static int ohci_stop(struct usb_hcd *hcd) {
	return 0;
}

static inline unsigned int ohci_port_stat_map(uint16_t val) {
	unsigned short ret = 0;

	if (val & OHCI_RH_PORT_CONN) {
		ret |= USB_HUB_PORT_CONNECT;
	}
	if (val & OHCI_RH_PORT_RESET) {
		ret |= USB_HUB_PORT_RESET;
	}
	if (val & OHCI_RH_PORT_POWERST) {
		ret |= USB_HUB_PORT_POWER;
	}

	return ret;
}

static int ohci_rh_ctrl(struct usb_hub_port *port, enum usb_hub_request req,
			unsigned short value) {
	struct ohci_hcd *ohcd = hcd2ohci(port->hub->hcd);
	uint32_t wval = 0;

	if (value & USB_HUB_PORT_ENABLE) {
		wval |= OHCI_RH_PORT_ENABLE;
	}
	if (value & USB_HUB_PORT_POWER) {
		wval |= OHCI_RH_PORT_POWERST;
	}
	if (value & USB_HUB_PORT_RESET) {
		wval |= OHCI_RH_PORT_RESET;
	}

	if (req == USB_HUB_REQ_PORT_CLEAR) {
		wval = ~wval
			& OHCI_READ(ohcd, &ohcd->base->hc_rh_port_stat[port->idx]);
	}

	OHCI_WRITE(ohcd, &ohcd->base->hc_rh_port_stat[port->idx], wval);

	port->status = ohci_port_stat_map(OHCI_READ(ohcd,
				&ohcd->base->hc_rh_port_stat[port->idx]));

	return 0;
}

static void ohci_ed_sched(struct ohci_hcd *ohcd, struct ohci_ed *ed) {
	uint32_t cmdstat;

	if (REG_LOAD(&ed->flags) & OHCI_ED_SCHEDULED) {
		return;
	}

	REG_ORIN(&ed->flags, OHCI_ED_SCHEDULED);

	REG_STORE(&ed->next_ed, OHCI_READ(ohcd, &ohcd->base->hc_ctrl_head_ed));
	OHCI_WRITE(ohcd, &ohcd->base->hc_ctrl_head_ed, ed);

	cmdstat = OHCI_READ(ohcd, &ohcd->base->hc_cmdstat);
	OHCI_WRITE(ohcd, &ohcd->base->hc_cmdstat, cmdstat |
			OHCI_CMD_CONTROL_FILLED);
}

static void ohci_transfer(struct ohci_ed *ed, uint32_t token, void *buf,
		size_t len, struct usb_request *req) {
	struct ohci_td *td, *next_td;

	next_td = ohci_td_alloc();
	td = ohci_ed_get_tail_td(ed);

	ohci_td_fill(td, token, buf, len, req);

	ohci_td_enque_tail(ed, next_td);
}

static int ohci_request(struct usb_endp *endp, struct usb_request *req) {
	struct ohci_hcd *ohcd = hcd2ohci(endp->dev->hcd);
	struct ohci_ed *ed = endp2ohci(endp);
	uint32_t token, negtoken;

	if (req->ctrl_header.bm_request_type & USB_DEV_REQ_TYPE_RD) {
		token = OHCI_TD_IN;
		negtoken = OHCI_TD_OUT;
	} else {
		token = OHCI_TD_OUT;
		negtoken = OHCI_TD_IN;
	}

	ohci_ed_fill(ed, endp); /* function address could change due bus
				   enumeration */

	ohci_transfer(ed, OHCI_TD_SETUP, &req->ctrl_header,
			sizeof(struct usb_control_header), NULL);

	if (req->len > 0) {
		ohci_transfer(ed, token, req->buf, req->len, NULL);
	}

	ohci_transfer(ed, negtoken, (void *) 1, 0, req);

	ohci_ed_sched(ohcd, ed);

	return 0;
}

static struct usb_hcd_ops ohci_hcd_ops = {
	.hcd_hci_alloc = ohci_hcd_alloc,
	.hcd_hci_free = ohci_hcd_free,
	.endp_hci_alloc = ohci_ed_alloc,
	.endp_hci_free = ohci_ed_free,
	.hcd_start = ohci_start,
	.hcd_stop = ohci_stop,
	.rhub_ctrl = ohci_rh_ctrl,
	.control_request = ohci_request,
};

static inline enum usb_request_status ohci_td_stat(struct ohci_td *td) {
	switch (td->flags & OHCI_TD_FLAGS_CC_MASK) {
	case OHCI_TD_CC_NOERR:
		return USB_REQ_NOERR;
	case OHCI_TD_CC_STALL:
		return USB_REQ_STALL;
	default:
		return USB_REQ_INTERR;
	}
}

static irq_return_t ohci_irq(unsigned int irq_nr, void *data) {
	struct usb_hcd *hcd = data;
	struct ohci_hcd *ohcd = hcd2ohci(hcd);
	uint32_t intr_stat = OHCI_READ(ohcd, &ohcd->base->hc_intstat);

	/*printk("%s: irq_stat=0x%08x\n", __func__, intr_stat);*/

	if (intr_stat & OHCI_INTERRUPT_RHUB) {
		struct usb_hub *rhub = ohci2hcd(ohcd)->root_hub;

		OHCI_WRITE(ohcd, &ohcd->base->hc_intstat, OHCI_INTERRUPT_RHUB);

		for (int i = 0; i < rhub->port_n; i++) {
			uint32_t port_stat = OHCI_READ(ohcd, &ohcd->base->hc_rh_port_stat[i]);

			rhub->ports[i].status = ohci_port_stat_map(port_stat & 0xffff);
			rhub->ports[i].changed = ohci_port_stat_map(port_stat >> 16);

			if (port_stat >> 16) {
				OHCI_WRITE(ohcd, &ohcd->base->hc_rh_port_stat[i],
						port_stat & 0xffff0000);
			}
		}

		usb_rh_nofity(hcd);
	}

	if (intr_stat & OHCI_INTERRUPT_DONE_QUEUE) {
		struct ohci_td *td;
		struct usb_request *req;

		td = (struct ohci_td *) (REG_LOAD(&ohcd->hcca->done_head) & ~1);

		do {
			req = ohci2req(td);
			if (!req) {
				ohci_td_free(td);
				continue;
			}

			req->req_stat = ohci_td_stat(td);
			usb_request_complete(req);
		} while ((td = ohci_td_next(td)));

		OHCI_WRITE(ohcd, &ohcd->base->hc_intstat, OHCI_INTERRUPT_DONE_QUEUE);
	}

	return IRQ_HANDLED;
}

static inline int ohci_verify(struct ohci_reg *base) {
	return (0xff & REG_LOAD(&base->hc_revision)) == OHCI_USB_REV10;
}

PCI_DRIVER("Apple OHCI usb host", ohci_pci_init, PCI_VENDOR_ID_APPLE,
		PCI_DEV_ID_APPLE_OHCI_HOST);

static int ohci_pci_init(struct pci_slot_dev *pci_dev) {
	struct ohci_reg *base = (struct ohci_reg *) pci_dev->bar[0];
	struct usb_hcd *hcd;
	int ret;

	if (!ohci_verify(base)) {
		return -ENOTSUP;
	}

	hcd = usb_hcd_alloc(&ohci_hcd_ops, base);
	if (!hcd) {
		return -ENOMEM;
	}

	ret = irq_attach(pci_dev->irq, ohci_irq, 0, hcd, "ohci irq");
	if (0 != ret) {
		return ret;
	}

	return usb_hcd_register(hcd);
}

