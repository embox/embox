/**
 * @file
 * @brief BCM2711 GENET v5 register map
 *
 * Offsets taken from U-Boot's bcmgenet.c. The block is split into sub-areas
 * at fixed offsets, and every register below is written absolute so there is
 * one number to check against the datasheet rather than two to add up:
 *
 *   SYS   0x0000    UMAC  0x0800    RBUF  0x0300    EXT   0x0080
 *
 * MDIO lands at 0x0E14, which is what the device tree calls mdio@e14.
 */

#ifndef BCM2711_GENET_REGS_H_
#define BCM2711_GENET_REGS_H_

#include <stdint.h>

#include <hal/mem_barriers.h>
#include <hal/reg.h>

/* SYS */
#define GENET_SYS_REV_CTRL        0x0000
#define GENET_SYS_PORT_CTRL       0x0004
#define GENET_PORT_MODE_EXT_GPHY  3
#define GENET_SYS_RBUF_FLUSH_CTRL 0x0008
#define GENET_SYS_TBUF_FLUSH_CTRL 0x000c

/* The revision register's major field needs a lookup, not a comparison:
 * GENET v5 reports 6 here. U-Boot maps 6->5, 5->4, 0->1. */
#define GENET_MAJOR(rev) (((rev) >> 24) & 0x0f)
#define GENET_MINOR(rev) (((rev) >> 16) & 0x0f)

/* EXT. The RGMII block has to be taken out of out-of-band control and
 * enabled before the PHY's clock reaches the MAC. ID_MODE_DIS *disables* the
 * MAC's internal transmit delay, and on this board it is set; see
 * genet_rgmii_init(). */
#define GENET_EXT_RGMII_OOB_CTRL 0x008c
#define GENET_RGMII_LINK         (1u << 4)
#define GENET_OOB_DISABLE        (1u << 5)
#define GENET_RGMII_MODE_EN      (1u << 6)
#define GENET_ID_MODE_DIS        (1u << 16)

/* RBUF */
#define GENET_RBUF_CTRL           0x0300
#define GENET_RBUF_ALIGN_2B       (1u << 1)
#define GENET_RBUF_TBUF_SIZE_CTRL 0x03b4

/* UMAC */
#define GENET_UMAC_CMD           0x0808
#define GENET_CMD_SW_RESET       (1u << 13)
#define GENET_CMD_LCL_LOOP_EN    (1u << 15)
#define GENET_UMAC_MAC0          0x080c
#define GENET_UMAC_MAC1          0x0810
#define GENET_UMAC_MAX_FRAME_LEN 0x0814
#define GENET_UMAC_MIB_CTRL      0x0d80

/* The MIB counter block; only its reset bits are used here. */
#define GENET_MIB_RESET_RX   (1u << 0)
#define GENET_MIB_RESET_RUNT (1u << 1)
#define GENET_MIB_RESET_TX   (1u << 2)

/* MDIO */
#define GENET_MDIO_CMD        0x0e14
#define GENET_MDIO_START_BUSY (1u << 29)
#define GENET_MDIO_READ_FAIL  (1u << 28)
#define GENET_MDIO_RD         (2u << 26)
#define GENET_MDIO_WR         (1u << 26)
#define GENET_MDIO_PMD_SHIFT  21
#define GENET_MDIO_REG_SHIFT  16

#define MII_BCM54XX_AUXCTL               0x18
#define BCM54XX_AUXCTL_SHDWSEL_MISC      0x0007
#define BCM54XX_AUXCTL_READ_SHIFT        12
#define BCM54XX_AUXCTL_MISC_WREN         0x8000
#define BCM54XX_AUXCTL_MISC_RXD_RXC_SKEW 0x0100

/* Shadow registers (0x1c), a second window. 0x03 holds the PHY's own
 * transmit clock delay, read but never written here: this board's transmit
 * works without touching it. */
#define MII_BCM54XX_SHD                0x1c
#define BCM54XX_SHD_WRITE              0x8000
#define BCM54XX_SHD_SEL(x)             (((x) & 0x1f) << 10)
#define BCM54XX_SHD_DATA(x)            ((x) & 0x3ff)
#define BCM54810_SHD_CLK_CTL           0x03
#define BCM54810_SHD_CLK_CTL_GTXCLK_EN (1u << 9)

/* Generic MII registers, for the PHY behind that MDIO */
#define MII_BMCR     0x00
#define MII_BMSR     0x01
#define MII_PHYSID1  0x02
#define MII_PHYSID2  0x03
#define MII_ANAR     0x04
#define MII_LPA      0x05
#define MII_CTRL1000 0x09
#define MII_STAT1000 0x0a

#define BMCR_ANRESTART (1u << 9)
#define BMCR_ANENABLE  (1u << 12)
#define BMCR_RESET     (1u << 15)

/* BMSR bit 2 is latching-low: one read reports the state since the last read
 * and clears it, so the current link state is only in a second read. */
#define BMSR_LINK     (1u << 2)
#define BMSR_ANEGCAPS (1u << 3)
#define BMSR_ANEGDONE (1u << 5)

#define ANAR_10HD  (1u << 5)
#define ANAR_10FD  (1u << 6)
#define ANAR_100HD (1u << 7)
#define ANAR_100FD (1u << 8)
#define ANAR_802_3 (1u << 0)

#define CTRL1000_1000HD (1u << 8)
#define CTRL1000_1000FD (1u << 9)

#define STAT1000_1000HD (1u << 10)
#define STAT1000_1000FD (1u << 11)

/* UMAC_CMD, the bits that matter for bringing a link into service */
#define GENET_CMD_TX_EN       (1u << 0)
#define GENET_CMD_RX_EN       (1u << 1)
#define GENET_CMD_SPEED_SHIFT 2
#define GENET_CMD_SPEED_MASK  3u
#define GENET_UMAC_SPEED_10   0
#define GENET_UMAC_SPEED_100  1
#define GENET_UMAC_SPEED_1000 2
#define GENET_CMD_HD_EN       (1u << 10)
#define GENET_CMD_PROMISC     (1u << 4)

/* The receive address filter, which decides what the MAC accepts. UMAC_MAC0/1
 * is NOT it: that pair only signs pause frames. Seventeen slots of two words
 * each, with a control word whose bit (MAX - slot) enables one; at zero and
 * with promiscuous mode off the MAC receives nothing.
 */
#define GENET_UMAC_MDF_CTRL  0x0e50
#define GENET_UMAC_MDF_ADDR  0x0e54
#define GENET_MDF_MAX_FILTER 17

/* TX DMA.
 *
 * The layout is arithmetic, not a flat list, so it is spelled out here once:
 *
 *   descriptors      TX_OFF                          = 0x4000, 12 bytes each
 *   ring registers   TX_OFF + 256 * 12 + 16 * 0x40   = 0x5000
 *   block registers  TX_OFF + 256 * 12 + 17 * 0x40   = 0x5040
 *
 * 256 is the descriptor count the block provides and 16 the default queue;
 * both are fixed by the hardware. Reference: U-Boot drivers/net/bcmgenet.c.
 */
#define GENET_TX_OFF        0x4000
#define GENET_TOTAL_DESCS   256
#define GENET_DMA_DESC_SIZE 12
#define GENET_DEFAULT_Q     16
#define GENET_DMA_RING_SIZE 0x40

#define GENET_TDMA_REG_OFF \
	(GENET_TX_OFF + GENET_TOTAL_DESCS * GENET_DMA_DESC_SIZE)
#define GENET_TDMA_RING \
	(GENET_TDMA_REG_OFF + GENET_DEFAULT_Q * GENET_DMA_RING_SIZE)
#define GENET_TDMA_BLOCK \
	(GENET_TDMA_REG_OFF + (GENET_DEFAULT_Q + 1) * GENET_DMA_RING_SIZE)

/* within a ring's register block */
#define GENET_TDMA_READ_PTR      (GENET_TDMA_RING + 0x00)
#define GENET_TDMA_CONS_INDEX    (GENET_TDMA_RING + 0x08)
#define GENET_TDMA_PROD_INDEX    (GENET_TDMA_RING + 0x0c)
#define GENET_TDMA_RING_BUF_SIZE (GENET_TDMA_RING + 0x10)
#define GENET_TDMA_START_ADDR    (GENET_TDMA_RING + 0x14)
#define GENET_TDMA_END_ADDR      (GENET_TDMA_RING + 0x1c)
#define GENET_TDMA_DONE_THRESH   (GENET_TDMA_RING + 0x24)
#define GENET_TDMA_FLOW_PERIOD   (GENET_TDMA_RING + 0x28)
#define GENET_TDMA_WRITE_PTR     (GENET_TDMA_RING + 0x2c)

/* within the block's register set */
#define GENET_TDMA_RING_CFG       (GENET_TDMA_BLOCK + 0x00)
#define GENET_TDMA_CTRL           (GENET_TDMA_BLOCK + 0x04)
#define GENET_TDMA_SCB_BURST_SIZE (GENET_TDMA_BLOCK + 0x0c)

#define GENET_DMA_EN                (1u << 0)
#define GENET_DMA_RING_BUF_EN_SHIFT 1
#define GENET_DMA_MAX_BURST         0x08
#define GENET_DMA_RING_SIZE_SHIFT   16
#define GENET_RX_BUF_LENGTH         2048

/* descriptor words */
#define GENET_DESC_LENGTH_STATUS 0x00
#define GENET_DESC_ADDRESS_LO    0x04
#define GENET_DESC_ADDRESS_HI    0x08

#define GENET_DMA_BUFLENGTH_SHIFT 16
#define GENET_DMA_TX_APPEND_CRC   0x0040
#define GENET_DMA_TX_QTAG_SHIFT   7
#define GENET_DMA_TX_QTAG_MASK    0x3f
#define GENET_DMA_EOP             0x4000
#define GENET_DMA_SOP             0x2000

#define GENET_MAX_MTU 1536

/* RX DMA.
 *
 * Same arithmetic as the transmit side, one block lower:
 *
 *   descriptors      RX_OFF                          = 0x2000, 12 bytes each
 *   ring registers   RX_OFF + 256 * 12 + 16 * 0x40   = 0x3000
 *   block registers  RX_OFF + 256 * 12 + 17 * 0x40   = 0x3040
 *
 * The ring register offsets are NOT the transmit ones renamed: READ_PTR and
 * WRITE_PTR swap places (0x00/0x28 here, 0x00/0x2c there) and the transmit
 * ring's DONE_THRESH slot holds the flow-control thresholds, so both sets
 * are written out in full. Reference: U-Boot drivers/net/bcmgenet.c.
 */
#define GENET_RX_OFF 0x2000
#define GENET_RDMA_REG_OFF \
	(GENET_RX_OFF + GENET_TOTAL_DESCS * GENET_DMA_DESC_SIZE)
#define GENET_RDMA_RING \
	(GENET_RDMA_REG_OFF + GENET_DEFAULT_Q * GENET_DMA_RING_SIZE)
#define GENET_RDMA_BLOCK \
	(GENET_RDMA_REG_OFF + (GENET_DEFAULT_Q + 1) * GENET_DMA_RING_SIZE)

#define GENET_RDMA_WRITE_PTR       (GENET_RDMA_RING + 0x00)
#define GENET_RDMA_PROD_INDEX      (GENET_RDMA_RING + 0x08)
#define GENET_RDMA_CONS_INDEX      (GENET_RDMA_RING + 0x0c)
#define GENET_RDMA_RING_BUF_SIZE   (GENET_RDMA_RING + 0x10)
#define GENET_RDMA_START_ADDR      (GENET_RDMA_RING + 0x14)
#define GENET_RDMA_END_ADDR        (GENET_RDMA_RING + 0x1c)
#define GENET_RDMA_XON_XOFF_THRESH (GENET_RDMA_RING + 0x24)
#define GENET_RDMA_READ_PTR        (GENET_RDMA_RING + 0x28)

#define GENET_RDMA_RING_CFG       (GENET_RDMA_BLOCK + 0x00)
#define GENET_RDMA_CTRL           (GENET_RDMA_BLOCK + 0x04)
#define GENET_RDMA_SCB_BURST_SIZE (GENET_RDMA_BLOCK + 0x0c)

/* Flow control: assert XOFF with 5 descriptors left, XON at ring/16. */
#define GENET_DMA_FC_THRESH_HI (GENET_TOTAL_DESCS >> 4)
#define GENET_DMA_FC_THRESH_LO 5
#define GENET_DMA_FC_THRESH \
	((GENET_DMA_FC_THRESH_LO << 16) | GENET_DMA_FC_THRESH_HI)

/* The descriptor's status word. SOP/EOP are shared with transmit; the low
 * bits below are receive-only and every one of them means "do not hand this
 * frame to the stack". */
#define GENET_DMA_OWN            0x8000
#define GENET_DMA_BUFLENGTH_MASK 0x0fff
#define GENET_DMA_RX_BRDCAST     0x40
#define GENET_DMA_RX_MULT        0x20
#define GENET_DMA_RX_LG          0x10 /* oversized */
#define GENET_DMA_RX_NO          0x08 /* alignment  */
#define GENET_DMA_RX_RXER        0x04
#define GENET_DMA_RX_CRC_ERROR   0x02
#define GENET_DMA_RX_OV          0x01 /* overflow   */
#define GENET_DMA_RX_ERRORS                                \
	(GENET_DMA_RX_LG | GENET_DMA_RX_NO | GENET_DMA_RX_RXER \
	    | GENET_DMA_RX_CRC_ERROR | GENET_DMA_RX_OV)

/* RBUF_ALIGN_2B is set in the umac reset, so the hardware inserts two bytes
 * before every frame to word-align the IP header; the reported length counts
 * them. */
#define GENET_RX_BUF_OFFSET 2

/* Interrupt controller 0 -- the one the DTB's first interrupt line belongs
 * to, and the one that carries ring 16. INTRL2_1 (the second line) carries
 * rings 0..15, which this driver does not use. */
#define GENET_INTRL2_0               0x0200
#define GENET_INTRL2_CPU_STAT        (GENET_INTRL2_0 + 0x00)
#define GENET_INTRL2_CPU_CLEAR       (GENET_INTRL2_0 + 0x08)
#define GENET_INTRL2_CPU_MASK_STATUS (GENET_INTRL2_0 + 0x0c)
#define GENET_INTRL2_CPU_MASK_SET    (GENET_INTRL2_0 + 0x10)
#define GENET_INTRL2_CPU_MASK_CLEAR  (GENET_INTRL2_0 + 0x14)

#define GENET_IRQ_RXDMA_DONE  (1u << 13) /* mailbox done */
#define GENET_IRQ_RXDMA_PDONE (1u << 14)
#define GENET_IRQ_RXDMA_BDONE (1u << 15)
#define GENET_IRQ_TXDMA_DONE  (1u << 16)
#define GENET_IRQ_TXDMA_PDONE (1u << 17)
#define GENET_IRQ_TXDMA_BDONE (1u << 18)

static inline uint32_t genet_rd(uintptr_t base, uint32_t off) {
	uint32_t v;

	v = REG32_LOAD(base + off);
	dmb(sy);
	return v;
}

static inline void genet_wr(uintptr_t base, uint32_t off, uint32_t val) {
	dmb(sy);
	REG32_STORE(base + off, val);
}

#endif /* BCM2711_GENET_REGS_H_ */
