/**
 * @file
 *
 * @date Jan 9, 2020
 *  @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_NET_MCST_L_E1000_H_
#define SRC_DRIVERS_NET_MCST_L_E1000_H_

#include <stdint.h>

#define L_E1000_TOTAL_PCI_IO_SIZE    0x20

/* Register Map */
#define L_E1000_E_CSR         0x00 /* Ethernet Control/Status Register */
#define L_E1000_MGIO_CSR      0x04 /* MGIO Control/Status Register */
#define L_E1000_MGIO_DATA     0x08 /* MGIO Data Register */
#define L_E1000_E_BASE_ADDR   0x0c /* EthernetBase Address Register */
#define L_E1000_DMA_BASE_ADDR 0x10 /* DMA Base Address Register */
#define L_E1000_PSF_CSR       0x14 /* Pause Frame Control/Status Register */
#define L_E1000_PSF_DATA      0x18 /* Pause Frame Data Register */
#define L_E1000_INT_DELAY     0x1c /* Interrupt Delay Register */

/* E_CSR register bits */
/* 31:21 unused, readed as 0 */
#define E_CSR_ATME        (1 << 24) /* RW, Add Timer Enable */
#define E_CSR_TMCE        (1 << 23) /* RW, Timer Clear Enable */
#define E_CSR_DRIN        (1 << 22) /* RW, Disable RX Interrupt */
#define E_CSR_DTIN        (1 << 21) /* RW, Disable TX Interrupt */
#define E_CSR_ESLE        (1 << 20) /* RW, Enable Slave Error */
#define E_CSR_SLVE        (1 << 19) /* RW1c, Slave Error */
#define E_CSR_PSFI        (1 << 18) /* RW1c, Pause Frame Interrupt */
/* 17 unused, read as 0  */
#define E_CSR_SINT       (1 << 16) /* R, Status Interrupt */
#define E_CSR_ERR        (1 << 15) /* R, Error */
#define E_CSR_BABL       (1 << 14) /* RW1c, Babble */
#define E_CSR_CERR       (1 << 13) /* RW1c, Collision Error */
#define E_CSR_MISS       (1 << 12) /* RW1c, Missed Packet */
#define E_CSR_MERR       (1 << 11) /* RW1c, Memory Error */
#define E_CSR_RINT       (1 << 10) /* RW1c, Receiver Interrupt */
#define E_CSR_TINT       (1 << 9)  /* RW1c, Transmiter Interrupt */
#define E_CSR_IDON       (1 << 8)  /* RW1c, Initialization Done */
#define E_CSR_INTR       (1 << 7)  /* R, Interrupt Flag */
#define E_CSR_INEA       (1 << 6)  /* RW, Interrupt Enable */
#define E_CSR_RXON       (1 << 5)  /* R, Receiver On */
#define E_CSR_TXON       (1 << 4)  /* R, Transmiter On */
#define E_CSR_TDMD       (1 << 3)  /* RW1, Transmit Demand */
#define E_CSR_STOP       (1 << 2)  /* RW1, Stop */
#define E_CSR_STRT       (1 << 1)  /* RW1, Start */
#define E_CSR_INIT       (1 << 0)  /* RW1, Initialize */

/* MGIO_CSR regiter bits */
#define MGIO_CSR_FDPL            (1 << 23)  /* rw*** FULL DUPLEX POLARITY */
#define MGIO_CSR_FEPL            (1 << 22)  /* rw*** FAST ETHERNET POLARITY */
#define MGIO_CSR_GEPL            (1 << 21)  /* rw*** GIGABIT ETHERNET POLARITY */
#define MGIO_CSR_SLSP            (1 << 20)  /* rw*** LINK STATUS POLARITY */
#define MGIO_CSR_SLST            (1 << 19)  /* rw*** SOFT LINK STATUS */
#define MGIO_CSR_SLSE            (1 << 18)  /* rw*** SOFT LINK STATUS ENABLE */
#define MGIO_CSR_FDUP            (1 << 17)  /* RW/R* FULL DUPLEX */
#define MGIO_CSR_FETH            (1 << 16)  /* RW/R* FAST ETHERNET */
#define MGIO_CSR_GETH            (1 << 15)  /* RW/R* GIGABIT ETHERNET */
#define MGIO_CSR_HARD            (1 << 14)  /* RW HARD/SOFT Selector */
#define MGIO_CSR_RRDY            (1 << 13)  /* RC RESULT READY */
#define MGIO_CSR_CST1            (1 << 12)  /* RC CHANGED LINK STATUS 1 */
#define MGIO_CSR_LST1            (1 << 11)  /* R LINK STATUS 1 */
#define MGIO_CSR_CST0            (1 << 10)  /* RC CHANGED LINK STATUS 0 */
#define MGIO_CSR_LST0            (1 << 9)   /* R LINK STATUS 0 */
#define MGIO_CSR_OUT1            (1 << 8)   /* RW OUTPUT PORT 1 */
#define MGIO_CSR_OUT0            (1 << 7)   /* RW OUTPUT PORT 0 */
#define MGIO_CSR_RSTP(polarity)  ((polarity) << 6)   /* RW RESET POLARITY */
# define L_E1000_RSET_POLARITY   MGIO_CSR_RSTP(0) /* polarity is 0 for E1000 */
#define MGIO_CSR_ERDY            (1 << 5)   /* RW ENABLE INT ON RRDY  */
#define MGIO_CSR_EST1            (1 << 4)   /* RW ENABLE INT ON CST1 */
#define MGIO_CSR_EST0            (1 << 3)   /* RW ENABLE INT ON CST0 */
#define MGIO_CSR_SRST            (1 << 2)   /* RW PHY SOFTWARE RESET */
#define MGIO_CSR_LSTS            (1 << 1)   /* RW LINK STATUS SELECT */
#define MGIO_CSR_MGIO_SINT       (1 << 0)   /* STATUS INTERRUPT */

/* PSF_CSR Register */
#define PSF_CSR_PSEX   (1 << 4) /* RC, Pause Expired */
#define PSF_CSR_EPSX   (1 << 3) /* RW, PSEX Enable   */
#define PSF_CSR_PSFR   (1 << 2) /* RC, Pause Frame Recieved */
#define PSF_CSR_EPSF   (1 << 1) /* RW, PSFR Enable */
#define PSF_CSR_PSFI   (1 << 0) /* R,  Pause Frame Interrupt */

/* L_E1000_PSF_DATA Register, R */
#define PSF_DATA_RCNT_SHIFT     0
#define PSF_DATA_RCNT_MASK      0xFFFF
#define PSF_DATA_RCNT_CUR_SHIFT 16
#define PSF_DATA_RCNT_CUR_MASK  0xFFFF

/* L_E1000_INT_DELAY Register */
#define INT_DELAY_RX_SHIFT  0
#define INT_DELAY_RX_MASK   0xFFFF
#define INT_DELAY_TX_SHIFT  16
#define INT_DELAY_TX_MASK   0xFFFF

/* Bits for L_E1000_MGIO_DATA registers */
#define MGIO_DATA_OFF       0
#define MGIO_CS_OFF         16
#define MGIO_REG_AD_OFF     18
#define MGIO_PHY_AD_OFF     23
#define MGIO_OP_CODE_OFF    28
#define MGIO_ST_OF_F_OFF    30

struct l_e1000_init_block {
	uint16_t mode;
	uint8_t  paddr[6];
	uint64_t laddrf;
	/* 31:4 = addr of rx desc ring (16 bytes align) +
	 * 3:0  = number of descriptors (the power of two)
	 * 0x09 is max value (desc number = 512 if [3:0] >= 0x09)
	 */
	uint32_t rdra;
	/* 31:4 = addr of tx desc ring (16 bytes align) +
	 * 3:0  = number of descriptors (the power of two)
	 * 0x09 is max value (desc number = 512 if [3:0] >= 0x09)
	 */
	uint32_t tdra;
} __attribute__((packed));

/* Init Block bits */
#define DRX     (1 << 0)  /* Receiver disable */
#define DTX     (1 << 1)  /* Transmitter disable */
#define LOOP    (1 << 2)  /* loopback */
#define DTCR    (1 << 3)  /* disable transmit crc */
#define COLL    (1 << 4)  /* force collision */
#define DRTY    (1 << 5)  /* disable retry */
#define INTL    (1 << 6)  /* Internal loopback */
#define EMBA    (1 << 7)  /* enable modified back-off algorithm */
#define EJMF    (1 << 8)  /* enable jambo frame */
#define EPSF    (1 << 9)  /* enable pause frame */
#define FULL    (1 << 10)  /* full packet mode */
#define PROM    (1 << 15) /* promiscuous mode */

struct l_e1000_rx_desc {
	uint32_t    base;
	int16_t     buf_length;
	int16_t     status;
	int16_t     msg_length;
	uint16_t    reserved1;
	uint32_t    etmr;
} __attribute__((packed));

struct l_e1000_tx_desc {
	uint32_t    base;
	int16_t     buf_length;
	int16_t     status;
	uint32_t    misc;
	uint32_t    etmr;
} __attribute__((packed));

/* RX Descriptor status bits */
#define RD_OWN     (1 << 15)
#define RD_ERR     (1 << 14)
#define RD_FRAM    (1 << 13)
#define RD_OFLO    (1 << 12)
#define RD_CRC     (1 << 11)
#define RD_BUFF    (1 << 10)
#define RD_STP     (1 << 9)
#define RD_ENP     (1 << 8)
#define RD_PAM     (1 << 6)
#define RD_LAFM    (1 << 4)
#define RD_BAM     (1 << 3)

/* TX Descriptor status bits */
#define TD_OWN      (1 << 15)
#define TD_ERR      (1 << 14)
#define TD_AFCS     (1 << 13)
#define TD_NOINTR   (1 << 13)
#define TD_MORE     (1 << 12)
#define TD_ONE      (1 << 11)
#define TD_DEF      (1 << 10)
#define TD_STP      (1 << 9)
#define TD_ENP      (1 << 8)

/* TX Descriptor misc bits */
#define TD_RTRY         (1 << 26)
#define TD_LCAR         (1 << 27)
#define TD_LCOL         (1 << 28)
#define TD_UFLO         (1 << 30)
#define TD_BUFF         (1 << 31)

#endif /* SRC_DRIVERS_NET_MCST_L_E1000_H_ */
