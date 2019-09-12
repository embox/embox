/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.10.2013
 */

#ifndef USB_OHCI_H_
#define USB_OHCI_H_

#define OHCI_USB_REV10 0x10 // for USB 1.0

#define OHCI_CTRL_CB_RATIO11          0x00000000
#define OHCI_CTRL_CB_RATIO21          0x00000001
#define OHCI_CTRL_CB_RATIO31          0x00000002
#define OHCI_CTRL_CB_RATIO41          0x00000003

#define OHCI_CTRL_LISTS_EN_MASK       0x0000003c
#define OHCI_CTRL_PERIOD_EN           0x00000004
#define OHCI_CTRL_ISOCHR_EN           0x00000008
#define OHCI_CTRL_CTRL_EN             0x00000010
#define OHCI_CTRL_BULK_EN             0x00000020

#define OHCI_CTRL_FUNC_STATE_MASK     0x000000c0
#define OHCI_CTRL_FUNC_STATE_RST      0x00000000
#define OHCI_CTRL_FUNC_STATE_RESM     0x00000040
#define OHCI_CTRL_FUNC_STATE_OPRT     0x00000080
#define OHCI_CTRL_FUNC_STATE_SUSP     0x000000c0

#define OHCI_CTRL_INT_ROUT            0x00000100

#define OHCI_CMD_OWNER_CHAGE          0x00000008
#define OHCI_CMD_RESET                0x00000001
#define OHCI_CMD_CONTROL_FILLED       0x00000002

#define OHCI_INTERRUPT_ALL_BUTSOF     0xc000007b
#define OHCI_INTERRUPT_RHUB           0x00000040
#define OHCI_INTERRUPT_DONE_QUEUE     0x00000002

#define OHCI_FM_INTERVAL_FI_MASK      0x00003fff
#define OHCI_FM_INTERVAL_FI_TOGL      0x80000000

#define OHCI_PERIOD_START_MASK        0x000003ff

#define OHCI_RH_DESC_A_N_DOWNP_M      0x000000ff

#define OHCI_RH_R_CONN_W_CLREN        0x0001
#define OHCI_RH_R_EN_W_STEN           0x0002
#define OHCI_RH_R_SUSP_W_STSUSP       0x0004
#define OHCI_RH_R_OVERC_W_CLSUSP      0x0008
#define OHCI_RH_R_RST_W_STRST         0x0010
#define OHCI_RH_R_PWR_W_STPWR         0x0100
#define OHCI_RH_R_LOWSPD_W_CLPWR      0x0200

#define OHCI_TD_SETUP                 0x00000000
#define OHCI_TD_OUT                   0x00080000
#define OHCI_TD_IN                    0x00100000
#define OHCI_TD_BUF_ROUND             0x00040000
#define OHCI_TD_FLAGS_CC_MASK         0xf0000000
#define OHCI_TD_CC_NOERR              0x00000000
#define OHCI_TD_CC_STALL              0x40000000
#define OHCI_TD_CC_NOT_RESP           0x50000000
#define OHCI_TD_CC_DATA_OVERRUN       0x80000000
#define OHCI_TD_CC_DATA_UNDERRUN      0x90000000

#define OHCI_ED_FUNC_ADDRESS_MASK     0x0000007f
#define OHCI_ED_FUNC_ADDRESS_OFFS     0
#define OHCI_ED_ENDP_ADDRESS_MASK     0x00000780
#define OHCI_ED_ENDP_ADDRESS_OFFS     7
#define OHCI_ED_DIR_MASK              0x00001800
#define OHCI_ED_S                     0x00002000
#define OHCI_ED_K                     0x00004000
#define OHCI_ED_F                     0x00008000
#define OHCI_ED_MAX_PKT_SIZE_MASK     0x07ff0000
#define OHCI_ED_SCHEDULED             0x80000000 // spec says some bits could be
                                                 // used by driver

#define OHCI_HCCA_INTERRUPT_LIST_N    32
#define OHCI_READ(ohcd, _reg) \
	REG_LOAD(_reg)

#define OHCI_WRITE(ohcd, _reg, _val) \
	do { \
		REG_STORE(_reg, (unsigned long) _val); \
	} while(0)

/* HCCAs */
struct ohci_hcca {
	uint32_t interrupt_table[OHCI_HCCA_INTERRUPT_LIST_N];
	uint16_t frame_number;
	uint16_t hcca_pad;
	uint32_t done_head;
	uint32_t reserved[29];
} __attribute__((aligned(256)));

/* OHCI Host Controller Driver */
struct ohci_reg {
	uint32_t hc_revision;
	uint32_t hc_control;
	uint32_t hc_cmdstat;
	uint32_t hc_intstat;
	uint32_t hc_inten;
	uint32_t hc_intdis;

	uint32_t hc_hcca;
	uint32_t hc_period_cur_ed;
	uint32_t hc_ctrl_head_ed;
	uint32_t hc_ctrl_cur_ed;
	uint32_t hc_bulk_head_ed;
	uint32_t hc_bulk_cur_ed;
	uint32_t hc_done_head;

	uint32_t hc_fm_interval;
	uint32_t hc_fm_remaining;
	uint32_t hc_fm_number;
	uint32_t hc_period_start;
	uint32_t hc_ls_threshold;

	uint32_t hc_rh_desc_a;
	uint32_t hc_rh_desc_b;
	uint32_t hc_rh_stat;
	uint32_t hc_rh_port_stat[];
} __attribute__((packed,aligned(sizeof(uint32_t))));

struct ohci_hcd {
	struct usb_hcd *hcd;
	struct ohci_hcca *hcca;
	struct ohci_reg *base;

};

static inline struct usb_hcd *ohci2hcd(struct ohci_hcd *ohcd) {
	return ohcd->hcd;
}

static inline struct ohci_hcd *hcd2ohci(struct usb_hcd *hcd) {
	return (struct ohci_hcd *) hcd->hci_specific;
}

struct ohci_ed {
	uint32_t flags;
	uint32_t tail_td;
	uint32_t head_td;
	uint32_t next_ed;
} __attribute__((packed,aligned(16)));


static inline struct ohci_ed *endp2ohci(struct usb_endp *endp) {
	return endp->hci_specific;
}

static inline struct usb_endp *ohci2endp(struct ohci_ed *ed) {
	assertf(0, "%s: NYI", __func__);
	return NULL;
}

struct ohci_td {
	uint32_t flags;
	uint32_t buf_p;
	uint32_t next_td;
	uint32_t buf_end;
	struct usb_request *req;
} __attribute__((packed,aligned(16)));

static inline struct usb_request *ohci2req(struct ohci_td *td) {
	return td->req;
}

#endif /* USB_OHCI_H_ */
