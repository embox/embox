/**
 * @file
 *
 * @date Aug 12, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_USB_HC_EHCI_H_
#define SRC_DRIVERS_USB_HC_EHCI_H_

#include <drivers/usb/usb.h>

#include "ehci_regs.h"

/* type tag from {qh,itd,sitd,fstn}->hw_next */
#define EHCI_Q_NEXT_TYPE(ehci, dma) ((dma) & 3 << 1)

/* values for that type tag */
#define EHCI_Q_TYPE_ITD  (0 << 1)
#define EHCI_Q_TYPE_QH   (1 << 1)
#define EHCI_Q_TYPE_SITD (2 << 1)
#define EHCI_Q_TYPE_FSTN (3 << 1)

/* next async queue entry, or pointer to interrupt/periodic QH */
#define EHCI_QH_NEXT(ehci, dma) ((((uint32_t) dma) & ~0x01f) | EHCI_Q_TYPE_QH)

/* for periodic/async schedules and qtd lists, mark end of list */
#define EHCI_LIST_END(ehci) (0x1) /* "null pointer" to hw */

#define EHCI_QTD_TOGGLE      (1 << 31)         /* data toggle */
#define EHCI_QTD_LENGTH(tok) (((tok)>>16) & 0x7fff)
#define EHCI_QTD_IOC         (1 << 15)         /* interrupt on complete */
#define EHCI_QTD_CERR(tok)   (((tok)>>10) & 0x3)
#define EHCI_QTD_PID(tok)    (((tok)>>8) & 0x3)
#define EHCI_QTD_STS_ACTIVE  (1 << 7)   /* HC may execute this */
#define EHCI_QTD_STS_HALT    (1 << 6)   /* halted on error */
#define EHCI_QTD_STS_DBE     (1 << 5)   /* data buffer error (in HC) */
#define EHCI_QTD_STS_BABBLE  (1 << 4)   /* device was babbling (qtd halted) */
#define EHCI_QTD_STS_XACT    (1 << 3)   /* device gave illegal response */
#define EHCI_QTD_STS_MMF     (1 << 2)   /* incomplete split transaction */
#define EHCI_QTD_STS_STS     (1 << 1)   /* split transaction state */
#define EHCI_QTD_STS_PING    (1 << 0)   /* issue PING? */

#define EHCI_QTD_PID_OUT     0
#define EHCI_QTD_PID_IN      1
#define EHCI_QTD_PID_SETUP   2

/*
* EHCI Specification 0.95 Section 3.5
* QTD: describe data transfer components (buffer, direction, ...)
* See Fig 3-6 "Queue Element Transfer Descriptor Block Diagram".
*
* These are associated only with "QH" (Queue Head) structures,
* used with control, bulk, and interrupt transfers.
*/
struct ehci_qtd_hw {
	/* first part defined by EHCI spec */
	uint32_t hw_next;        /* see EHCI 3.5.1 */
	uint32_t hw_alt_next;    /* see EHCI 3.5.2 */
	uint32_t hw_token;       /* see EHCI 3.5.3 */
	uint32_t hw_buf[5];      /* see EHCI 3.5.4 */
	uint32_t hw_buf_hi[5];   /* Appendix B */
} __attribute__((packed,aligned(32)));

/*
 * EHCI Specification 0.95 Section 3.6
 * QH: describes control/bulk/interrupt endpoints
 * See Fig 3-7 "Queue Head Structure Layout".
 *
 * These appear in both the async and (for interrupt) periodic schedules.
 */
#define EHCI_QH_CONTROL_EP (1 << 27)  /* FS/LS control endpoint */
#define EHCI_QH_HEAD       (1 << 15)  /* Head of async reclamation list */
#define EHCI_QH_TOGGLE_CTL (1 << 14)  /* Data toggle control */
#define EHCI_QH_HIGH_SPEED (2 << 12)  /* Endpoint speed */
#define EHCI_QH_LOW_SPEED  (1 << 12)
#define EHCI_QH_FULL_SPEED (0 << 12)
#define EHCI_QH_INACTIVATE (1 << 7)   /* Inactivate on next transaction */

#define EHCI_QH_SMASK      0x000000ff
#define EHCI_QH_CMASK      0x0000ff00
#define EHCI_QH_HUBADDR    0x007f0000
#define EHCI_QH_HUBPORT    0x3f800000
#define EHCI_QH_MULT       0xc0000000

/* first part defined by EHCI spec */
struct ehci_qh_hw {
	uint32_t hw_next;      /* see EHCI 3.6.1 */
	uint32_t hw_info1;     /* see EHCI 3.6.2 */

	uint32_t hw_info2;     /* see EHCI 3.6.2 */

	uint32_t hw_current;   /* qtd list - see EHCI 3.6.4 */

	/* qtd overlay (hardware parts of a struct ehci_qtd) */
	uint32_t hw_qtd_next;
	uint32_t hw_alt_next;
	uint32_t hw_token;
	uint32_t hw_buf[5];
	uint32_t hw_buf_hi[5];
} __attribute__((packed,aligned(32)));

/*
 * Entries in periodic shadow table are pointers to one of four kinds
 * of data structure.  That's dictated by the hardware; a type tag is
 * encoded in the low bits of the hardware's periodic schedule.  Use
 * Q_NEXT_TYPE to get the tag.
 *
 * For entries in the async schedule, the type tag always says "qh".
 */
union ehci_shadow {
	struct ehci_qh   *qh;   /* Q_TYPE_QH */
	struct ehci_itd  *itd;  /* Q_TYPE_ITD */
	struct ehci_sitd *sitd; /* Q_TYPE_SITD */
	struct ehci_fstn *fstn; /* Q_TYPE_FSTN */
	uint32_t *hw_next;  /* (all types) */
	void *ptr;
};

/*
 * EHCI Specification 0.96 Section 3.7
 * Periodic Frame Span Traversal Node (FSTN)
 *
 * Manages split interrupt transactions (using TT) that span frame boundaries
 * into uframes 0/1; see 4.12.2.2.  In those uframes, a "save place" FSTN
 * makes the HC jump (back) to a QH to scan for fs/ls QH completions until
 * it hits a "restore" FSTN; then it returns to finish other uframe 0/1 work.
 */
struct ehci_fstn {
	uint32_t hw_next; /* any periodic q entry */
	uint32_t hw_prev; /* qh or EHCI_LIST_END */

	/* the rest is HCD-private */
	uintptr_t fstn_dma;
	union ehci_shadow fstn_next; /* ptr to periodic q entry */
} __attribute__((packed,aligned(32)));

struct ehci_qh {
	struct ehci_qh_hw *hw; /* Must come first */
	/* the rest is HCD-private */
	uintptr_t qh_dma; /* address of qh */
	union ehci_shadow qh_next; /* ptr to qh; or periodic */
	struct usb_request *req;

	struct ehci_qtd_hw *qdt;
};

#define EHCI_ISOC_ACTIVE        (1<<31)        /* activate transfer this slot */
#define EHCI_ISOC_BUF_ERR       (1<<30)        /* Data buffer error */
#define EHCI_ISOC_BABBLE        (1<<29)        /* babble detected */
#define EHCI_ISOC_XACTERR       (1<<28)        /* XactErr - transaction error */
#define EHCI_ITD_LENGTH(tok)    (((tok)>>16) & 0x0fff)
#define EHCI_ITD_IOC            (1 << 15)      /* interrupt on complete */

/*
 * EHCI Specification 0.95 Section 3.3
 * Fig 3-4 "Isochronous Transaction Descriptor (iTD)"
 *
 * Schedule records for high speed iso xfers
 */
struct ehci_itd {
	/* first part defined by EHCI spec */
	uint32_t hw_next;           /* see EHCI 3.3.1 */
	uint32_t hw_transaction[8]; /* see EHCI 3.3.2 */


	uint32_t hw_bufp[7];        /* see EHCI 3.3.3 */
	uint32_t hw_bufp_hi[7];     /* Appendix B */

} __attribute__((packed,aligned(32)));

#define EHCI_SITD_IOC        (1 << 31) /* interrupt on completion */
#define EHCI_SITD_PAGE       (1 << 30) /* buffer 0/1 */
#define EHCI_SITD_LENGTH(x)  (((x) >> 16) & 0x3ff)
#define EHCI_SITD_STS_ACTIVE (1 << 7) /* HC may execute this */
#define EHCI_SITD_STS_ERR    (1 << 6) /* error from TT */
#define EHCI_SITD_STS_DBE    (1 << 5) /* data buffer error (in HC) */
#define EHCI_SITD_STS_BABBLE (1 << 4) /* device was babbling */
#define EHCI_SITD_STS_XACT   (1 << 3) /* illegal IN response */
#define EHCI_SITD_STS_MMF    (1 << 2) /* incomplete split transaction */
#define EHCI_SITD_STS_STS    (1 << 1) /* split transaction state */

/*
 * EHCI Specification 0.95 Section 3.4
 * siTD, aka split-transaction isochronous Transfer Descriptor
 *       ... describe full speed iso xfers through TT in hubs
 * see Figure 3-5 "Split-transaction Isochronous Transaction Descriptor (siTD)
 */
struct ehci_sitd {
	/* first part defined by EHCI spec */
	uint32_t hw_next;
/* uses bit field macros above - see EHCI 0.95 Table 3-8 */
	uint32_t hw_fullspeed_ep; /* EHCI table 3-9 */
	uint32_t hw_uframe;       /* EHCI table 3-10 */
	uint32_t hw_results;      /* EHCI table 3-11 */

	uint32_t hw_buf[2];       /* EHCI table 3-12 */
	uint32_t hw_backpointer;  /* EHCI table 3-13 */
	uint32_t hw_buf_hi[2];    /* Appendix B */

} __attribute__((packed,aligned(32)));

struct ehci_hcd {
	struct ehci_caps *ehci_caps;
	struct ehci_regs *ehci_regs;
	struct usb_hcd *usb_hcd;

	int has_ppcd;
	uint32_t command;

	struct pool *qh_pool;   /* qh per active urb */
	struct pool *qtd_pool;  /* one or more per qh */
	struct pool *itd_pool;  /* itd per iso urb */
	struct pool *sitd_pool; /* sitd per split iso urb */

	/* async schedule support */
	struct ehci_qh *async;

	/* periodic schedule support */
	int periodic_size;
	int i_thresh;
	uint32_t *periodic;
	uintptr_t periodic_dma;
};

static inline struct usb_hcd *ehci_to_hcd(struct ehci_hcd *ehci) {
	return ehci->usb_hcd;
}

static inline struct ehci_hcd *hcd_to_ehci(struct usb_hcd *hcd) {
	return (struct ehci_hcd *) hcd->hci_specific;
}

static inline uint32_t ehci_read(struct ehci_hcd *ehci_hdc, volatile uint32_t *regs) {
	(void)ehci_hdc;
	return *regs;
}

static inline void ehci_write(struct ehci_hcd *ehci_hdc, uint32_t val, volatile uint32_t *regs) {
	(void)ehci_hdc;
	*regs = val;
}

extern int ehci_hcd_register(void * base, unsigned int irq);
extern int ehci_mem_init(struct ehci_hcd *ehci_hcd);

extern int ehci_qtd_fill(struct ehci_hcd *ehci, struct ehci_qtd_hw *qtd, uintptr_t buf, size_t len, int token, int maxpacket);
extern struct ehci_qtd_hw *ehci_qtd_alloc(struct ehci_hcd *ehci);
extern void ehci_qtd_free(struct ehci_hcd *ehci, struct ehci_qtd_hw *qtd);
extern struct ehci_qh *ehci_qh_alloc(struct ehci_hcd *ehci);
extern void ehci_qh_free(struct ehci_hcd *ehci, struct ehci_qh *qh);
extern void scan_async (struct ehci_hcd *ehci);

#endif /* SRC_DRIVERS_USB_HC_EHCI_H_ */
