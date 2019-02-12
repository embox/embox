/**
 * @file
 *
 * @date Sep 4, 2018
 * @author Anton Bondarev
 */
#ifndef SRC_DRIVERS_NET_DWC_GMAC_DWC_DESCS_H_
#define SRC_DRIVERS_NET_DWC_GMAC_DWC_DESCS_H_

#include <stdint.h>

/* Normal receive descriptor defines */

#define GENMASK(h, l) \
	(((~0UL) - (1UL << (l)) + 1) & (~0UL >> (BITS_PER_LONG - 1 - (h))))

/* RDES0 */
#define RDES0_PAYLOAD_CSUM_ERR           (uint32_t)(1<<0)
#define RDES0_CRC_ERROR                  (uint32_t)(1<<1)
#define RDES0_DRIBBLING                  (uint32_t)(1<<2)
#define RDES0_MII_ERROR                  (uint32_t)(1<<3)
#define RDES0_RECEIVE_WATCHDOG           (uint32_t)(1<<4)
#define RDES0_FRAME_TYPE                 (uint32_t)(1<<5)
#define RDES0_COLLISION                  (uint32_t)(1<<6)
#define RDES0_IPC_CSUM_ERROR             (uint32_t)(1<<7)
#define RDES0_LAST_DESCRIPTOR            (uint32_t)(1<<8)
#define RDES0_FIRST_DESCRIPTOR           (uint32_t)(1<<9)
#define RDES0_VLAN_TAG                   (uint32_t)(1<<10)
#define RDES0_OVERFLOW_ERROR             (uint32_t)(1<<11)
#define RDES0_LENGTH_ERROR               (uint32_t)(1<<12)
#define RDES0_SA_FILTER_FAIL             (uint32_t)(1<<13)
#define RDES0_DESCRIPTOR_ERROR           (uint32_t)(1<<14)
#define RDES0_ERROR_SUMMARY              (uint32_t)(1<<15)
//#define RDES0_FRAME_LEN_MASK             GENMASK(29, 16)
#define RDES0_FRAME_LEN_SHIFT            16
#define RDES0_DA_FILTER_FAIL             (uint32_t)(1<<30)
#define RDES0_OWN                        (uint32_t)(1<<31)

/* RDES1 */
#define RDES1_BUFFER1_SIZE_MASK          0x1FFF/*GENMASK(10, 0) */
//#define RDES1_BUFFER2_SIZE_MASK          GENMASK(21, 11)
#define RDES1_BUFFER2_SIZE_SHIFT         11
#define RDES1_SECOND_ADDRESS_CHAINED     (uint32_t)(1<<24)
#define RDES1_END_RING                   (uint32_t)(1<<25)
#define RDES1_DISABLE_IC                 (uint32_t)(1<<31)

/* Enhanced receive descriptor defines */

/* RDES0 (similar to normal RDES) */
#define ERDES0_RX_MAC_ADDR               (uint32_t)(1<<0)

/* RDES1: completely differ from normal desc definitions */
#define ERDES1_BUFFER1_SIZE_MASK         GENMASK(12, 0)
#define ERDES1_SECOND_ADDRESS_CHAINED    (uint32_t)(1<<14)
#define ERDES1_END_RING                  (uint32_t)(1<<15)
#define ERDES1_BUFFER2_SIZE_MASK         GENMASK(28, 16)
#define ERDES1_BUFFER2_SIZE_SHIFT        16
#define ERDES1_DISABLE_IC                (uint32_t)(1<<31)

/* Normal transmit descriptor defines */
/* TDES0 */
#define TDES0_DEFERRED                   (uint32_t)(1<<0)
#define TDES0_UNDERFLOW_ERROR            (uint32_t)(1<<1)
#define TDES0_EXCESSIVE_DEFERRAL         (uint32_t)(1<<2)
#define TDES0_COLLISION_COUNT_MASK       0x780 /* GENMASK(6, 3) */
#define TDES0_VLAN_FRAME                 (uint32_t)(1<<7)
#define TDES0_EXCESSIVE_COLLISIONS       (uint32_t)(1<<8)
#define TDES0_LATE_COLLISION             (uint32_t)(1<<9)
#define TDES0_NO_CARRIER                 (uint32_t)(1<<10)
#define TDES0_LOSS_CARRIER               (uint32_t)(1<<11)
#define TDES0_PAYLOAD_ERROR              (uint32_t)(1<<12)
#define TDES0_FRAME_FLUSHED              (uint32_t)(1<<13)
#define TDES0_JABBER_TIMEOUT             (uint32_t)(1<<14)
#define TDES0_ERROR_SUMMARY              (uint32_t)(1<<15)
#define TDES0_IP_HEADER_ERROR            (uint32_t)(1<<16)
#define TDES0_TIME_STAMP_STATUS          (uint32_t)(1<<17)
#define TDES0_OWN                        (uint32_t)(1<<31)
/* TDES1 */
#define TDES1_BUFFER1_SIZE_MASK          0x7FF     /* GENMASK(10, 0) */
#define TDES1_BUFFER2_SIZE_MASK          0x3FF800   /* GENMASK(21, 11) */
#define TDES1_BUFFER2_SIZE_SHIFT         11
#define TDES1_TIME_STAMP_ENABLE          (uint32_t)(1<<22)
#define TDES1_DISABLE_PADDING            (uint32_t)(1<<23)
#define TDES1_SECOND_ADDRESS_CHAINED     (uint32_t)(1<<24)
#define TDES1_END_RING                   (uint32_t)(1<<25)
#define TDES1_CRC_DISABLE                (uint32_t)(1<<26)
#define TDES1_CHECKSUM_INSERTION_MASK    (uint32_t)(3<<27) /*GENMASK(28, 27)*/
#define TDES1_CHECKSUM_INSERTION_SHIFT   27
#define TDES1_FIRST_SEGMENT              (uint32_t)(1<<29)
#define TDES1_LAST_SEGMENT               (uint32_t)(1<<30)
#define TDES1_INTERRUPT                  (uint32_t)(1<<31)

/* Enhanced transmit descriptor defines */
/* TDES0 */
#define ETDES0_DEFERRED                  (uint32_t)(1<<0)
#define ETDES0_UNDERFLOW_ERROR           (uint32_t)(1<<1)
#define ETDES0_EXCESSIVE_DEFERRAL        (uint32_t)(1<<2)
#define ETDES0_COLLISION_COUNT_MASK      0x780 /* GENMASK(6, 3) */
#define ETDES0_VLAN_FRAME                (uint32_t)(1<<7)
#define ETDES0_EXCESSIVE_COLLISIONS      (uint32_t)(1<<8)
#define ETDES0_LATE_COLLISION            (uint32_t)(1<<9)
#define ETDES0_NO_CARRIER                (uint32_t)(1<<10)
#define ETDES0_LOSS_CARRIER              (uint32_t)(1<<11)
#define ETDES0_PAYLOAD_ERROR             (uint32_t)(1<<12)
#define ETDES0_FRAME_FLUSHED             (uint32_t)(1<<13)
#define ETDES0_JABBER_TIMEOUT            (uint32_t)(1<<14)
#define ETDES0_ERROR_SUMMARY             (uint32_t)(1<<15)
#define ETDES0_IP_HEADER_ERROR           (uint32_t)(1<<16)
#define ETDES0_TIME_STAMP_STATUS         (uint32_t)(1<<17)
#define ETDES0_SECOND_ADDRESS_CHAINED    (uint32_t)(1<<20)
#define ETDES0_END_RING                  (uint32_t)(1<<21)
#define ETDES0_CHECKSUM_INSERTION_MASK   (uint32_t)(1<<22) /*GENMASK(23, 22)*/
#define ETDES0_CHECKSUM_INSERTION_SHIFT  22
#define ETDES0_TIME_STAMP_ENABLE         (uint32_t)(1<<25)
#define ETDES0_DISABLE_PADDING           (uint32_t)(1<<26)
#define ETDES0_CRC_DISABLE               (uint32_t)(1<<27)
#define ETDES0_FIRST_SEGMENT             (uint32_t)(1<<28)
#define ETDES0_LAST_SEGMENT              (uint32_t)(1<<29)
#define ETDES0_INTERRUPT                 (uint32_t)(1<<30)
#define ETDES0_OWN                       (uint32_t)(1<<31)
/* TDES1 */
#define ETDES1_BUFFER1_SIZE_MASK         0x1FFF /* GENMASK(12, 0) */
#define ETDES1_BUFFER2_SIZE_MASK         0x1FFF0000 /* GENMASK(28, 16) */
#define ETDES1_BUFFER2_SIZE_SHIFT        16

/* Extended Receive descriptor definitions */
#define ERDES4_IP_PAYLOAD_TYPE_MASK     GENMASK(2, 6)
#define ERDES4_IP_HDR_ERR               (uint32_t)(1<<3)
#define ERDES4_IP_PAYLOAD_ERR           (uint32_t)(1<<4)
#define ERDES4_IP_CSUM_BYPASSED         (uint32_t)(1<<5)
#define ERDES4_IPV4_PKT_RCVD            (uint32_t)(1<<6)
#define ERDES4_IPV6_PKT_RCVD            (uint32_t)(1<<7)
#define ERDES4_MSG_TYPE_MASK            GENMASK(11, 8)
#define ERDES4_PTP_FRAME_TYPE           (uint32_t)(1<<12)
#define ERDES4_PTP_VER                  (uint32_t)(1<<13)
#define ERDES4_TIMESTAMP_DROPPED        (uint32_t)(1<<14)
#define ERDES4_AV_PKT_RCVD              (uint32_t)(1<<16)
#define ERDES4_AV_TAGGED_PKT_RCVD       (uint32_t)(1<<17)
#define ERDES4_VLAN_TAG_PRI_VAL_MASK    GENMASK(20, 18)
#define ERDES4_L3_FILTER_MATCH          (uint32_t)(1<<24)
#define ERDES4_L4_FILTER_MATCH          (uint32_t)(1<<25)
#define ERDES4_L3_L4_FILT_NO_MATCH_MASK GENMASK(27, 26)

/* Extended RDES4 message type definitions */
#define RDES_EXT_NO_PTP                 0x0
#define RDES_EXT_SYNC                   0x1
#define RDES_EXT_FOLLOW_UP              0x2
#define RDES_EXT_DELAY_REQ              0x3
#define RDES_EXT_DELAY_RESP             0x4
#define RDES_EXT_PDELAY_REQ             0x5
#define RDES_EXT_PDELAY_RESP            0x6
#define RDES_EXT_PDELAY_FOLLOW_UP       0x7
#define RDES_PTP_ANNOUNCE               0x8
#define RDES_PTP_MANAGEMENT             0x9
#define RDES_PTP_SIGNALING              0xa
#define RDES_PTP_PKT_RESERVED_TYPE      0xf

/* Basic descriptor structure for normal and alternate descriptors */
struct dma_desc {
	uint32_t des0;
	uint32_t des1;
	uint32_t des2;
	uint32_t des3;
} __attribute__((aligned(8)));

/* Extended descriptor structure (e.g. >= databook 3.50a) */
struct dma_extended_desc {
	struct dma_desc basic;  /* Basic descriptors */
	uint32_t        des4;   /* Extended Status */
	uint32_t        des5;   /* Reserved */
	uint32_t        des6;   /* Tx/Rx Timestamp Low */
	uint32_t        des7;   /* Tx/Rx Timestamp High */
} __attribute__((aligned(8)));

#endif /* SRC_DRIVERS_NET_DWC_GMAC_DWC_DESC_H_ */
