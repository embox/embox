
/**
 * @file
 *
 * Copyright (c) 1987, 1997, 2006, Vrije Universiteit, Amsterdam,
 * The Netherlands All rights reserved. Redistribution and use of the MINIX 3
 * operating system in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *     * Neither the name of the Vrije Universiteit nor the names of the
 *     software authors or contributors may be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 *     * Any deviations from these conditions require written permission
 *     from the copyright holder in advance
 *
 * @author Niek Linnenbank <nieklinnenbank@gmail.com>
 * @author Anton Kozlov
 * @date 27.09.2012
 *
 */

#ifndef __E1000_REG_H
#define __E1000_REG_H

/**
 * @name Controller Registers.
 * @{
 */

/** Device Control. */
#define E1000_REG_CTRL		0x00000

/** Device Status. */
#define E1000_REG_STATUS	0x00008

/** EEPROM Read. */
#define E1000_REG_EERD		0x00014

/** MDI Control. */
#define E1000_REG_MDIC		0x00020

/** Flow Control Address Low. */
#define E1000_REG_FCAL		0x00028

/** Flow Control Address High. */
#define E1000_REG_FCAH		0x0002c

/** Flow Control Type. */
#define E1000_REG_FCT		0x00030

/** Interrupt Cause Read. */
#define E1000_REG_ICR		0x000c0

/** Interrupt Mask Set/Read Register. */
#define E1000_REG_IMS		0x000d0

/** Receive Control Register. */
#define E1000_REG_RCTL		0x00100

/** Transmit Control Register. */
#define E1000_REG_TCTL		0x00400

/** Flow Control Transmit Timer Value. */
#define E1000_REG_FCTTV		0x00170

/** Receive Descriptor Base Address Low. */
#define E1000_REG_RDBAL		0x02800

/** Receive Descriptor Base Address High. */
#define E1000_REG_RDBAH		0x02804

/** Receive Descriptor Length. */
#define E1000_REG_RDLEN		0x02808

/** Receive Descriptor Head. */
#define E1000_REG_RDH		0x02810

/** Receive Descriptor Tail. */
#define E1000_REG_RDT		0x02818

/** Transmit Descriptor Base Address Low. */
#define E1000_REG_TDBAL		0x03800

/** Transmit Descriptor Base Address High. */
#define E1000_REG_TDBAH		0x03804

/** Transmit Descriptor Length. */
#define E1000_REG_TDLEN		0x03808

/** Transmit Descriptor Head. */
#define E1000_REG_TDH		0x03810

/** Transmit Descriptor Tail. */
#define E1000_REG_TDT		0x03818

/** CRC Error Count. */
#define E1000_REG_CRCERRS	0x04000

/** RX Error Count. */
#define E1000_REG_RXERRC	0x0400c

/** Missed Packets Count. */
#define E1000_REG_MPC		0x04010

/** Collision Count. */
#define E1000_REG_COLC		0x04028

/** Total Packets Received. */
#define E1000_REG_TPR		0x040D0

/** Total Packets Transmitted. */
#define E1000_REG_TPT		0x040D4

/** Receive Address Low. */
#define E1000_REG_RAL		0x05400

/** Receive Address High. */
#define E1000_REG_RAH		0x05404

/** Multicast Table Array. */
#define E1000_REG_MTA		0x05200

/**
 * @}
 */

/**
 * @name Control Register Bits.
 * @{
 */

/** Auto-Speed Detection Enable. */
#define E1000_REG_CTRL_ASDE	(1 << 5)

/** Link Reset. */
#define E1000_REG_CTRL_LRST	(1 << 3)

/** Set Link Up. */
#define E1000_REG_CTRL_SLU	(1 << 6)

/** Invert Los Of Signal. */
#define E1000_REG_CTRL_ILOS	(1 << 7)

/** Device Reset. */
#define E1000_REG_CTRL_RST	(1 << 26)

/** VLAN Mode Enable. */
#define E1000_REG_CTRL_VME	(1 << 30)

/** PHY Reset. */
#define E1000_REG_CTRL_PHY_RST	(1 << 31)

/**
 * @}
 */

/**
 * @name Status Register Bits.
 * @{
 */

/** Link Full Duplex Configuration Indication. */
#define E1000_REG_STATUS_FD	 (1 << 0)

/** Link Up Indication. */
#define E1000_REG_STATUS_LU	 (1 << 1)

/** Transmission Paused. */
#define E1000_REG_STATUS_TXOFF	 (1 << 4)

/** Link Speed Setting. */
#define E1000_REG_STATUS_SPEED	((1 << 6) | (1 << 7))

/**
 * @}
 */

/**
 * @name EEPROM Read Register Bits.
 * @{
 */

/** Start Read. */
#define E1000_REG_EERD_START	(1 << 0)

/** Read Done. */
#define E1000_REG_EERD_DONE	(1 << 4)

/** Read Address Bit Mask. */
#define E1000_REG_EERD_ADDR	(0xff   << 8)

/** Read Data Bit Mask. */
#define E1000_REG_EERD_DATA	(0xffff << 16)

/**
 * @}
 */

#define E1000_REG_MDIC_DATA 	0xffff

#define E1000_REG_MDIC_REGADD_OFF 16
#define E1000_REG_MDIC_PHYADD_OFF 21
#define E1000_REG_MDIC_OP_WRITE (0x01 << 26)
#define E1000_REG_MDIC_OP_READ  (0x10 << 26)
#define E1000_REG_MDIC_READY	(0x1  << 28)
#define E1000_REG_MDIC_ERROR	(0x1  << 30)

#define E1000_MDI_PSSTAT	17

#define E1000_PSSTAT_LINK	(0x1 << 10)

/**
 * @name Interrupt Cause Read.
 * @{
 */

/** Transmit Descripts Written Back. */
#define E1000_REG_ICR_TXDW	(1 << 0)

/** Transmit Queue Empty. */
#define E1000_REG_ICR_TXQE	(1 << 1)

/** Link Status Change. */
#define E1000_REG_ICR_LSC	(1 << 2)

/** Receiver Overrun. */
#define E1000_REG_ICR_RXO	(1 << 6)

/** Receiver Timer Interrupt. */
#define E1000_REG_ICR_RXT	(1 << 7)

/**
 * @}
 */

/**
 * @name Interrupt Mask Set/Read Register Bits.
 * @{
 */

/** Transmit Descripts Written Back. */
#define E1000_REG_IMS_TXDW	(1 << 0)

/** Transmit Queue Empty. */
#define E1000_REG_IMS_TXQE	(1 << 1)

/** Link Status Change. */
#define E1000_REG_IMS_LSC	(1 << 2)

/** Receiver FIFO Overrun. */
#define E1000_REG_IMS_RXO	(1 << 6)

/** Receiver Timer Interrupt. */
#define E1000_REG_IMS_RXT	(1 << 7)

/**
 * @}
 */

/**
 * @name Receive Control Register Bits.
 * @{
 */

/** Receive Enable. */
#define E1000_REG_RCTL_EN	(1 << 1)

/** Multicast Promiscious Enable. */
#define E1000_REG_RCTL_MPE	(1 << 4)

/** Broadcast Accept Mode. */
#define E1000_REG_RCTL_BAM	(1 << 15)

/** Receive Buffer Size. */
#define E1000_REG_RCTL_BSIZE	((1 << 16) | (1 << 17))

/**
 * @}
 */

/**
 * @name Transmit Control Register Bits.
 * @{
 */

/** Transmit Enable. */
#define E1000_REG_TCTL_EN	(1 << 1)

/** Pad Short Packets. */
#define E1000_REG_TCTL_PSP	(1 << 3)

/**
 * @}
 */

/**
 * @name Receive Address High Register Bits.
 * @{
 */

/** Receive Address Valid. */
#define E1000_REG_RAH_AV	(1 << 31)

/**
 * @}
 */

/**
 * @name ICH Flash Registers.
 * @see http://gitweb.dragonflybsd.org
 * @{
 */

#define ICH_FLASH_GFPREG                 0x0000
#define ICH_FLASH_HSFSTS                 0x0004
#define ICH_FLASH_HSFCTL                 0x0006
#define ICH_FLASH_FADDR                  0x0008
#define ICH_FLASH_FDATA0                 0x0010
#define FLASH_GFPREG_BASE_MASK           0x1FFF
#define FLASH_SECTOR_ADDR_SHIFT          12
#define ICH_FLASH_READ_COMMAND_TIMEOUT   500
#define ICH_FLASH_LINEAR_ADDR_MASK       0x00FFFFFF
#define ICH_CYCLE_READ                   0
#define ICH_FLASH_CYCLE_REPEAT_COUNT     10

/** End of Packet. */
#define E1000_TX_CMD_EOP	(1 << 0)

/** Insert FCS/CRC. */
#define E1000_TX_CMD_FCS	(1 << 1)

/** Report Status. */
#define E1000_TX_CMD_RS		(1 << 3)

#endif /* __E1000_REG_H */
