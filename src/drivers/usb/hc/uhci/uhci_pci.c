/**
 * @file
 * @brief
 *
 * @author  Filipp Chubukov
 * @date    01.10.2020
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
#include <drivers/pci/pci_repo.h>
#include <drivers/pci/pci_chip/pci_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <mem/vmem.h>
#include "uhci.h"

#define UHCI_BAR_NUMBER                 4
#define UHCI_PCI_BAR_IO                 0x01

/* Limits */

#define UHCI_MAX_QH                     8
#define UHCI_MAX_TD                     32
#define UHCI_FRAMELIST_SIZE             1024

/* UHCI private stuff */
POOL_DEF(uhci_controllers, struct uhci_controller, USB_MAX_HCD);
POOL_DEF(uhci_hcds, struct uhci_hcd, USB_MAX_HCD);
POOL_DEF_ATTR(uhci_framelist, uint32_t, UHCI_FRAMELIST_SIZE, __attribute__((aligned(MMU_PAGE_SIZE))));
POOL_DEF_ATTR(uhci_qhs, struct uhci_qh, UHCI_MAX_QH, __attribute__((aligned(MMU_PAGE_SIZE))));
POOL_DEF_ATTR(uhci_tds, struct uhci_td, UHCI_MAX_TD, __attribute__((aligned(MMU_PAGE_SIZE))));

static struct uhci_qh *uhci_alloc_qh(void) {
	struct uhci_qh *qh = pool_alloc(&uhci_qhs);
	return qh;
}

static struct uhci_td *uhci_alloc_td(void) {
	struct uhci_td *td = pool_alloc(&uhci_tds);
	return td;
}

static void uhci_td_free(struct uhci_td *td) {
	pool_free(&uhci_tds, td);
}

static void uhci_qh_free(struct uhci_qh *qh) {
	pool_free(&uhci_qhs, qh);
}

static void uhci_port_set(uint32_t addr, uint16_t data) {
	uint16_t status = UHCI_READ_16(addr);
	status |= data;
	status &= ~PORT_RWC;
	UHCI_WRITE_16(status, addr);
}

static void uhci_port_clr(uint32_t addr, uint16_t data) {
    uint16_t status = UHCI_READ_16(addr);
    status &= ~PORT_RWC;
    status &= ~data;
    status |= PORT_RWC & data;
		UHCI_WRITE_16(status, addr);
}

static void *uhci_hcd_alloc(struct usb_hcd *hcd, void *args) {
	struct uhci_hcd *uhcd = pool_alloc(&uhci_hcds);
	struct uhci_controller *uhc = pool_alloc(&uhci_controllers);

	if (!uhcd) {
		assert(uhc);
		return NULL;
	}

	assert(uhc);

	memset(uhc, 0, sizeof(struct uhci_controller));

	uhc->io_addr = *(uint32_t *) args;

	uhc->framelist = pool_alloc(&uhci_framelist);
	assert(uhc->framelist);

	/* Frame list init */
	struct uhci_qh *async_qh = uhci_alloc_qh();

	assert(async_qh);

	async_qh->head = TD_PTR_TERMINATE;
	async_qh->element = TD_PTR_TERMINATE;
	async_qh->qh_link.prev = &async_qh->qh_link;
	async_qh->qh_link.next = &async_qh->qh_link;

	uhc->qh_async = async_qh;
	for (int i = 0; i < UHCI_FRAMELIST_SIZE; i++) {
		uhc->framelist[i] = TD_PTR_QH | (uint32_t)async_qh;
	}

	uhcd->hcd = hcd;
	uhcd->uhc = uhc;

	return uhcd;
}

static uint32_t uhci_roothub_portstatus(struct uhci_controller *uhc, int port) {
	uint16_t status;
	if (port == 0) {
		status = UHCI_READ_16(uhc->io_addr + REG_PORT1);
	} else if (port == 1) {
		status = UHCI_READ_16(uhc->io_addr + REG_PORT2);
	}

	if (~(status) & PORT_CONNECTION) { /* No device in port */
		return 256; /* copy value from OHCI, TODO: how to get status UHCI for usb_hub_port_get_status */
	}
	if (~(status) & PORT_CONNECTION_CHANGE) { /* port reset, clear, device in port */
		return 0x100103; /* copy value from OHCI, TODO: how to get status UHCI correct for usb_hub_port_get_status */
	} else { /* port not reset and clear, device in port */
		return 0x1114371; /* copy value from OHCI, TODO: how to get status UHCI correct for usb_hub_port_get_status */
	}
}

static int uhci_start(struct usb_hcd *hcd) {
	struct usb_dev *udev;
	struct uhci_hcd *uhcd;
	uint16_t io_addr;

	uhcd = hcd2uhci(hcd);
	io_addr = uhcd->uhc->io_addr;

	/* Disable Legacy Support */
	UHCI_WRITE_16(0x8f00, io_addr + REG_LEGSUP);

	/* Disable interrupts */
	UHCI_WRITE_16(0, io_addr + REG_INTR);

	/* Init frame list */
	UHCI_WRITE_16(0, io_addr + REG_FRNUM);
	UHCI_WRITE_32((uint32_t)uhcd->uhc->framelist, io_addr + REG_FRBASEADD);

	/* Init SOFMOD */
	UHCI_WRITE_16(0x40, io_addr + REG_SOFMOD);

	/* Init status */
	UHCI_WRITE_16(0xffff, io_addr + REG_STS);

	/* Start hc */
	UHCI_WRITE_16(CMD_RS, io_addr + REG_CMD);

	/* Create root hub */
	udev = usb_new_device(NULL, hcd, 0);
	if (!udev) {
		log_error("uhci_start: usb_new_device failed\n");
		return -1;
	}

	return 0;
}

static int uhci_stop(struct usb_hcd *hcd) {
	return 0;
}

static void uhci_get_hub_descriptor(struct uhci_hcd *uhcd,
		struct usb_desc_hub *desc) {
	desc->b_desc_length = 7;
	desc->b_desc_type = USB_DT_HUB;
	desc->b_nbr_ports = 2; /* TODO: detect number of ports */
}

static int uhci_root_hub_control(struct usb_request *req) {

	struct uhci_hcd *uhcd = hcd2uhci(req->endp->dev->hcd);
	struct usb_control_header *ctrl = &req->ctrl_header;
	uint32_t type_req;
	uint32_t port_reg;
	type_req = (ctrl->bm_request_type << 8) | ctrl->b_request;

	switch (type_req) {
	case USB_GET_HUB_DESCRIPTOR:
		uhci_get_hub_descriptor(uhcd, (struct usb_desc_hub *) req->buf);
		break;
	case USB_GET_PORT_STATUS:
		*(uint32_t *)req->buf = uhci_roothub_portstatus(uhcd->uhc, ctrl->w_index - 1);
		break;
	case USB_SET_PORT_FEATURE:
		switch (ctrl->w_value) {
		case USB_PORT_FEATURE_RESET:
			break;
		case USB_PORT_FEATURE_POWER:
      /* do nothing, uhci auto start port */
			break;
		default:
			log_error("Unknown port set feature: 0x%x\n", ctrl->w_value);
			return -1;
		}
		break;

    case USB_CLEAR_PORT_FEATURE:
  		switch (ctrl->w_value) {
  		case USB_PORT_FEATURE_POWER:
  			break;
  		case USB_PORT_FEATURE_C_CONNECTION:

			if (ctrl->w_index == 1) {
				port_reg = uhcd->uhc->io_addr + REG_PORT1;
        	} else if (ctrl->w_index == 1) {
				port_reg = uhcd->uhc->io_addr + REG_PORT2;
        	}
        	/* after this our port have correct status for work */
        	uhci_port_set(port_reg, PORT_RESET);
        	uhci_port_clr(port_reg, PORT_RESET);
        	uhci_port_clr(port_reg, PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE);
  			break;
  		case USB_PORT_FEATURE_C_RESET:

  			break;
  		default:
  			log_error("Unknown port clear feature: 0x%x\n", ctrl->w_value);
  			return -1;
  		}
		break;
	default:
		panic("uhci_root_hub_control: Unknown req_type=0x%x, request=0x%x\n",
				ctrl->bm_request_type, ctrl->b_request);
    	break;

	}
	req->req_stat = USB_REQ_NOERR;
	usb_request_complete(req);
	return 0;
}

static void uhci_init_td(struct uhci_td *td, struct uhci_td *prev,
                       unsigned int speed, unsigned int addr,
                        unsigned int endp, unsigned int toggle,
                          unsigned int packet_type, unsigned int len, const void *data) {

    if (len == 0) {
		len = 0x7ff; /* null data packet */
    } else {
		len = (len - 0x1); /* len of data packet */
    }
    if (prev) {
        prev->link = (uint32_t)td | TD_PTR_DEPTH;
        prev->td_next = (uint32_t)td;
    }

    td->link = TD_PTR_TERMINATE;
    td->td_next = 0;

    td->cs = (3 << TD_CS_ERROR_SHIFT) | TD_CS_ACTIVE;
    if (speed == USB_SPEED_LOW) {
        td->cs |= TD_CS_LOW_SPEED;
    }

	td->token =
        (len << TD_TOK_MAXLEN_SHIFT) |
        (toggle << TD_TOK_D_SHIFT) |
        (endp << TD_TOK_ENDP_SHIFT) |
        (addr << TD_TOK_DEVADDR_SHIFT) |
        packet_type;

	td->buffer = (uint32_t)data;
}

static void uhci_init_qh(struct uhci_qh *qh, struct uhci_td *td) {
	qh->head = (uint32_t)td;
	qh->element = (uint32_t)td;
}

static void uhci_insert_qh(struct uhci_controller *uhc, struct uhci_qh *qh) {
	struct uhci_qh *list = uhc->qh_async;
	/* find last active qh, TODO: rewrite this place with dlist from embox */
	struct uhci_qh *end = (struct uhci_qh *)((char *)(list->qh_link.prev) -
			(unsigned long)(&(((struct uhci_qh*)0)->qh_link)));

	qh->head = TD_PTR_TERMINATE;
	end->head = (uint32_t)qh | TD_PTR_QH; /* add link to the qh in the end of the queue */

	/* add new qh in the end TODO: rewrite this place with dlist from embox */
	struct uhci_link *a = &list->qh_link;
	struct uhci_link *x = &qh->qh_link;

	struct uhci_link *p = a->prev;
	struct uhci_link *n = a;
	n->prev = x;
	x->next = n;
	x->prev = p;
	p->next = x;
}

static void uhci_remove_qh(struct uhci_qh *qh) {
	/* find last active qh, TODO: rewrite this place with dlist from embox */
	struct uhci_qh *prev = (struct uhci_qh *)((char *)(qh->qh_link.prev) -
			(unsigned long)(&(((struct uhci_qh*)0)->qh_link)));

	prev->head = qh->head;

	/* remove our qh TODO: rewrite this place with dlist from embox */
	struct uhci_link *x = &qh->qh_link;
	struct uhci_link *p = x->prev;
	struct uhci_link *n = x->next;
	n->prev = p;
	p->next = n;
	x->next = 0;
	x->prev = 0;
}

static int uhci_process_qh(struct uhci_controller *uhc, struct uhci_qh *qh) {
	struct uhci_td *td = (struct uhci_td *)(qh->element & ~0xf);
    if (!td) {
        return 1; /* all tds in queue are complete */
    }
    else if (~td->cs & TD_CS_ACTIVE) {
        if (td->cs & TD_CS_NAK) {
            printk("NAK\n");
        }

        if (td->cs & TD_CS_STALLED) {
            printk("TD is stalled\n");
        }

        if (td->cs & TD_CS_DATABUFFER) {
            printk("TD data buffer error\n");
        }
        if (td->cs & TD_CS_BABBLE) {
            printk("TD babble error\n");
        }
        if (td->cs & TD_CS_CRC_TIMEOUT) {
            printk("TD timeout error\n");
        }
        if (td->cs & TD_CS_BITSTUFF) {
            printk("TD bitstuff error\n");
        }
	}

    return 0;
}

static void uhci_wait_for_qh(struct uhci_controller *uhc, struct uhci_qh *qh) {
    int complete = 0;

    while (complete != 1) {
        complete = uhci_process_qh(uhc, qh);
    }
}

static void uhci_complete_control_request(struct uhci_qh *qh, struct uhci_td *head, struct usb_request *req) {
    struct uhci_td *td = head;
    struct uhci_td *remove;
    do {
		remove = td;
		req->actual_len += 8;
		td = (struct uhci_td *)(td->link & ~0xf);
		uhci_td_free(remove);
    } while(td->link != TD_PTR_TERMINATE);
	uhci_td_free(td);

    usb_request_complete(req);

    uhci_remove_qh(qh);
    uhci_qh_free(qh);
}

static int uhci_control_request(struct usb_request *req) {
	struct uhci_hcd *uhcd = hcd2uhci(req->endp->dev->hcd);
	struct uhci_controller *uhc = uhcd->uhc;

	/* Request properties */
	uint32_t speed = req->endp->dev->speed;
	uint32_t addr = 0;
	uint32_t endp = 0;
	uint32_t max_size = 8;
	uint32_t len = req->len;

	int toggle = 0;
	uint32_t packet_type = TD_PACKET_SETUP;
	uint32_t packet_size;

	struct uhci_td *test = uhci_alloc_td();
	uhci_td_free(test);

	/* Create queue of transfer descriptors */
	struct uhci_td *td = uhci_alloc_td();
	if (!td) {
		return 0;
	}

	struct uhci_td *head = td;
	struct uhci_td *prev = 0;

	/* Setup packet */
	uhci_init_td(td, prev, speed, addr, endp, toggle, packet_type, sizeof(req->ctrl_header), &req->ctrl_header);

	prev = td;

	/* Data in/out packets */

	if (len > 0) {
		packet_type = req->token & USB_TOKEN_OUT ? TD_PACKET_OUT : TD_PACKET_IN;

		uint8_t *tmp = (uint8_t *)req->buf;
    	uint8_t *end = tmp + len; /* end of buffer */
    	while (tmp < end) {
        	td = uhci_alloc_td();
        	if (!td) {
        		return 0;
        	}

			toggle ^= 1; /* switch toggle for every data td */
        	packet_size = end - tmp; /* size of remaining data */
        	if (packet_size > max_size) {
        		packet_size = max_size;
        	}

        	uhci_init_td(td, prev, speed, addr, endp, toggle, packet_type, packet_size, tmp);

        	tmp += packet_size;
        	prev = td;
		}
	}

	/* Status packet */
	td = uhci_alloc_td();
	if (!td) {
		return 0;
	}

	toggle = 1;
	packet_type = req->token & USB_TOKEN_OUT ? TD_PACKET_OUT : TD_PACKET_IN;
	uhci_init_td(td, prev, speed, addr, endp, toggle, packet_type, 0, 0);

	/* Initialize queue head */
	struct uhci_qh *qh = uhci_alloc_qh();
	uhci_init_qh(qh, head);

	/* Wait until queue has been processed */
	uhci_insert_qh(uhc, qh);

	uhci_wait_for_qh(uhc, qh);

	uhci_complete_control_request(qh, head, req);
	return 0;
}

static int uhci_common_request(struct usb_request *req) {
	struct uhci_hcd *uhcd = hcd2uhci(req->endp->dev->hcd);
	struct uhci_controller *uhc = uhcd->uhc;

	/* Request properties */
	uint32_t speed = req->endp->dev->speed;
	uint32_t addr = 0;
	uint32_t endp = req->endp->address;
	uint32_t len = req->len;
	int toggle = 0;
	uint32_t packet_type;

	struct uhci_td *td;

	packet_type = req->token & USB_TOKEN_OUT ? TD_PACKET_OUT : TD_PACKET_IN;

	td = uhci_alloc_td();
	if (!td) {
		return 0;
	}

	uhci_init_td(td, 0, speed, addr, endp, toggle, packet_type, len, req->buf);

	/* Initialize queue head */
	struct uhci_qh *qh = uhci_alloc_qh();
	uhci_init_qh(qh, td);

	/* Wait until queue has been processed */
	uhci_insert_qh(uhc, qh);

	uhci_wait_for_qh(uhc, qh);

	/* finish our request */
	uhci_td_free(td);

	req->actual_len += len;

	usb_request_complete(req);

	uhci_remove_qh(qh);
	uhci_qh_free(qh);
	return 0;
}

static int uhci_request_do(struct usb_request *req) {

	switch (req->endp->type) {
	case USB_COMM_CONTROL:
		uhci_control_request(req);
		break;
	case USB_COMM_BULK:
	case USB_COMM_INTERRUPT:
	case USB_COMM_ISOCHRON:
		uhci_common_request(req);
		break;
	default:
		panic("uhci_request: Unsupported enpd type %d", req->endp->type);
	}

	return 0;
}


static int uhci_request(struct usb_request *req) {

	return uhci_request_do(req);
}

static struct usb_hcd_ops uhci_hcd_ops = {
	.hcd_hci_alloc = uhci_hcd_alloc,
	.hcd_start = uhci_start,
	.hcd_stop = uhci_stop,
	.root_hub_control = uhci_root_hub_control,
	.request = uhci_request,
};

static uint16_t uhci_get_io_addr(struct pci_slot_dev *pci_dev, int bar_num) {
	int reg = PCI_BASE_ADDR_REG_0 + bar_num * sizeof(uint32_t);
	uint32_t addr;

	pci_read_config32(pci_dev->busn, pci_dev->slot << 3 | pci_dev->func,
			reg, &addr);

	if (~(addr) & UHCI_PCI_BAR_IO) {
			/* Support only I/O */
			return 0;
	}

	return (uint16_t)(addr & PCI_BASE_ADDR_IO_MASK);
}

PCI_DRIVER("Intel UHCI usb host", uhci_pci_init, PCI_VENDOR_ID_INTEL,
    PCI_DEV_ID_INTEL_82371SB_USB);

static int uhci_pci_init(struct pci_slot_dev *pci_dev) {
	struct usb_hcd *hcd;
	uint16_t io_addr;

	io_addr = uhci_get_io_addr(pci_dev, UHCI_BAR_NUMBER);

	if (!io_addr) {
		/* can't get addr */
		return -1;
	}

	hcd = usb_hcd_alloc(&uhci_hcd_ops, &io_addr);

	if (!hcd) {
		return -ENOMEM;
	}

	return usb_hcd_register(hcd);
}
