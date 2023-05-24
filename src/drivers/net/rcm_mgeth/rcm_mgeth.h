/**
 * @file
 *
 * @date Mar 29, 2023
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_NET_RCM_MGETH_RCM_MGETH_H_
#define SRC_DRIVERS_NET_RCM_MGETH_RCM_MGETH_H_

#include <stdint.h>

#define RCM_MGETH_MAX_DMA_CHANNELS  4

// Common registers
#define MGETH_ID              0x0000
#define MGETH_VERSION         0x0004
#define MGETH_SW_RST          0x0008
#define MGETH_GLOBAL_STATUS   0x000C

// MGETH registers
#define MGETH_STATUS          0x0010
#define MGETH_IRQ_MASK        0x0018
#define MGETH_CONTROL         0x001C
#define MGETH_LEN_MASK_CH0    0x0020
#define MGETH_LEN_MASK_CHd       0x4
#define MGETH_TX0_DELAY_TIMER 0x0030
#define MGETH_TXd_DELAY_TIMER    0x4
#define HD_SGMII_MODE         0x0040

// Receive channel registers (relative begin of channel)
#define RXx_ETH_MASK_VALUE_0 -0x0100
#define RXx_ETH_MASK_VALUE_d     0x4
#define RXx_ETH_MASK_VALUE_c      32
#define RXx_ETH_MASK_ACTIV_0 -0x0080
#define RXx_ETH_MASK_ACTIV_d     0x4
#define RXx_ETH_MASK_ACTIV_c      32
#if 0
// Receive channels
#define MGETH_RECV_CH_0       0x0300
#define MGETH_RECV_CH_d        0x200

// Transmit channels
#define MGETH_SEND_CH_0       0x0A00
#define MGETH_SEND_CH_d        0x100

// Setup registers of DMA channel (relative begin of channel)
#define ENABLE_x                0x00
#define SETTINGS_x              0x10
#define IRQ_MASK_x              0x14
#define STATUS_x                0x18
#define DESC_ADDR_x             0x20
#endif

#define MDMA_IRQ_INT_DESC      (1 << 2)
#define MDMA_IRQ_STOP_DESC     (1 << 4)

typedef volatile uint32_t rwreg32;


struct mdma_regs {
	/*********************** WDMA Channel registers *****************************/
	rwreg32 enable; /* 0x000 - enable channel       */
	rwreg32 suspend; /* 0x004 - suspend channel      */
	rwreg32 cancel; /* 0x008 - cancel channel       */
	const volatile uint32_t _skip01; /* 0x00C                        */
	rwreg32 settings; /* 0x010 - channel settings     */
	rwreg32 irq_mask; /* 0x014 - channel irq mask     */
	const volatile uint32_t status; /* 0x018 - channel status       */
	const volatile uint32_t _skip02; /* 0x01C                        */
	rwreg32 desc_addr; /* 0x020 - first decriptor addr */
	const volatile uint32_t _skip03; /* 0x024                        */
	const volatile uint32_t curr_desc_addr; /* 0x028 - current decript addr */
	const volatile uint32_t curr_addr; /* 0x02C - current trans addr   */
	const volatile uint32_t dma_state; /* 0x030 - state of DMA         */
	const volatile uint32_t _skip04[3]; /* 0x034 - 0x13C                */
	volatile uint32_t desc_axlen; /* 0x040 - axlen for desc ops   */
	volatile uint32_t desc_axcache; /* 0x044 - axcache for desc ops */
	volatile uint32_t desc_axprot; /* 0x048 - axprot for desc ops  */
	volatile uint32_t desc_axlock; /* 0x04C - axlock for desc ops  */
	const volatile uint32_t desc_rresp; /* 0x050 - rresp of desc ops    */
	const volatile uint32_t desc_raxi_err_addr; /* 0x054 - addr of axi read err */
	const volatile uint32_t desc_bresp; /* 0x058 - bresp of desc ops    */
	const volatile uint32_t desc_waxi_err_addr; /* 0x05C - addr of axi write err*/
	volatile uint32_t  desc_permut; /* 0x060 - byte swapping scheme */
	const volatile uint32_t _skip05[7]; /* 0x064 - 0x17C                */
	volatile uint32_t  max_trans; /* 0x080 - max unfinished trans */
	volatile uint32_t  arlen; /* 0x084 - axi arlen            */
	volatile uint32_t  arcache; /* 0x088 - axi arcache          */
	volatile uint32_t  arprot; /* 0x08C - axi arprot           */
	volatile uint32_t  arlock; /* 0x090 - axi arlock           */
	const volatile uint32_t rresp; /* 0x094 - axi operation rresp  */
	const volatile uint32_t waxi_err_addr; /* 0x098 - addr of axi write err*/
	const volatile uint32_t _skip06; /* 0x09C                        */
	const volatile uint32_t state; /* 0x0A0 - axi state            */
	const volatile uint32_t avaliable_space; /* 0x0A4 - num of free bytes    */
	volatile uint32_t  permutation; /* 0x0A8 - byte swapping scheme */
	const volatile uint32_t _skip07[5]; /* 0x0AC - 0x1BC                */
} __attribute__ ((packed));

struct mgeth_rx_regs {
	/************************* RX Channel registers *****************************/
	volatile uint32_t rx_eth_mask_value[32]; /* 0x000-0x07C - packets mask   */
	volatile uint32_t rx_eth_mask_activ[32]; /* 0x080-0x0FC - mask activation*/

	/*********************** WDMA Channel registers *****************************/

	struct mdma_regs dma_regs;
	/************************** Statistc counters *******************************/
	const volatile uint32_t a_frames_received_ok;    /* 0x1C0                        */
	const volatile uint64_t a_octets_received_ok;    /* 0x1C4                        */
	const volatile uint32_t if_in_ucast_pkts;        /* 0x1CC                        */
	const volatile uint32_t if_in_multicast_pkts;    /* 0x1D0                        */
	const volatile uint32_t if_in_broadcast_pkts;    /* 0x1D4                        */
	const volatile uint32_t descriptor_short;        /* 0x1D8                        */
	const volatile uint32_t rtp_overmuch_line;       /* 0x1DC                        */
	const volatile uint32_t _skip08[8];              /* 0x1E0 - 0x1FC                */
} __attribute__ ((packed));

// generic flags
#define MGETH_ENABLE 0x1

// channel settings
#define MGETH_CHAN_DESC_NORMAL     0x00000000
#define MGETH_CHAN_DESC_LONG       0x00000002
#define MGETH_CHAN_DESC_PITCH      0x00000003
#define MGETH_CHAN_ADD_INFO        0x00000010
#define MGETH_CHAN_DESC_GAP_SHIFT  16

/* mg_control */
#define MGETH_SPEED_SPEED_10    0x0
#define MGETH_SPEED_SPEED_100   0x1
#define MGETH_SPEED_1000        0x2
#define RCM_MGETH_CTRL_FD_M         0x00000001
#define RCM_MGETH_CTRL_SPEED_OFF  1
//#define CTRL_SPEED_M 0x00000006

struct mgeth_tx_regs {
	/*********************** WDMA Channel registers *****************************/
	struct mdma_regs dma_regs;

	/************************** Statistc counters *******************************/
	const volatile uint32_t a_frames_transmitted_ok;   /* 0x0C0                        */
	const volatile uint64_t a_octets_transmitted_ok;   /* 0x0C4                        */
	const volatile uint32_t if_out_ucast_pkts;         /* 0x0CC                        */
	const volatile uint32_t if_out_multicast_pkts;     /* 0x0D0                        */
	const volatile uint32_t if_out_broadcast_pkts;     /* 0x0D4                        */
	const volatile uint32_t _skip08[10];               /* 0x0D8 - 0x0FC                */
} __attribute__ ((packed));

struct mgeth_regs {
	/***************** Common registers for MGETH and MDMA **********************/
	const volatile uint32_t id;             /* 0x000 - device id            */
	const volatile uint32_t version;        /* 0x004 - device version       */
	volatile uint32_t sw_rst;               /* 0x008 - program reset        */
	const volatile uint32_t global_status;  /* 0x00C - status               */
	/**************************** MGETH registers *******************************/
	const volatile uint32_t mg_status;      /* 0x010 - mgeth status         */
	const volatile uint32_t _skip01;        /* 0x014                        */
	volatile uint32_t mg_irq_mask;          /* 0x018 - mgeth irq mask       */
	volatile uint32_t mg_control;           /* 0x01C - mgeth control reg    */
	volatile uint32_t mg_len_mask_ch0;      /* 0x020 - mgeth mask len ch0   */
	volatile uint32_t mg_len_mask_ch1;      /* 0x024 - mgeth mask len ch1   */
	volatile uint32_t mg_len_mask_ch2;      /* 0x028 - mgeth mask len ch2   */
	volatile uint32_t mg_len_mask_ch3;      /* 0x02C - mgeth mask len ch3   */
	volatile uint32_t tx0_delay_timer;      /* 0x030 - delay timer for tx0  */
	volatile uint32_t tx1_delay_timer;      /* 0x034 - delay timer for tx1  */
	volatile uint32_t tx2_delay_timer;      /* 0x038 - delay timer for tx2  */
	volatile uint32_t tx3_delay_timer;      /* 0x03C - delay timer for tx3  */
	volatile uint32_t hd_sgmii_mode;        /* 0x040 - SGMII mode           */
	const volatile uint32_t _skip02[47];    /* 0x044 - 0x0FC                */

	/************************** Statistc counters *******************************/
	const volatile uint32_t a_frames_received_ok;          /* 0x100 */
	const volatile uint64_t a_octets_received_ok;          /* 0x104 */
	const volatile uint32_t if_in_ucast_pkts;              /* 0x10C */
	const volatile uint32_t if_in_multicast_pkts;          /* 0x110 */
	const volatile uint32_t if_in_broadcast_pkts;          /* 0x114 */
	const volatile uint32_t a_frame_check_sequence_errors; /* 0x118 */
	const volatile uint32_t if_in_errors;                  /* 0x11C */
	const volatile uint32_t ether_stats_drop_events;       /* 0x120 */
	const volatile uint64_t ether_stats_octets;            /* 0x124 */
	const volatile uint32_t ether_stats_pkts;              /* 0x12C */
	const volatile uint32_t ether_stats_undersize_pkts;    /* 0x130 */
	const volatile uint32_t ether_stats_oversize_pkts;     /* 0x134 */
	const volatile uint32_t ether_stats_64_octets;         /* 0x138 */
	const volatile uint32_t ether_stats_65_127_octets;     /* 0x13C */
	const volatile uint32_t ether_stats_128_255_octets;    /* 0x140 */
	const volatile uint32_t ether_stats_256_511_octets;    /* 0x144 */
	const volatile uint32_t ether_stats_512_1023_octets;   /* 0x148 */
	const volatile uint32_t ether_stats_1024_1518_octets;  /* 0x14C */
	const volatile uint32_t ether_stats_1519_10240_octets; /* 0x150 */
	const volatile uint32_t ether_stats_jabbers;           /* 0x154         */
	const volatile uint32_t ether_stats_fragments;         /* 0x158         */
	const volatile uint32_t _skip03[9];                    /* 0x15C - 0x17C */
	const volatile uint32_t a_frames_transmitted_ok;       /* 0x180         */
	const volatile uint64_t a_octets_transmitted_ok;       /* 0x184         */
	const volatile uint32_t if_out_ucast_pkts;             /* 0x18C         */
	const volatile uint32_t if_out_multicast_pkts;         /* 0x190         */
	const volatile uint32_t if_out_broadcast_pkts;         /* 0x194         */
	const volatile uint32_t _skip04[26];                   /* 0x198 - 0x1FC */
	/****************************** RX channels *********************************/
	struct mgeth_rx_regs rx[RCM_MGETH_MAX_DMA_CHANNELS];   /* 0x200 - 0x9FC */
	/****************************** TX channels *********************************/
	struct mgeth_tx_regs tx[RCM_MGETH_MAX_DMA_CHANNELS];   /* 0xA00 - 0xDFC */
	/******************************* Reserved ***********************************/
	const volatile uint32_t _skip05[128];                  /* 0xE00 - 0xFFC */
} __attribute__ ((packed));

struct rcm_mgeth_long_desc {
	uint32_t usrdata_l;
	uint32_t usrdata_h;
	uint32_t memptr;
	uint32_t flags_length;
} __attribute__ ((packed, aligned(16)));

/* descriptor flags */
#define MGETH_BD_OWN       0x80000000
#define MGETH_BD_LINK      0x40000000
#define MGETH_BD_INT       0x20000000
#define MGETH_BD_STOP      0x10000000
#define MGETH_BD_LEN_MASK  0x00003fff

#endif /* SRC_DRIVERS_NET_RCM_MGETH_RCM_MGETH_H_ */
