/**
 * @file
 *
 * @date Nov 12, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_NET_EMAC_EMAC_DESC_H_
#define SRC_DRIVERS_NET_EMAC_EMAC_DESC_H_

/**
 * EMAC Buffer Descriptor
 */
struct emac_desc {
	uint32_t next;
	uint32_t data;
#if __BYTE_ORDER == __BIG_ENDIAN
	uint16_t data_off;
	uint16_t data_len;
	uint16_t flags;
	uint16_t packet_len;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	uint16_t data_len;
	uint16_t data_off;
	uint16_t packet_len;
	uint16_t flags;
#endif
};

/**
 * EMAC Descriptor Flags
 */
#define EMAC_DESC_F_SOP        0x8000U /* Start of Packet (SOP) Flag */
#define EMAC_DESC_F_EOP        0x4000U /* End of Packet (EOP) Flag */
#define EMAC_DESC_F_OWNER      0x2000U /* Ownership (OWNER) Flag */
#define EMAC_DESC_F_EOQ        0x1000U /* End of Queue (EOQ) Flag */
#define EMAC_DESC_F_TDOWNCMPLT 0x0800U /* Teardown Complete (TDOWNCMPLT) Flag */
#define EMAC_DESC_F_PASSCRC    0x0400U /* Pass CRC (PASSCRC) Flag */
#define EMAC_DESC_F_JABBER     0x0200U /* Jabber Flag */
#define EMAC_DESC_F_OVERSIZE   0x0100U /* Oversize Flag */
#define EMAC_DESC_F_FRAGMENT   0x0080U /* Fragment Flag */
#define EMAC_DESC_F_UNDERSIZED 0x0040U /* Undersized Flag */
#define EMAC_DESC_F_CONTROL    0x0020U /* Control Flag */
#define EMAC_DESC_F_OVERRUN    0x0010U /* Overrun Flag */
#define EMAC_DESC_F_CODEERROR  0x0008U /* Code Error (CODEERROR) Flag */
#define EMAC_DESC_F_ALIGNERROR 0x0004U /* Alignment Error (ALIGNERROR) Flag */
#define EMAC_DESC_F_CRCERROR   0x0002U /* CRC Error (CRCERROR) Flag */
#define EMAC_DESC_F_NOMATCH    0x0001U /* No Match (NOMATCH) Flag */

#endif /* SRC_DRIVERS_NET_EMAC_EMAC_DESC_H_ */
