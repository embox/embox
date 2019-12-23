/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.10.2013
 */

#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>

#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <kernel/time/ktime.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <drivers/usb/usb.h>
#include <mem/misc/pool.h>
#include <util/log.h>

#include "ohci.h"

#define OHCI_MAX_REQUESTS 32

#define OHCI_WRITE_STATE(ohcd, state) \
	OHCI_WRITE(ohcd, &ohcd->base->hc_control, \
			(OHCI_READ(ohcd, &ohcd->base->hc_control) & \
				~OHCI_CTRL_FUNC_STATE_MASK) | state)

struct ohci_request_priv {
	uint32_t tds_count;
};

/* OHCI private stuff */
/* HCCAs are in separate pool to prevent huge padding */
POOL_DEF(ohci_hccas, struct ohci_hcca, USB_MAX_HCD);
POOL_DEF(ohci_hcds, struct ohci_hcd, USB_MAX_HCD);
POOL_DEF(ohci_eds, struct ohci_ed, USB_MAX_ENDP);
POOL_DEF(ohci_tds, struct ohci_td, OHCI_MAX_REQUESTS);
POOL_DEF(ohci_req_priv_pool, struct ohci_request_priv, OHCI_MAX_REQUESTS);

static struct ohci_td *ohci_ed_get_tail_td(struct ohci_ed *ed);
static int ohci_ed_desched_interrupt(struct ohci_hcd *ohcd, struct ohci_ed *ed);
static int ohci_ed_desched(struct ohci_hcd *ohcd, struct ohci_ed *ed,
		struct ohci_ed **ed_queue);

static void *ohci_hcd_alloc(struct usb_hcd *hcd, void *args) {
	struct ohci_hcd *ohcd = pool_alloc(&ohci_hcds);
	struct ohci_hcca *hcca = pool_alloc(&ohci_hccas);

	if (!ohcd) {
		assert(!hcca);
		return NULL;
	}
	assert(hcca);

	memset(hcca, 0, sizeof(struct ohci_hcca));

	ohcd->base = (struct ohci_reg *) args;
	ohcd->hcca = hcca;
	ohcd->hcd = hcd;

	return ohcd;
}

static void ohci_hcd_free(struct usb_hcd *hcd, void *spec) {
	struct ohci_hcd *ohcd = hcd2ohci(hcd);

	assert(ohcd == spec);

	pool_free(&ohci_hcds, ohcd->hcca);
	pool_free(&ohci_hccas, ohcd);
}

static struct ohci_td *ohci_td_alloc(void) {
	struct ohci_td *td = pool_alloc(&ohci_tds);
	return td;
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

static void ohci_ed_free(struct usb_endp *ep, void *spec) {
	struct ohci_ed *ed = endp2ohci(ep);
	struct ohci_hcd *ohcd = hcd2ohci(ep->dev->hcd);
	struct ohci_td *placeholder_td;

	if (ep->type == USB_COMM_INTERRUPT) {
		ohci_ed_desched_interrupt(ohcd, ed);
	} else {
		ohci_ed_desched(ohcd, ed, (struct ohci_ed **) &ohcd->base->hc_ctrl_head_ed);
		/* FIXME WTF? */
		ohci_ed_desched(ohcd, ed, (struct ohci_ed **) &ohcd->base->hc_ctrl_head_ed);
	}

	placeholder_td = ohci_ed_get_tail_td(ed);
	ohci_td_free(placeholder_td);

	pool_free(&ohci_eds, spec);
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
		(endp->dev->addr << OHCI_ED_FUNC_ADDRESS_OFFS);
	flags = (flags & ~OHCI_ED_ENDP_ADDRESS_MASK) |
		(endp->address << OHCI_ED_ENDP_ADDRESS_OFFS);

#if 0
	/* resetting any error condition */
	REG_ANDIN(&ed->head_td, ~3);
#endif

	REG_STORE(&ed->flags, flags);
}

static int ohci_start(struct usb_hcd *hcd) {
	struct ohci_hcd *ohcd;
	bool need_wait;
	uint32_t hc_ctrl;
	uint32_t hc_state;
	uint32_t backed_fm_interval;
	struct usb_dev *udev;

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


	OHCI_WRITE(ohcd, &ohcd->base->hc_period_cur_ed, 0);
	OHCI_WRITE(ohcd, &ohcd->base->hc_ctrl_head_ed,  0);
	OHCI_WRITE(ohcd, &ohcd->base->hc_ctrl_cur_ed,   0);
	OHCI_WRITE(ohcd, &ohcd->base->hc_bulk_head_ed,  0);
	OHCI_WRITE(ohcd, &ohcd->base->hc_bulk_cur_ed,   0);
	OHCI_WRITE(ohcd, &ohcd->base->hc_done_head,     0);

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

	/* Create root hub */
	udev = usb_new_device(NULL, hcd, 0);
	if (!udev) {
		log_error("ohci_start: usb_new_device failed\n");
		return -1;
	}

	return 0;
}

static int ohci_stop(struct usb_hcd *hcd) {
	return 0;
}

static void ohci_get_hub_descriptor(struct ohci_hcd *ohcd,
		struct usb_desc_hub *desc) {
	desc->b_desc_length = 7;
	desc->b_desc_type = USB_DT_HUB;
	desc->b_nbr_ports = OHCI_READ(ohcd, &ohcd->base->hc_rh_desc_a)
		& OHCI_RH_DESC_A_N_DOWNP_M;
}

static uint32_t ohci_roothub_portstatus(struct ohci_hcd *ohcd,
		unsigned port) {
	return OHCI_READ(ohcd, &ohcd->base->hc_rh_port_stat[port]);
}

static int ohci_root_hub_control(struct usb_request *req) {
	struct ohci_hcd *ohcd = hcd2ohci(req->endp->dev->hcd);
	struct usb_control_header *ctrl = &req->ctrl_header;
	uint32_t type_req;
	uint32_t wval;

	type_req = (ctrl->bm_request_type << 8) | ctrl->b_request;

	switch (type_req) {
	case USB_GET_HUB_DESCRIPTOR:
		ohci_get_hub_descriptor(ohcd, (struct usb_desc_hub *) req->buf);
		break;
	case USB_GET_PORT_STATUS:
		*(uint32_t *)req->buf = ohci_roothub_portstatus(ohcd,
			ctrl->w_index - 1);
		break;
	case USB_SET_PORT_FEATURE:
		switch (ctrl->w_value) {
		case USB_PORT_FEATURE_RESET:
			wval = OHCI_RH_R_RST_W_STRST;
			break;
		case USB_PORT_FEATURE_POWER:
			wval = OHCI_RH_R_PWR_W_STPWR;
			break;
		default:
			log_error("Unknown port set feature: 0x%x\n", ctrl->w_value);
			return -1;
		}
		OHCI_WRITE(ohcd, &ohcd->base->hc_rh_port_stat[ctrl->w_index - 1],
			wval);
		break;
	case USB_CLEAR_PORT_FEATURE:
		switch (ctrl->w_value) {
		case USB_PORT_FEATURE_POWER:
			wval = OHCI_RH_R_LOWSPD_W_CLPWR;
			break;
		case USB_PORT_FEATURE_C_CONNECTION:
			wval = OHCI_RH_PS_CSC;
			break;
		default:
			log_error("Unknown port clear feature: 0x%x\n", ctrl->w_value);
			return -1;
		}
		OHCI_WRITE(ohcd, &ohcd->base->hc_rh_port_stat[ctrl->w_index - 1],
			wval);
		break;
	default:
		panic("ohci_root_hub_control: Unknown req_type=0x%x, request=0x%x\n",
			ctrl->bm_request_type, ctrl->b_request);
		break;
	}

	req->req_stat = USB_REQ_NOERR;
	usb_request_complete(req);

	return 0;
}

static int ohci_common_sched(struct ohci_hcd *ohcd, struct ohci_ed *ed) {
	if (REG_LOAD(&ed->flags) & OHCI_ED_SCHEDULED) {
		return 0;
	}

	REG_ORIN(&ed->flags, OHCI_ED_SCHEDULED);

	return 1;
}

static int ohci_common_issched(struct ohci_hcd *ohcd, struct ohci_ed *ed) {
	return REG_LOAD(&ed->flags) & OHCI_ED_SCHEDULED;
}

static void ohci_common_desched(struct ohci_hcd *ohcd, struct ohci_ed *ed) {
	REG_ANDIN(&ed->flags, ~OHCI_ED_SCHEDULED);
}

static void ohci_ed_sched(struct ohci_hcd *ohcd, struct ohci_ed *ed) {
	uint32_t cmdstat;

	if (ohci_common_sched(ohcd, ed)) {
		REG_STORE(&ed->next_ed, OHCI_READ(ohcd, &ohcd->base->hc_ctrl_head_ed));
		OHCI_WRITE(ohcd, &ohcd->base->hc_ctrl_head_ed, ed);
	}

	cmdstat = OHCI_READ(ohcd, &ohcd->base->hc_cmdstat);
	OHCI_WRITE(ohcd, &ohcd->base->hc_cmdstat, cmdstat |
			OHCI_CMD_CONTROL_FILLED);
}

static int ohci_ed_desched(struct ohci_hcd *ohcd, struct ohci_ed *ed,
		struct ohci_ed **ed_queue) {
	struct ohci_ed **p_ed, *c_ed, *n_ed;

	if (!ohci_common_issched(ohcd, ed)) {
		return 0;
	}

	REG_ORIN(&ed->flags, OHCI_ED_K);

	p_ed = ed_queue;
	c_ed = (struct ohci_ed *) OHCI_READ(ohcd, p_ed);
	while (c_ed != NULL && c_ed != ed) {
		p_ed = (struct ohci_ed **) &c_ed->next_ed;
		c_ed = (struct ohci_ed *) REG_LOAD(p_ed);
	}

	if (!c_ed) {
		return 0;
	}

	n_ed = (void *) REG_LOAD(&ed->next_ed);

	if (p_ed == ed_queue) {
		OHCI_WRITE(ohcd, p_ed, n_ed);
	} else {
		REG_STORE(p_ed, (unsigned long) n_ed);
	}

	ohci_common_desched(ohcd, ed);

	return 1;
}

static void ohci_ed_sched_interrupt(struct ohci_hcd *ohcd, struct ohci_ed *ed) {
	struct ohci_hcca *hcca = ohcd->hcca;
	int i;

	if (!ohci_common_sched(ohcd, ed)) {
		return;
	}

	/* just leave somewhere! */
	for (i = 0; i < OHCI_HCCA_INTERRUPT_LIST_N; i++) {
		struct ohci_ed *next_ed = (struct ohci_ed *) REG_LOAD(&hcca->interrupt_table[i]);

		if (!next_ed) {
			REG_STORE(&ed->next_ed, (unsigned long) next_ed);
			REG_STORE(&hcca->interrupt_table[i], (unsigned long) ed);
			break;
		}
	}

	assertf(i < OHCI_HCCA_INTERRUPT_LIST_N, "%s: there is no empty slot for "
			"interrupt request", __func__);
}

static int ohci_ed_desched_interrupt(struct ohci_hcd *ohcd, struct ohci_ed *ed) {
	struct ohci_hcca *hcca = ohcd->hcca;
	int i;

	if (!ohci_common_issched(ohcd, ed)) {
		return 0;
	}

	for (i = 0; i < OHCI_HCCA_INTERRUPT_LIST_N; i++) {
		struct ohci_ed *i_ed = (struct ohci_ed *) REG_LOAD(&hcca->interrupt_table[i]);

		if (i_ed == ed) {
			REG_STORE(&hcca->interrupt_table[i], (unsigned long) NULL);
			return 1;
		}
	}

	return 0;

}

static void ohci_transfer(struct ohci_ed *ed, uint32_t token, void *buf,
		size_t len, struct usb_request *req) {
	struct ohci_td *td, *next_td;

	next_td = ohci_td_alloc();
	td = ohci_ed_get_tail_td(ed);

	ohci_td_fill(td, token, buf, len, req);

	ohci_td_enque_tail(ed, next_td);
}

static int ohci_request(struct usb_request *req) {
	struct ohci_hcd *ohcd = hcd2ohci(req->endp->dev->hcd);
	struct ohci_ed *ed = endp2ohci(req->endp);
	uint32_t token;
	int tds_count = 0;
	struct ohci_request_priv *priv;

	/* function address could change due bus enumeration */
	ohci_ed_fill(ed, req->endp);

	switch (req->endp->type) {
	case USB_COMM_CONTROL:
		/* Setup stage */
		ohci_transfer(ed, OHCI_TD_SETUP, &req->ctrl_header,
			sizeof req->ctrl_header, req);
		tds_count++;
		/* Data stage (optional) */
		if (req->len > 0) {
			tds_count++;
			token = (req->token & USB_TOKEN_OUT) ? OHCI_TD_OUT : OHCI_TD_IN;
			ohci_transfer(ed, token, req->buf, req->len, req);
		}
		/* Status stage */
		tds_count++;
		token = (req->token & USB_TOKEN_OUT) ? OHCI_TD_IN : OHCI_TD_OUT;
		ohci_transfer(ed, token, NULL, 0, req);
		break;
	case USB_COMM_BULK:
	case USB_COMM_INTERRUPT:
	case USB_COMM_ISOCHRON:
		tds_count++;
		token = (req->token & USB_TOKEN_OUT) ? OHCI_TD_OUT : OHCI_TD_IN;
		ohci_transfer(ed, token, req->buf, req->len, req);
		break;
	default:
		panic("ohci_request: Unsupported enpd type %d", req->endp->type);
	}

	priv = pool_alloc(&ohci_req_priv_pool);
	assert(priv);
	priv->tds_count = tds_count;
	req->hci_specific = priv;

	if (req->endp->type == USB_COMM_INTERRUPT) {
		ohci_ed_sched_interrupt(ohcd, ed);
	} else {
		ohci_ed_sched(ohcd, ed);
	}

	return 0;
}

static struct usb_hcd_ops ohci_hcd_ops = {
	.hcd_hci_alloc = ohci_hcd_alloc,
	.hcd_hci_free = ohci_hcd_free,
	.endp_hci_alloc = ohci_ed_alloc,
	.endp_hci_free = ohci_ed_free,
	.hcd_start = ohci_start,
	.hcd_stop = ohci_stop,
	.root_hub_control = ohci_root_hub_control,
	.request = ohci_request,
};

static inline enum usb_request_status ohci_td_stat(struct ohci_td *td) {
	switch (td->flags & OHCI_TD_FLAGS_CC_MASK) {
	case OHCI_TD_CC_NOERR:
		return USB_REQ_NOERR;
	case OHCI_TD_CC_STALL:
		return USB_REQ_STALL;
	case OHCI_TD_CC_DATA_UNDERRUN:
		return USB_REQ_UNDERRUN;
	default:
		return USB_REQ_INTERR;
	}
}

static uint32_t ohci_td_received_len(struct ohci_td *td, struct usb_request *req) {
	assert(((uint32_t) req->buf <= td->buf_p) || !td->buf_p);

	if (!td->buf_p) {
		/* A value of 0 indicates a zerolength
		 * data packet or that all bytes have been transferred. */
		return req->req_stat == USB_REQ_NOERR ? req->len : 0;
	}

	return td->buf_p - (uint32_t) req->buf;
}

static irq_return_t ohci_irq(unsigned int irq_nr, void *data) {
	struct usb_hcd *hcd;
	struct ohci_hcd *ohcd;
	uint32_t intr_stat;

	hcd = data;
	ohcd = hcd2ohci(hcd);
	intr_stat = OHCI_READ(ohcd, &ohcd->base->hc_intstat);

	if (intr_stat & OHCI_INTERRUPT_RHUB) {
		OHCI_WRITE(ohcd, &ohcd->base->hc_intstat, OHCI_INTERRUPT_RHUB);
	}

	if (intr_stat & OHCI_INTERRUPT_DONE_QUEUE) {
		struct ohci_td *td, *next_td;
		struct usb_request *req;
		struct ohci_request_priv *priv;

		td = (struct ohci_td *) (REG_LOAD(&ohcd->hcca->done_head) & ~1);

		do {
			req = ohci2req(td);

			assert(req);

			req->req_stat = ohci_td_stat(td);
			req->actual_len += ohci_td_received_len(td, req);
			next_td = ohci_td_next(td);

			ohci_td_free(td);

			priv = req->hci_specific;
			priv->tds_count--;
			if (!priv->tds_count) {
				pool_free(&ohci_req_priv_pool, priv);
				usb_request_complete(req);
			}
		} while ((td = next_td));

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

	base = (struct ohci_reg *)mmap_device_memory(base, 0x1000,
			PROT_WRITE | PROT_READ,
			MAP_FIXED,
			(uintptr_t)base);
	if (!ohci_verify(base)) {
		return -ENOTSUP;
	}

	hcd = usb_hcd_alloc(&ohci_hcd_ops, base);
	if (!hcd) {
		return -ENOMEM;
	}

	ret = irq_attach(pci_dev->irq, ohci_irq, IF_SHARESUP, hcd, "ohci irq");
	if (0 != ret) {
		return ret;
	}

	return usb_hcd_register(hcd);
}

