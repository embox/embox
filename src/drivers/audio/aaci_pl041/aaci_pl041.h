/**
 * @file
 *
 * @date 27.09.2016
 * @author Anton Bondarev
 */

#ifndef AACI_PL041_H_
#define AACI_PL041_H_


#define AACI_CTRL_CH1        0x00
#define AACI_CTRL_CH2        0x14
#define AACI_CTRL_CH3        0x28
#define AACI_CTRL_CH4        0x3c

/* offsets in CTRL_CH */
#define AACI_RXCR        0x00
#define AACI_TXCR        0x04
#define AACI_SR          0x08
#define AACI_ISR         0x0C
#define AACI_IE          0x10

/* other register offsets */
#define AACI_SL1RX           0x050
#define AACI_SL1TX           0x054
#define AACI_SL2RX           0x058
#define AACI_SL2TX           0x05c
#define AACI_SL12RX          0x060
#define AACI_SL12TX          0x064
#define AACI_SLFR            0x068
#define AACI_SLISTAT         0x06c
#define AACI_SLIEN           0x070
#define AACI_INTCLR          0x074
#define AACI_MAINCR          0x078
#define AACI_RESET           0x07c
#define AACI_SYNC            0x080
#define AACI_ALLINTS         0x084
#define AACI_MAINFR          0x088
#define AACI_DR1             0x090
#define AACI_DR2             0x0b0
#define AACI_DR3             0x0d0
#define AACI_DR4             0x0f0

/* both for AACI_RXCR and AACI_TXCR */
#define AACI_CR_FEN           (1 << 16)
#define AACI_CR_COMPACT       (1 << 15)
#define AACI_CR_SZ16          (0 << 13)
#define AACI_CR_SZ18          (1 << 13)
#define AACI_CR_SZ20          (2 << 13)
#define AACI_CR_SZ12          (3 << 13)
#define AACI_CR_SL12          (1 << 12)
#define AACI_CR_SL11          (1 << 11)
#define AACI_CR_SL10          (1 << 10)
#define AACI_CR_SL9           (1 << 9)
#define AACI_CR_SL8           (1 << 8)
#define AACI_CR_SL7           (1 << 7)
#define AACI_CR_SL6           (1 << 6)
#define AACI_CR_SL5           (1 << 5)
#define AACI_CR_SL4           (1 << 4)
#define AACI_CR_SL3           (1 << 3)
#define AACI_CR_SL2           (1 << 2)
#define AACI_CR_SL1           (1 << 1)
#define AACI_CR_EN            (1 << 0)

/* status register bits */
#define AACI_SR_RXTOFE        (1 << 11)
#define AACI_SR_TXTO          (1 << 10)
#define AACI_SR_TXU           (1 << 9)
#define AACI_SR_RXO           (1 << 8)
#define AACI_SR_TXB           (1 << 7)
#define AACI_SR_RXB           (1 << 6)
#define AACI_SR_TXFF          (1 << 5)
#define AACI_SR_RXFF          (1 << 4)
#define AACI_SR_TXHE          (1 << 3)
#define AACI_SR_RXHF          (1 << 2)
#define AACI_SR_TXFE          (1 << 1)
#define AACI_SR_RXFE          (1 << 0)

#define AACI_ISR_RXTOFEINTR   (1 << 6)
#define AACI_ISR_URINTR       (1 << 5)
#define AACI_ISR_ORINTR       (1 << 4)
#define AACI_ISR_RXINTR       (1 << 3)
#define AACI_ISR_TXINTR       (1 << 2)
#define AACI_ISR_RXTOINTR     (1 << 1)
#define AACI_ISR_TXCINTR      (1 << 0)

/* interrupt enable */
#define AACI_IE_RXTOIE        (1 << 6)
#define AACI_IE_URIE          (1 << 5)
#define AACI_IE_ORIE          (1 << 4)
#define AACI_IE_RXIE          (1 << 3)
#define AACI_IE_TXIE          (1 << 2)
#define AACI_IE_RXTIE        (1 << 1)
#define AACI_IE_TXCIE         (1 << 0)

/* slot flag register bits */
#define AACI_SLFR_RWIS        (1 << 13)
#define AACI_SLFR_RGPIOINTR   (1 << 12)
#define AACI_SLFR_12TXE       (1 << 11)
#define AACI_SLFR_12RXV       (1 << 10)
#define AACI_SLFR_2TXE        (1 << 9)
#define AACI_SLFR_2RXV        (1 << 8)
#define AACI_SLFR_1TXE        (1 << 7)
#define AACI_SLFR_1RXV        (1 << 6)
#define AACI_SLFR_12TXB       (1 << 5)
#define AACI_SLFR_12RXB       (1 << 4)
#define AACI_SLFR_2TXB        (1 << 3)
#define AACI_SLFR_2RXB        (1 << 2)
#define AACI_SLFR_1TXB        (1 << 1)
#define AACI_SLFR_1RXB        (1 << 0)

/* Interrupt clear register */
#define AACI_ICLR_RXTOFEC4    (1 << 12)
#define AACI_ICLR_RXTOFEC3    (1 << 11)
#define AACI_ICLR_RXTOFEC2    (1 << 10)
#define AACI_ICLR_RXTOFEC1    (1 << 9)
#define AACI_ICLR_TXUEC4      (1 << 8)
#define AACI_ICLR_TXUEC3      (1 << 7)
#define AACI_ICLR_TXUEC2      (1 << 6)
#define AACI_ICLR_TXUEC1      (1 << 5)
#define AACI_ICLR_RXOEC4      (1 << 4)
#define AACI_ICLR_RXOEC3      (1 << 3)
#define AACI_ICLR_RXOEC2      (1 << 2)
#define AACI_ICLR_RXOEC1      (1 << 1)
#define AACI_ICLR_WISC        (1 << 0)

/* Main control register bits AACI_MAINCR */
#define AACI_MAINCR_SCRA(x)   ((x) << 10)
#define AACI_MAINCR_DMAEN     (1 << 9)
#define AACI_MAINCR_SL12TXEN  (1 << 8)
#define AACI_MAINCR_SL12RXEN  (1 << 7)
#define AACI_MAINCR_SL2TXEN   (1 << 6)
#define AACI_MAINCR_SL2RXEN   (1 << 5)
#define AACI_MAINCR_SL1TXEN   (1 << 4)
#define AACI_MAINCR_SL1RXEN   (1 << 3)
#define AACI_MAINCR_LPM       (1 << 2)
#define AACI_MAINCR_LOOPBK    (1 << 1)
#define AACI_MAINCR_IE        (1 << 0)

/*
 * Reset register bits. P65
 */
#define RESET_NRST   (1 << 0)

/*
 * Sync register bits. P65
 */
#define SYNC_FORCE   (1 << 0)

/*
 * Main flag register bits. P66
 */
#define MAINFR_TXB   (1 << 1)	/* transmit busy */
#define MAINFR_RXB   (1 << 0)	/* receive busy */


#endif /* AACI_PL041_H_ */
