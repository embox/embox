/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.10.2013
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <util/math.h>
#include <lib/libds/bit.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <kernel/printk.h>
#include <mem/misc/pool.h>

#include <hal/mmu.h>
#include <drivers/common/memory.h>
#include <mem/vmem.h>

#include <drivers/usb/usb.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(usb_ti81xx_init);

#define TI81XX_USB_DEBUG 0
#if TI81XX_USB_DEBUG
#define DBG(x) x
#else
#define DBG(x)
#endif

#define TI8168_USBSS_BASE 0x47400000
#define TI8168_USBSS_SYSCONF (TI8168_USB0_BASE + 0x10)
#define TI81_USBSS_SYSCONFIG_RESET 0x00000001

#define TI8168_USB0_BASE (TI8168_USBSS_BASE + 0x1000)

#define TI8168_USBSS_IRQ 17
#define TI8168_USB0_IRQ 18

#define PRCM_BASE 0x48180000

#define PRCM_P_DEFAULT	(PRCM_BASE + 0xb00)
#define PRCM_P_DEFAULT_RSTCTL (PRCM_P_DEFAULT + 0x10)
#define PRCM_P_DEFAULT_RSTST (PRCM_P_DEFAULT + 0x14)

#define PRCM_C_DEFAULT	(PRCM_BASE + 0x500)
#define PRCM_C_DEFAULT_L3_SLOW_CLKCTRL (PRCM_C_DEFAULT + 0x14)
#define PRCM_C_DEFAULT_USB_CLKCTRL (PRCM_C_DEFAULT + 0x58)
#define PRCM_C_DEFAULT_USB_CLKCTRL_V_STBYIDLE (0x00070000)

#define PRCM_C_DEFAULT_V_CLKEN 0x2

#define CM_BASE		0x48140000
#define CM_USB0_CTRL	(CM_BASE + 0x620)
#define CM_USBPHY0_CTRL (CM_BASE + 0x624)

#define TI81_USB_CTRL_RST               0x00000001
#define TI81_USB_IRQ_STAT1_ALL_BUT_SOF  0x000003f7
#define TI81_USB_IRQ_STAT1_CONN         0x00000010
#define TI81_USB_IRQ_STAT1_DISCONN      0x00000020
#define TI81_USB_PHYUTMI_DEFAULT        0x00000020
#define TI81_USB_MODE_IDDIG             0x00000100
#define TI81_USB_DEVCTL_SESSION         0x01
#define TI81_USB_POWER_RESET            0x08
#define TI81_USB_FADDR_MASK             0x7f

#define TI81_USB_CSR0_CTRL_RX           0x0001
#define TI81_USB_CSR0_CTRL_TX           0x0002
#define TI81_USB_CSR0_CTRL_STALL        0x0004
#define TI81_USB_CSR0_CTRL_SETUP        0x0008
#define TI81_USB_CSR0_CTRL_ERROR        0x0010
#define TI81_USB_CSR0_CTRL_REQPKT       0x0020
#define TI81_USB_CSR0_CTRL_STATUS       0x0040
#define TI81_USB_CSR0_CTRL_NAK          0x0080
#define TI81_USB_CSR0_CTRL_FIFIFLUSH    0x0100
#define TI81_USB_CSR0_CTRL_DATATOG      0x0200
#define TI81_USB_CSR0_CTRL_DATATOG_WREN 0x0400
#define TI81_USB_CSR0_CTRL_DSPING       0x0800

#define TI81_USB_TX_CTRL_TX             0x0001
#define TI81_USB_TX_CTRL_FIFONEMPTY     0x0002
#define TI81_USB_TX_CTRL_ERROR          0x0004
#define TI81_USB_TX_CTRL_FIFOFLUSH      0x0008
#define TI81_USB_TX_CTRL_SETUP          0x0010
#define TI81_USB_TX_CTRL_STALL          0x0020
#define TI81_USB_TX_CTRL_CLRDATATOG     0x0040
#define TI81_USB_TX_CTRL_NAK            0x0080
#define TI81_USB_TX_CTRL_DATATOG        0x0100
#define TI81_USB_TX_CTRL_DATATOG_WREN   0x0200
#define TI81_USB_TX_CTRL_DMAMODE        0x0400
#define TI81_USB_TX_CTRL_FRC_DATATOG    0x0800
#define TI81_USB_TX_CTRL_DMAEN          0x1000
#define TI81_USB_TX_CTRL_MODE           0x2000
#define TI81_USB_TX_CTRL_AUTOSET        0x8000

#define TI81_USB_RX_CTRL_RX             0x0001
#define TI81_USB_RX_CTRL_FIFOFULL       0x0002
#define TI81_USB_RX_CTRL_ERROR          0x0004
#define TI81_USB_RX_CTRL_NAK            0x0008
#define TI81_USB_RX_CTRL_FIFOFLUSH      0x0010
#define TI81_USB_RX_CTRL_REQPKT         0x0020
#define TI81_USB_RX_CTRL_STALL          0x0040
#define TI81_USB_RX_CTRL_CLRDATATOG     0x0080
#define TI81_USB_RX_CTRL_DATATOG        0x0200
#define TI81_USB_RX_CTRL_DATATOG_WREN   0x0400
#define TI81_USB_RX_CTRL_DMAMODE        0x0800
#define TI81_USB_RX_CTRL_DISNYET        0x1000
#define TI81_USB_RX_CTRL_DMAEN          0x2000
#define TI81_USB_RX_CTRL_AUTOREQ        0x4000
#define TI81_USB_RX_CTRL_AUTOCLEAR      0x8000

#define TI81_USB_TYPE_HISPD             0x40
#define TI81_USB_TYPE_FLSPD             0x80
#define TI81_USB_TYPE_LOSPD             0xc0

#define TI81_USB_TYPE_CONTROL           0x00
#define TI81_USB_TYPE_ISCHR             0x10
#define TI81_USB_TYPE_BULK              0x20
#define TI81_USB_TYPE_INTERRUPT         0x30

#define TI81_USB_TYPE_ENDP_MASK         0x0f

#define TI81_USB_ENDP_N 15

#define TI81_USB_FIFO_SZ 512
#define TI81_USB_FIFO_SZ_RAW (bit_ctz(TI81_USB_FIFO_SZ) - 3)

#define TI81_USB_FIFO_RAM_DIV 8
#define TI81_USB_FIFO_SZ_EP0  64

struct ti81xx_endp_csr0 {
        uint16_t        pad0;
        uint16_t        csr0;
        uint16_t        pad1[2];
        uint16_t        count0;
        uint8_t         type0;
        uint8_t         naklim0;
        uint8_t         pad2[3];
        uint8_t         config;
} __attribute__((packed));

struct ti81xx_endp {
        uint16_t        tx_maxp;
        uint16_t        tx_csr;
        uint16_t        rx_maxp;
        uint16_t        rx_csr;
        uint16_t        rx_count;
        uint8_t         tx_type;
        uint8_t         tx_interv;
        uint8_t         rx_type;
        uint8_t         rx_interv;
        uint8_t         pad0[2];
} __attribute__((packed));

struct ti81xx_usb {
        uint32_t        rev;
        uint8_t         pad0[16];
        uint32_t        ctrl;
        uint32_t        stat;
        uint8_t         pad1[4];
        uint32_t        irq_stat;
        uint32_t        irq_eoi;
        uint32_t        irq_statraw0;
        uint32_t        irq_statraw1;
        uint32_t        irq_stat0;
        uint32_t        irq_stat1;
        uint32_t        irq_set0;
        uint32_t        irq_set1;
        uint32_t        irq_clear0;
        uint32_t        irq_clear1;
        uint8_t         pad2[40];
        uint32_t        txmode;
        uint32_t        rxmode;
        uint8_t         pad3[8];
        uint32_t        rndis[15];
        uint8_t         pad4[20];
        uint32_t        autoreq;
        uint32_t        srp_fix_time;
        uint32_t        teardown;
        uint8_t         pad5[4];
        uint32_t        phy_utmi;
        uint32_t        mgc_utmi;
        uint32_t        mode;
        uint8_t         pad6[20 + 3 * 0x100];
        /* mentor registers */
        uint8_t         m_faddr;
        uint8_t         m_power;
        uint16_t        m_intrtx;
        uint16_t        m_intrrx;
        uint16_t        m_intrtxe;
        uint16_t        m_intrrxe;
        uint8_t         m_intrusb;
        uint8_t         m_intrusbe;
        uint16_t        m_frame;
        uint8_t         m_index;
        uint8_t         m_testmode;
        uint8_t         pad7[0x10];
        uint32_t        fifo0;
        uint32_t        fifo[TI81_USB_ENDP_N];

        uint8_t         m_devctl;
        uint8_t         pad8[1];
        uint8_t         m_txfifo_sz;
        uint8_t         m_rxfifo_sz;
        uint16_t        m_txfifo_addr;
        uint16_t        m_rxfifo_addr;
        uint8_t         pad9[4];
        uint16_t        m_hwver;
        uint8_t         pad10[2 + 0x90];

        struct ti81xx_endp_csr0 csr0;
        struct ti81xx_endp csr[TI81_USB_ENDP_N];
} __attribute__((packed));

struct ti81xx_hcd_hci {
	struct ti81xx_usb *tiusb;
	struct usb_request *cendp_req;
	struct usb_request *endp_req[TI81_USB_ENDP_N];
};

POOL_DEF(hcd_hcis, struct ti81xx_hcd_hci, USB_MAX_HCD);
#if 0
static inline struct ti81xx_hcd_hci *usb2hcd(struct usb_hcd *hcd) {
	return hcd->hci_specific;
}

static irq_return_t ti81xx_irq(unsigned int irq_nr, void *data);

static inline struct ti81xx_usb *hcd2ti(struct usb_hcd *hcd) {
	return usb2hcd(hcd)->tiusb;
}

static void ti81xx_endp_fifo_init(struct ti81xx_usb *tiusb) {
	int i;
	unsigned fifo_ram = TI81_USB_FIFO_SZ_EP0 / TI81_USB_FIFO_RAM_DIV;

	for (i = 1; i < TI81_USB_ENDP_N ; i++) {
		REG8_STORE(&tiusb->m_index, i);

		REG16_STORE(&tiusb->m_txfifo_addr, fifo_ram);
		REG8_STORE(&tiusb->m_txfifo_sz, TI81_USB_FIFO_SZ_RAW);

		REG16_STORE(&tiusb->m_rxfifo_addr, fifo_ram);
		REG8_STORE(&tiusb->m_rxfifo_sz, TI81_USB_FIFO_SZ_RAW);

		fifo_ram += TI81_USB_FIFO_SZ / TI81_USB_FIFO_RAM_DIV;
	}
}

static void *ti81xx_hcd_alloc(struct usb_hcd *hcd, void *arg) {
	struct ti81xx_hcd_hci *hcdhci = pool_alloc(&hcd_hcis);

	memset(hcdhci, 0, sizeof(struct ti81xx_hcd_hci));
	hcdhci->tiusb = arg;

	hcd->root_hub = usb_hub_alloc(hcd, 1);

	return hcdhci;
}

static void ti81xx_hcd_free(struct usb_hcd *hcd, void *hci) {

	pool_free(&hcd_hcis, hci);
}

static int ti81xx_start(struct usb_hcd *hcd) {
	struct ti81xx_usb *tiusb = hcd2ti(hcd);
	int ret;

	assert(tiusb == (void *) 0x47401000);

	ret = irq_attach(TI8168_USB0_IRQ, ti81xx_irq, 0, hcd, "ti8168 usb0 irq");
	if (0 != ret) {
		return ret;
	}

	REG_ORIN(&tiusb->ctrl, TI81_USB_CTRL_RST);

	while((REG_LOAD(&tiusb->ctrl) & TI81_USB_CTRL_RST));

	REG_STORE(&tiusb->phy_utmi, TI81_USB_PHYUTMI_DEFAULT);

	{
		uint32_t regval = REG_LOAD(&tiusb->mode);
		regval &= ~TI81_USB_MODE_IDDIG;
		regval |= 0x80;
		REG_STORE(&tiusb->mode, regval);
	}

	REG_ORIN(0x48140620, 0x00000003);
	REG_ORIN(0x48140624, 0xf);

	REG_STORE(&tiusb->irq_set0, 0xffffffff);
	REG_STORE(&tiusb->irq_set1, TI81_USB_IRQ_STAT1_ALL_BUT_SOF);
	REG8_STORE(&tiusb->m_devctl, TI81_USB_DEVCTL_SESSION);
	REG8_STORE(&tiusb->m_intrusbe, 0xff);

	ti81xx_endp_fifo_init(tiusb);

	return 0;
}

static int ti81xx_stop(struct usb_hcd *hcd) {

	return 0;
}

static int ti81xx_rh_ctrl(struct usb_hub_port *port, enum usb_hub_request req,
			unsigned short value) {
	struct ti81xx_usb *tiusb = hcd2ti(port->hub->hcd);

	DBG(printk("%s: request=%d value=%x\n", __func__, req, value);)

	if (value & USB_HUB_PORT_RESET) {
		if (req == USB_HUB_REQ_PORT_SET) {
			REG8_ORIN(&tiusb->m_power, TI81_USB_POWER_RESET);
		} else {
			REG8_ANDIN(&tiusb->m_power, ~TI81_USB_POWER_RESET);
		}

		return 0;
	}

	return -ENOTSUP;
}

static void ti_csr_write(uint16_t *ctrl_reg, uint16_t or_val, uint16_t and_val) {
	uint16_t ctrl_val;

	ctrl_val = REG16_LOAD(ctrl_reg);

	DBG(printk("%s: ctrl=%08x or=%08x and=%08x\n", __func__, ctrl_val,
		       or_val, and_val);)

	ctrl_val &= and_val;
	ctrl_val |= or_val;
	REG16_STORE(ctrl_reg, ctrl_val);
}

static void ti_fifo_write(uint32_t *fifo, struct usb_request *req) {
	char *td;
	size_t tl;

	DBG(printk("%s: data=", __func__);)
	for (td = req->buf, tl = req->len; tl > 0; td++, tl--) {
		REG8_STORE(fifo, *td);
		DBG(printk(" %02x", *td);)
	}
	DBG(printk("\n");)

	req->len = 0;

}

#if TI81XX_USB_DEBUG
static void ti_fifo_val_print(uint32_t fifo, size_t fifo_len) {

	for (; fifo_len > 0; fifo >>= 8, fifo_len --) {
		printk(" %02x", fifo & 0xff);
	}
}
#endif

static void ti_fifo_do_read(uint32_t *fifo, uint16_t *count, struct usb_request *req) {
	size_t tl, fifo_len;
	uint32_t fifo_val;

	fifo_len = REG16_LOAD(count);

	DBG(printk("%s: req->buf=%p, req->len=%d, fifo=%p, count=%d, data=",
			__func__, req->buf, (int) req->len, fifo, (int) fifo_len);)

	for (tl = min(fifo_len / sizeof(uint32_t), req->len / sizeof(uint32_t));
			tl > 0;
			req->len -= sizeof(uint32_t), fifo_len -= sizeof(uint32_t), tl--) {

		fifo_val = REG_LOAD(fifo);
		req->buf = sizeof(uint32_t) + memcpy(req->buf, &fifo_val, sizeof(uint32_t));

		DBG(ti_fifo_val_print(fifo_val, sizeof(uint32_t));)
	}

	tl = min(req->len, fifo_len);
	if (tl > 0) {
		fifo_val = REG_LOAD(fifo);

		DBG(ti_fifo_val_print(fifo_val, tl);)
		req->buf = tl + memcpy(req->buf, &fifo_val, tl);
		req->len -= tl;
	}

	DBG(printk("\n");)
}

enum ti81xx_endp_type {
	TI81_ENDP_CTRL,
	TI81_ENDP_RX,
	TI81_ENDP_TX,
};

static enum usb_request_status ti81xx_endp_status_map(enum ti81xx_endp_type type,
		uint16_t csr, uint16_t *errmask) {

	*errmask = 0;
	switch(type) {
	case TI81_ENDP_CTRL:
		*errmask = TI81_USB_CSR0_CTRL_STALL | TI81_USB_CSR0_CTRL_ERROR
			| TI81_USB_CSR0_CTRL_NAK | TI81_USB_CSR0_CTRL_STATUS;

		if (csr & TI81_USB_CSR0_CTRL_STALL) {
			return USB_REQ_STALL;
		}
		if (csr & TI81_USB_CSR0_CTRL_ERROR) {
			return USB_REQ_INTERR;
		}
		if (csr & TI81_USB_CSR0_CTRL_NAK) {
			return USB_REQ_INTERR;
		}
		break;

	case TI81_ENDP_RX:
		*errmask = TI81_USB_RX_CTRL_STALL | TI81_USB_RX_CTRL_ERROR
			| TI81_USB_RX_CTRL_NAK;

		if (csr & TI81_USB_RX_CTRL_STALL) {
			return USB_REQ_STALL;
		}
		if (csr & TI81_USB_RX_CTRL_ERROR) {
			return USB_REQ_INTERR;
		}
		if (csr & TI81_USB_RX_CTRL_NAK) {
			return USB_REQ_INTERR;
		}
		break;

	case TI81_ENDP_TX:
		*errmask = TI81_USB_TX_CTRL_STALL | TI81_USB_TX_CTRL_ERROR
			| TI81_USB_TX_CTRL_NAK;

		if (csr & TI81_USB_TX_CTRL_STALL) {
			return USB_REQ_STALL;
		}
		if (csr & TI81_USB_TX_CTRL_ERROR) {
			return USB_REQ_INTERR;
		}
		if (csr & TI81_USB_TX_CTRL_NAK) {
			return USB_REQ_INTERR;
		}
		break;
	}

	return USB_REQ_NOERR;
}

static void ti81xx_csr_map(enum ti81xx_endp_type type, unsigned short token,
		uint16_t *or_mask, uint16_t *and_mask) {
	uint16_t or = 0;
	uint16_t and = ~0;

	switch(type) {
	case TI81_ENDP_CTRL:
		if (token & USB_TOKEN_SETUP) {
			or |= TI81_USB_CSR0_CTRL_SETUP;
		}
		if (token & USB_TOKEN_IN) {
			or |= TI81_USB_CSR0_CTRL_REQPKT;
		}
		if (token & USB_TOKEN_OUT) {
			or |= TI81_USB_CSR0_CTRL_TX;
		}
		if (token & USB_TOKEN_STATUS) {
			or |= TI81_USB_CSR0_CTRL_STATUS;
		}
		break;

	case TI81_ENDP_RX:
		if (token & USB_TOKEN_IN) {
			or |= TI81_USB_RX_CTRL_REQPKT;
		}
		/*or |= TI81_USB_RX_CTRL_DISNYET;*/
		assert(!(token & (USB_TOKEN_SETUP | USB_TOKEN_OUT | USB_TOKEN_STATUS)));
		break;

	case TI81_ENDP_TX:
		if (token & USB_TOKEN_SETUP) {
			or |= TI81_USB_TX_CTRL_SETUP;
		}
		if (token & USB_TOKEN_OUT) {
			or |= TI81_USB_TX_CTRL_TX;
		}
		assert(!(token & USB_TOKEN_IN));
		break;
	}

	*or_mask = or;
	*and_mask = and;
}

static uint8_t ti81xx_speed_map(struct usb_endp *endp) {

	return TI81_USB_TYPE_FLSPD;
}

static uint8_t ti81xx_proto_map(enum usb_comm_type type) {

	switch(type) {
	case USB_COMM_CONTROL:
		return TI81_USB_TYPE_CONTROL;
	case USB_COMM_INTERRUPT:
		return TI81_USB_TYPE_INTERRUPT;
	case USB_COMM_BULK:
		return TI81_USB_TYPE_BULK;
	case USB_COMM_ISOCHRON:
		return TI81_USB_TYPE_ISCHR;
	}

	/* NOTREACHED */
	return 0;
}

static void ti81xx_endp_req_bind(struct ti81xx_hcd_hci *hcdhci,
		enum ti81xx_endp_type endp_type, int host_endp_n,
		struct usb_request *req) {

	if (endp_type == TI81_ENDP_CTRL) {
		assert(hcdhci->cendp_req == NULL);
		hcdhci->cendp_req = req;
	} else {
		assert(host_endp_n < TI81_USB_ENDP_N);
		assert(hcdhci->endp_req[host_endp_n] == NULL);
		hcdhci->endp_req[host_endp_n] = req;
	}
}

static struct usb_request *ti81xx_endp_req_unbind(struct ti81xx_hcd_hci *hcdhci,
		enum ti81xx_endp_type endp_type, int host_endp_n) {
	struct usb_request *req;

	if (endp_type == TI81_ENDP_CTRL) {
		req = hcdhci->cendp_req;
		hcdhci->cendp_req = NULL;
	} else {
		assert(host_endp_n < TI81_USB_ENDP_N);
		req = hcdhci->endp_req[host_endp_n];
		hcdhci->endp_req[host_endp_n] = NULL;
	}

	return req;
}


static int ti81xx_request0(struct usb_request *req) {
	struct usb_endp *endp = req->endp;
	struct ti81xx_usb *tiusb = hcd2ti(endp->dev->hcd);
	struct ti81xx_hcd_hci *hcdhci = usb2hcd(endp->dev->hcd);
	uint16_t or_mask, and_mask;

	DBG(printk("%s: ", __func__);)

	ti81xx_endp_req_bind(hcdhci, TI81_ENDP_CTRL, 0, req);

	ti81xx_csr_map(TI81_ENDP_CTRL, req->token, &or_mask, &and_mask);

	if (req->token & USB_TOKEN_OUT) {
		ti_fifo_write(&tiusb->fifo0, req);
		ti_csr_write(&tiusb->csr0.csr0, or_mask, and_mask);
	} else {
		ti_csr_write(&tiusb->csr0.csr0, or_mask, and_mask);
	}

	return 0;
}

static void ti81xx_endp_setup(struct usb_endp *endp,
		uint8_t *type, uint16_t *maxp, uint8_t *interval) {

	DBG(printk("%s: endp type=%d addr=%d, maxp=%d\n", __func__,
				endp->type, endp->address, endp->max_packet_size);)
	REG8_STORE(type, ti81xx_speed_map(endp)
			| ti81xx_proto_map(endp->type)
			| (endp->address & TI81_USB_TYPE_ENDP_MASK));

	REG16_STORE(maxp, endp->max_packet_size);

	REG8_STORE(interval, 0);
}

static int ti81xx_request_rx(struct usb_request *req, int host_endp_n) {
	struct ti81xx_usb *tiusb = hcd2ti(req->endp->dev->hcd);
	struct ti81xx_endp *tiendp = &tiusb->csr[host_endp_n];
	uint16_t or_mask, and_mask;

	DBG(printk("%s: ", __func__);)

	ti_csr_write(&tiendp->tx_csr, 0, (uint16_t) ~TI81_USB_TX_CTRL_MODE);

	ti81xx_endp_setup(req->endp, &tiendp->rx_type, &tiendp->rx_maxp,
			&tiendp->rx_interv);

	ti81xx_csr_map(TI81_ENDP_RX, req->token, &or_mask, &and_mask);

	ti_csr_write(&tiendp->rx_csr, or_mask, and_mask);

	return 0;
}

static int ti81xx_request_tx(struct usb_request *req, int host_endp_n) {
	struct ti81xx_usb *tiusb = hcd2ti(req->endp->dev->hcd);
	struct ti81xx_endp *tiendp = &tiusb->csr[host_endp_n];
	uint16_t or_mask, and_mask;

	DBG(printk("%s: ", __func__);)

	ti_csr_write(&tiendp->tx_csr, TI81_USB_TX_CTRL_MODE,
			(uint16_t) ~(TI81_USB_TX_CTRL_FRC_DATATOG | TI81_USB_TX_CTRL_AUTOSET));

	ti81xx_endp_setup(req->endp, &tiendp->tx_type, &tiendp->tx_maxp,
			&tiendp->tx_interv);

	ti81xx_csr_map(TI81_ENDP_TX, req->token, &or_mask, &and_mask);

	ti_fifo_write(&tiusb->fifo[host_endp_n], req);
	ti_csr_write(&tiendp->tx_csr, or_mask, and_mask);

	return 0;
}

static int ti81xx_request(struct usb_request *req) {
	struct usb_endp *endp = req->endp;
	struct ti81xx_usb *tiusb = hcd2ti(endp->dev->hcd);
	struct ti81xx_hcd_hci *hcdhci = usb2hcd(endp->dev->hcd);
	int host_endp_n;

	/* lock ? */

	DBG(printk("%s: bus_idx=%d\n", __func__, endp->dev->bus_idx);)
	REG8_STORE(&tiusb->m_faddr, (uint8_t) endp->dev->bus_idx & TI81_USB_FADDR_MASK);

	if (endp->address == 0) {
		return ti81xx_request0(req);
	}

#if 0
	host_endp_n = endp->direction == USB_DIRECTION_IN ?
		0 : 1;
#else
	for (host_endp_n = 0; host_endp_n < TI81_USB_ENDP_N; host_endp_n++) {
		if (NULL == hcdhci->endp_req[host_endp_n]) {
			break;
		}
	}
#endif

	assertf(host_endp_n < TI81_USB_ENDP_N, "no idle endpoint right now");
	ti81xx_endp_req_bind(hcdhci, TI81_ENDP_RX, host_endp_n, req);

	if (endp->direction == USB_DIRECTION_IN) {
		return ti81xx_request_rx(req, host_endp_n);
	}

	return ti81xx_request_tx(req, host_endp_n);
}

static struct usb_hcd_ops ti81xx_hcd_ops = {
	.hcd_start = ti81xx_start,
	.hcd_stop = ti81xx_stop,
	.hcd_hci_alloc = ti81xx_hcd_alloc,
	.hcd_hci_free = ti81xx_hcd_free,
	.rhub_ctrl = ti81xx_rh_ctrl,
	.request = ti81xx_request,
};

static void ti81xx_port_stat_map(struct ti81xx_usb *tiusb, uint32_t intrstat1,
		struct usb_hub_port *port) {

	port->status = USB_HUB_PORT_POWER | USB_HUB_PORT_ENABLE;
	if (intrstat1 & TI81_USB_IRQ_STAT1_CONN) {
		port->status |= USB_HUB_PORT_CONNECT;
	}
	port->changed |= USB_HUB_PORT_CONNECT;
}

static void ti81xx_irq_generic_endp(uint16_t *csr,
		uint16_t *count, uint32_t *fifo, uint16_t read_done_mask,
		struct ti81xx_hcd_hci *hcdhci, enum ti81xx_endp_type endp_type,
	       	int endp_n) {
	struct usb_request *req;
	uint16_t csr_v;
	uint16_t errmask;

	assertf((count && fifo && read_done_mask)
			|| (!count && !fifo && !read_done_mask),
			"count, fifo, read_done_mask must be specified or "
			"unspecified simultaneously");

	req = ti81xx_endp_req_unbind(hcdhci, endp_type, endp_n);
	assert(req);

	csr_v = REG16_LOAD(csr);

	req->req_stat = ti81xx_endp_status_map(endp_type, csr_v, &errmask);
	DBG(printk("%s: csr_v=%04x errmask=%04x\n", __func__, csr_v, errmask);)
	if (req->req_stat != USB_REQ_NOERR) {
		printk("%s: csr=%p csr_v=%04x errmask=%04x\n", __func__, csr, csr_v, errmask);
		printk("%s: req=%p hnd=%p req_stat=%d count=%d\n", __func__,
				req, req->notify_hnd, req->req_stat,
					REG16_LOAD(count));
		/*return;*/
	}

	if (csr_v & read_done_mask) {
		ti_fifo_do_read(fifo, count, req);
	}

	ti_csr_write(csr, 0, ~(errmask | read_done_mask));

	usb_request_complete(req);
}

static void ti81xx_irq_cendp(struct ti81xx_hcd_hci *hcdhci,
		struct ti81xx_usb *tiusb) {
	struct ti81xx_endp_csr0 *tiendp0 = &tiusb->csr0;

	DBG(printk("%s\n", __func__);)

	ti81xx_irq_generic_endp(&tiendp0->csr0, &tiendp0->count0, &tiusb->fifo0,
			TI81_USB_CSR0_CTRL_RX, hcdhci, TI81_ENDP_CTRL, 0);
}

static void ti81xx_irq_txendp(struct ti81xx_hcd_hci *hcdhci,
		struct ti81xx_usb *tiusb, int i) {
	struct ti81xx_endp *tiendp = &tiusb->csr[i];

	DBG(printk("%s: %d\n", __func__, i);)

	ti81xx_irq_generic_endp(&tiendp->tx_csr, NULL, NULL, 0,
			hcdhci, TI81_ENDP_TX, i);
}

static void ti81xx_irq_rxendp(struct ti81xx_hcd_hci *hcdhci,
		struct ti81xx_usb *tiusb, int i) {
	struct ti81xx_endp *tiendp = &tiusb->csr[i];

	DBG(printk("%s: %d\n", __func__, i);)

	ti81xx_irq_generic_endp(&tiendp->rx_csr, &tiendp->rx_count,
			&tiusb->fifo[i], TI81_USB_RX_CTRL_RX, hcdhci,
			TI81_ENDP_RX, i);
}

static irq_return_t ti81xx_irq(unsigned int irq_nr, void *data) {
	struct usb_hcd *hcd = data;
	struct ti81xx_usb *tiusb = hcd2ti(hcd);
	struct ti81xx_hcd_hci *hcdhci = usb2hcd(hcd);
	uint32_t intrstat0, intrstat1;
#if TI81XX_USB_DEBUG
	uint8_t m_intrstat;

	m_intrstat = REG8_LOAD(&tiusb->m_intrusb);
#endif

	intrstat0 = REG_LOAD(&tiusb->irq_stat0);
	intrstat1 = REG_LOAD(&tiusb->irq_stat1);

	REG_STORE(&tiusb->irq_stat0, intrstat0);
	REG_STORE(&tiusb->irq_stat1, intrstat1);

	REG_STORE(&tiusb->irq_eoi, 1);

	DBG(printk("%s: stat=0x%08lx devctl=0x%08lx m_intrstat=0x%02x "
				"intrstat0=0x%08lx intrstat1=0x%08lx\n",
			__func__,
			REG_LOAD(&tiusb->stat),
			REG_LOAD(&tiusb->m_devctl),
			m_intrstat,
			(unsigned long) intrstat0,
			(unsigned long) intrstat1);)

	if (intrstat0) {
		if (intrstat0 & 0x01) {
			ti81xx_irq_cendp(hcdhci, tiusb);
		}

		for (int i = 1; i < 16; i++) {
			if (intrstat0 & (1 << i)) {
				ti81xx_irq_txendp(hcdhci, tiusb, i - 1);
			}
		}

		for (int i = 17; i < 32; i++) {
			if (intrstat0 & (1 << i)) {
				ti81xx_irq_rxendp(hcdhci, tiusb, i - 17);
			}
		}
	}


	if (intrstat1 & (TI81_USB_IRQ_STAT1_CONN | TI81_USB_IRQ_STAT1_DISCONN)) {
		struct usb_hub_port *port = &hcd->root_hub->ports[0];
		ti81xx_port_stat_map(tiusb, intrstat1, port);
		usb_rh_nofity(hcd);
	}

	return IRQ_HANDLED;
}
#endif
static int usb_ti81xx_init(void) {
#if 0
	struct usb_hcd *hcd;

	static_assert(offsetof(struct ti81xx_usb, m_devctl) == 0x460, "");
	static_assert(sizeof(struct ti81xx_endp_csr0) == 16, "");
	static_assert(offsetof(struct ti81xx_endp_csr0, count0) == 0x08, "");
	static_assert(sizeof(struct ti81xx_endp) == 16, "");
	static_assert(offsetof(struct ti81xx_usb, m_hwver) == 0x46c, "");
	static_assert(offsetof(struct ti81xx_usb, pad10) == 0x46e, "");
	static_assert(offsetof(struct ti81xx_usb, csr0) == 0x500, "");
	static_assert(offsetof(struct ti81xx_usb, csr[0]) == 0x510, "");
	static_assert(offsetof(struct ti81xx_usb, m_index) == 0x40e, "");

	REG_ANDIN(PRCM_P_DEFAULT_RSTCTL, 0xffffff9f);

	while ((REG_LOAD(PRCM_P_DEFAULT_RSTST) & 0x60) != 0x60);

	REG_STORE(PRCM_P_DEFAULT_RSTST, 0x60);

	REG_STORE(PRCM_C_DEFAULT_L3_SLOW_CLKCTRL, PRCM_C_DEFAULT_V_CLKEN);
	REG_STORE(PRCM_C_DEFAULT_USB_CLKCTRL, PRCM_C_DEFAULT_V_CLKEN);

	while ((REG_LOAD(PRCM_C_DEFAULT_USB_CLKCTRL)
				& PRCM_C_DEFAULT_USB_CLKCTRL_V_STBYIDLE)
			== PRCM_C_DEFAULT_USB_CLKCTRL_V_STBYIDLE);

	REG_STORE(TI8168_USBSS_SYSCONF, TI81_USBSS_SYSCONFIG_RESET);
	while (REG_LOAD(TI8168_USBSS_SYSCONF) & TI81_USBSS_SYSCONFIG_RESET);

	hcd = usb_hcd_alloc(&ti81xx_hcd_ops, (void *) TI8168_USB0_BASE);
	if (!hcd) {
		return -ENOMEM;
	}

	return usb_hcd_register(hcd);
#endif
	return 0;
}

PERIPH_MEMORY_DEFINE(hc_ti816x, PRCM_BASE, 0x1000);

PERIPH_MEMORY_DEFINE(ti816x_cm, CM_BASE, 0x1000);

PERIPH_MEMORY_DEFINE(ti816x_usb0, TI8168_USB0_BASE, 0x1000);
