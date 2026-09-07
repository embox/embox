/**
 * @file
 * @brief Synopsys DesignWare Ethernet QoS (dwmac-4.x) MAC core
 *
 * Register interface of the DesignWare EQoS generation: MAC block at
 * 0x000, MTL queues at 0xd00, per-channel DMA at 0x1000 and MDIO
 * through the dwmac4 register layout.  Found in e.g. Rockchip
 * RK3568 (dwmac-4.20a, two instances gmac0 @0xFE2A0000 and gmac1
 * @0xFE010000, RTL8211F PHYs over RGMII).  This file covers only the
 * IP itself; board/SoC integration (clock trees, GRF iomux, PHY
 * reset GPIOs) goes through the soc_init/soc_swr_quirk hooks of
 * struct dwc_eqos_plat, so other SoCs reuse the core unchanged.
 *
 * Register names and field positions follow the DesignWare Ethernet
 * QoS user manual.  Note the older in-tree dwc_gmac driver targets
 * the previous DWMAC 3.x generation (different register map) and is
 * intentionally separate.
 */

#ifndef SRC_DRIVERS_NET_DWC_EQOS_DWC_EQOS_H_
#define SRC_DRIVERS_NET_DWC_EQOS_DWC_EQOS_H_

#include <stdint.h>

#define DWC_EQOS_MAC_CONFIGURATION           0x000
#define  DWC_EQOS_MAC_CONF_RE                (1u << 0)
#define  DWC_EQOS_MAC_CONF_TE                (1u << 1)
#define  DWC_EQOS_MAC_CONF_DCRS              (1u << 9)
#define  DWC_EQOS_MAC_CONF_DM                (1u << 13)
#define  DWC_EQOS_MAC_CONF_FES               (1u << 14)
#define  DWC_EQOS_MAC_CONF_PS                (1u << 15)
#define  DWC_EQOS_MAC_CONF_JE                (1u << 16)
#define  DWC_EQOS_MAC_CONF_JD                (1u << 17)
#define  DWC_EQOS_MAC_CONF_BE                (1u << 18)

#define DWC_EQOS_MAC_PACKET_FILTER           0x008
#define  DWC_EQOS_PKT_FLT_PR                 (1u << 0)

#define DWC_EQOS_MAC_Q0_TX_FLOW_CTRL         0x070
#define DWC_EQOS_MAC_RX_FLOW_CTRL            0x090
#define  DWC_EQOS_RX_FLOW_CTRL_RFE           (1u << 0)

#define DWC_EQOS_MAC_RXQ_CTRL0               0x0A0
#define  DWC_EQOS_RXQ_CTRL0_RXQ0EN_MASK      (3u << 0)
#define  DWC_EQOS_RXQ_CTRL0_RXQ0EN_DCB       (1u << 0)
#define DWC_EQOS_MAC_RXQ_CTRL1               0x0A4
#define  DWC_EQOS_RXQ_CTRL1_MCBCQEN          (1u << 20)

#define DWC_EQOS_MAC_1US_TIC_COUNTER         0x0DC

#define DWC_EQOS_MAC_VERSION                 0x110

#define DWC_EQOS_MAC_MDIO_ADDRESS            0x200
/* Field layout on this core: PA[24:21] RDA[20:16] CR[10:8] GOC[3:2] GB[0].
 * This is NOT the older dwmac3 layout (PA[20:16] RDA[15:11]). */
#define  DWC_EQOS_MDIO_PA_SHIFT              21
#define  DWC_EQOS_MDIO_PA_MASK               (0x1fu << 21)
#define  DWC_EQOS_MDIO_RDA_SHIFT             16
#define  DWC_EQOS_MDIO_RDA_MASK              (0x1fu << 16)
#define  DWC_EQOS_MDIO_CR_SHIFT              8
#define  DWC_EQOS_MDIO_CR_MASK               (0x7u << 8)
/* MDC divider: pclk_gmac runs at 100 MHz here, CR=4 selects the
 * 150-250 MHz range (divide by 12) which keeps MDC at ~8.3 MHz,
 * inside the 12.5 MHz limit of the RTL8211F. */
#define  DWC_EQOS_MDIO_CR_150_250M           0x4
#define  DWC_EQOS_MDIO_GOC_READ              (3u << 2)
#define  DWC_EQOS_MDIO_GOC_WRITE             (1u << 2)
#define  DWC_EQOS_MDIO_GB                    (1u << 0)
#define DWC_EQOS_MAC_MDIO_DATA               0x204

#define DWC_EQOS_MAC_ADDRESS0_HIGH           0x300
#define DWC_EQOS_MAC_ADDRESS0_LOW            0x304

#define DWC_EQOS_MAC_HW_FEATURE1             0x120
#define  DWC_EQOS_HW_TXFIFOSZ_SHIFT          6
#define  DWC_EQOS_HW_TXFIFOSZ_MASK           (0x1fu << 6)
#define  DWC_EQOS_HW_RXFIFOSZ_MASK           0x1fu

#define DWC_EQOS_MTL_TXQ0_OPERATION_MODE     0xD00
#define  DWC_EQOS_MTL_TXQ0_OP_TQS_SHIFT      16
#define  DWC_EQOS_MTL_TXQ0_OP_TQS_MASK       (0x1ffu << 16)
#define  DWC_EQOS_MTL_TXQ0_OP_TXQEN_EN       (2u << 2)
#define  DWC_EQOS_MTL_TXQ0_OP_TSF            (1u << 1)

#define DWC_EQOS_MTL_RXQ0_OPERATION_MODE     0xD30
#define  DWC_EQOS_MTL_RXQ0_OP_RQS_SHIFT      20
#define  DWC_EQOS_MTL_RXQ0_OP_RQS_MASK       (0x3ffu << 20)
#define  DWC_EQOS_MTL_RXQ0_OP_RSF            (1u << 5)
#define  DWC_EQOS_MTL_RXQ0_OP_FEP            (1u << 4)
#define  DWC_EQOS_MTL_RXQ0_OP_FUP            (1u << 3)

#define DWC_EQOS_DMA_MODE                    0x1000
#define  DWC_EQOS_DMA_MODE_SWR               (1u << 0)

#define DWC_EQOS_DMA_SYSBUS_MODE             0x1004
#define  DWC_EQOS_SYSBUS_WR_OSR_SHIFT        24
#define  DWC_EQOS_SYSBUS_RD_OSR_SHIFT        16
#define  DWC_EQOS_SYSBUS_EAME                (1u << 11)
#define  DWC_EQOS_SYSBUS_BLEN16              (1u << 3)
#define  DWC_EQOS_SYSBUS_BLEN8               (1u << 2)
#define  DWC_EQOS_SYSBUS_BLEN4               (1u << 1)

#define DWC_EQOS_DMA_CH0_CONTROL             0x1100
#define  DWC_EQOS_CH0_CTRL_PBLX8             (1u << 16)

#define DWC_EQOS_DMA_CH0_TX_CONTROL          0x1104
#define  DWC_EQOS_TX_CTRL_TXPBL_SHIFT        16
#define  DWC_EQOS_TX_CTRL_TXPBL_MASK         (0x3fu << 16)
#define  DWC_EQOS_TX_CTRL_OSP                (1u << 4)
#define  DWC_EQOS_TX_CTRL_ST                 (1u << 0)

#define DWC_EQOS_DMA_CH0_RX_CONTROL          0x1108
#define  DWC_EQOS_RX_CTRL_RXPBL_SHIFT        16
#define  DWC_EQOS_RX_CTRL_RXPBL_MASK         (0x3fu << 16)
#define  DWC_EQOS_RX_CTRL_RBSZ_SHIFT         1
#define  DWC_EQOS_RX_CTRL_RBSZ_MASK          (0x3fffu << 1)
#define  DWC_EQOS_RX_CTRL_SR                 (1u << 0)

#define DWC_EQOS_DMA_CH0_TX_BASE_ADDR_HI     0x1110
#define DWC_EQOS_DMA_CH0_TX_BASE_ADDR_LO     0x1114
#define DWC_EQOS_DMA_CH0_RX_BASE_ADDR_HI     0x1118
#define DWC_EQOS_DMA_CH0_RX_BASE_ADDR_LO     0x111C
#define DWC_EQOS_DMA_CH0_TX_END_ADDR         0x1120
#define DWC_EQOS_DMA_CH0_RX_END_ADDR         0x1128
#define DWC_EQOS_DMA_CH0_TX_RING_LEN         0x112C
#define DWC_EQOS_DMA_CH0_RX_RING_LEN         0x1130

#define DWC_EQOS_DMA_CH0_INTR_ENABLE         0x1134
/* Interrupt enable bits: on this silicon NIE/AIE are at 15/14
 * (dwmac4.10a layout); bit 16 is not writable. */
#define  DWC_EQOS_CH0_INTR_NIE               (1u << 15)
#define  DWC_EQOS_CH0_INTR_AIE               (1u << 14)
#define  DWC_EQOS_CH0_INTR_FBE               (1u << 12)
#define  DWC_EQOS_CH0_INTR_RIE               (1u << 6)
#define  DWC_EQOS_CH0_INTR_TIE               (1u << 0)
#define  DWC_EQOS_CH0_INTR_DEFAULT           (DWC_EQOS_CH0_INTR_NIE \
			| DWC_EQOS_CH0_INTR_AIE \
			| DWC_EQOS_CH0_INTR_FBE \
			| DWC_EQOS_CH0_INTR_RIE \
			| DWC_EQOS_CH0_INTR_TIE)

#define DWC_EQOS_DMA_CH0_STATUS              0x1160
#define  DWC_EQOS_CH0_STATUS_NIS             (1u << 15)
#define  DWC_EQOS_CH0_STATUS_AIS             (1u << 14)
#define  DWC_EQOS_CH0_STATUS_FB              (1u << 12)
#define  DWC_EQOS_CH0_STATUS_RI              (1u << 6)
#define  DWC_EQOS_CH0_STATUS_TI              (1u << 0)

/* 16-byte normal DMA descriptor ring (contiguous, no skip length).
 * TX: des2 = frame length [13:0] | IOC [31], des3 = OWN [31] | FD [29]
 * | LD [28] | frame length [14:0].  RX: des2 = 0, des3 = OWN [31] |
 * IOC [30] | BUF1V [24], writeback length in des3 [14:0]. */
struct dwc_eqos_desc {
	uint32_t des0;   /* buffer address bits[31:0] */
	uint32_t des1;   /* buffer address bits[47:32] (unused, 0) */
	uint32_t des2;   /* TX: frame length | IOC */
	uint32_t des3;   /* control / writeback status */
};

#define DWC_EQOS_DESC_TX_IOC                      (1u << 31)
#define DWC_EQOS_DESC_OWN                         (1u << 31)
#define DWC_EQOS_DESC_RX_IOC                      (1u << 30)
#define DWC_EQOS_DESC_TX_FD                       (1u << 29)
#define DWC_EQOS_DESC_TX_LD                       (1u << 28)
#define DWC_EQOS_DESC_RX_BUF1V                    (1u << 24)
#define DWC_EQOS_DESC_RX_LEN_MASK                 0x7fffu

/* RTL8211F: the PHY straps its internal RGMII TX-delay and a hardware
 * reset re-latches the strap.  Boards whose GRF delays are calibrated
 * with the PHY-side delay off need the strap bit cleared after every
 * reset pulse (page d08, register 0x11, bit 8); the core does it when
 * plat->clear_rtl8211f_tx_delay is set. */
#define RTL8211F_PHYSID1                 0x001c
#define RTL8211F_PHYSID2                 0xc916
#define RTL8211F_PAGESEL                 0x1f
#define RTL8211F_TXDELAY                 (1u << 8)

/* Per-instance configuration.  Everything IP-specific lives here;
 * SoC integration data is opaque (soc_data) and only touched by the
 * glue hooks, keeping the core reusable across SoCs. */
struct dwc_eqos_plat {
	int            idx;              /* per-driver instance slot */
	unsigned long  base_addr;        /* MAC register base */
	unsigned int   irq_num;          /* combined mac/dma IRQ (INTID) */
	const char     *mac_addr;        /* fixed MAC, "xx:xx:xx:xx:xx:xx" */
	/* Clear the strapped RTL8211F RGMII TX-delay after each hard
	 * reset (see the RTL8211F defines above). */
	int            clear_rtl8211f_tx_delay;

	/* SoC glue.  soc_init brings up clocks, pin iomux, RGMII
	 * delays and the PHY reset before the core touches the MAC;
	 * soc_swr_quirk (optional) works around clock-path quirks
	 * while DMA_MODE.SWR is asserted. */
	int  (*soc_init)(const struct dwc_eqos_plat *plat);
	void (*soc_swr_quirk)(const struct dwc_eqos_plat *plat, int enter);
	const void *soc_data;
};

extern int dwc_eqos_dev_init(const struct dwc_eqos_plat *plat);

#endif /* SRC_DRIVERS_NET_DWC_EQOS_DWC_EQOS_H_ */
