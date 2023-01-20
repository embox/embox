/**
 * @file
 *
 * @date Mar 29, 2023
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_NET_RCM_MGETH_RCM_MGETH_H_
#define SRC_DRIVERS_NET_RCM_MGETH_RCM_MGETH_H_

#include <stdint.h>

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



typedef const volatile uint32_t roreg32;
typedef volatile uint32_t rwreg32;
typedef const volatile unsigned long long roreg64;
typedef volatile unsigned long long rwreg64;


typedef struct _mgeth_rx_regs {
/************************* RX Channel registers *****************************/
    rwreg32 rx_eth_mask_value[32];          /* 0x000-0x07C - packets mask   */
    rwreg32 rx_eth_mask_activ[32];          /* 0x080-0x0FC - mask activation*/
/*********************** WDMA Channel registers *****************************/
    rwreg32 enable;                         /* 0x100 - enable channel       */
    rwreg32 suspend;                        /* 0x104 - suspend channel      */
    rwreg32 cancel;                         /* 0x108 - cancel channel       */
    roreg32 _skip01;                        /* 0x10C                        */
    rwreg32 settings;                       /* 0x110 - channel settings     */
    rwreg32 irq_mask;                       /* 0x114 - channel irq mask     */
    roreg32 status;                         /* 0x118 - channel status       */
    roreg32 _skip02;                        /* 0x11C                        */
    rwreg32 desc_addr;                      /* 0x120 - first decriptor addr */
    roreg32 _skip03;                        /* 0x124                        */
    roreg32 curr_desc_addr;                 /* 0x128 - current decript addr */
    roreg32 curr_addr;                      /* 0x12C - current trans addr   */
    roreg32 dma_state;                      /* 0x130 - state of DMA         */
    roreg32 _skip04[3];                     /* 0x134 - 0x13C                */
    rwreg32 desc_axlen;                     /* 0x140 - axlen for desc ops   */
    rwreg32 desc_axcache;                   /* 0x144 - axcache for desc ops */
    rwreg32 desc_axprot;                    /* 0x148 - axprot for desc ops  */
    rwreg32 desc_axlock;                    /* 0x14C - axlock for desc ops  */
    roreg32 desc_rresp;                     /* 0x150 - rresp of desc ops    */
    roreg32 desc_raxi_err_addr;             /* 0x154 - addr of axi read err */
    roreg32 desc_bresp;                     /* 0x158 - bresp of desc ops    */
    roreg32 desc_waxi_err_addr;             /* 0x15C - addr of axi write err*/
    rwreg32 desc_permut;                    /* 0x160 - byte swapping scheme */
    roreg32 _skip05[7];                     /* 0x164 - 0x17C                */
    rwreg32 max_trans;                      /* 0x180 - max unfinished trans */
    rwreg32 awlen;                          /* 0x184 - axi awlen            */
    rwreg32 awcache;                        /* 0x188 - axi awcache          */
    rwreg32 awprot;                         /* 0x18C - axi awprot           */
    rwreg32 awlock;                         /* 0x190 - axi awlock           */
    roreg32 bresp;                          /* 0x194 - axi operation bresp  */
    roreg32 waxi_err_addr;                  /* 0x198 - addr of axi write err*/
    roreg32 _skip06;                        /* 0x19C                        */
    roreg32 state;                          /* 0x1A0 - axi state            */
    roreg32 avaliable_space;                /* 0x1A4 - num of free bytes    */
    rwreg32 permutation;                    /* 0x1A8 - byte swapping scheme */
    roreg32 _skip07[5];                     /* 0x1AC - 0x1BC                */
/************************** Statistc counters *******************************/
    roreg32 a_frames_received_ok;           /* 0x1C0                        */
    roreg64 a_octets_received_ok;           /* 0x1C4                        */
    roreg32 if_in_ucast_pkts;               /* 0x1CC                        */
    roreg32 if_in_multicast_pkts;           /* 0x1D0                        */
    roreg32 if_in_broadcast_pkts;           /* 0x1D4                        */
    roreg32 descriptor_short;               /* 0x1D8                        */
    roreg32 rtp_overmuch_line;              /* 0x1DC                        */
    roreg32 _skip08[8];                     /* 0x1E0 - 0x1FC                */
} __attribute__ ((packed)) mgeth_rx_regs;

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

typedef struct _mgeth_tx_regs {
    rwreg32 enable;                         /* 0x000 - enable channel       */
    rwreg32 suspend;                        /* 0x004 - suspend channel      */
    rwreg32 cancel;                         /* 0x008 - cancel channel       */
    roreg32 _skip01;                        /* 0x00C                        */
    rwreg32 settings;                       /* 0x010 - channel settings     */
    rwreg32 irq_mask;                       /* 0x014 - channel irq mask     */
    roreg32 status;                         /* 0x018 - channel status       */
    roreg32 _skip02;                        /* 0x01C                        */
    rwreg32 desc_addr;                      /* 0x020 - first decriptor addr */
    roreg32 _skip03;                        /* 0x024                        */
    roreg32 curr_desc_addr;                 /* 0x028 - current decript addr */
    roreg32 curr_addr;                      /* 0x02C - current trans addr   */
    roreg32 dma_state;                      /* 0x030 - state of DMA         */
    roreg32 _skip04[3];                     /* 0x034 - 0x13C                */
    rwreg32 desc_axlen;                     /* 0x040 - axlen for desc ops   */
    rwreg32 desc_axcache;                   /* 0x044 - axcache for desc ops */
    rwreg32 desc_axprot;                    /* 0x048 - axprot for desc ops  */
    rwreg32 desc_axlock;                    /* 0x04C - axlock for desc ops  */
    roreg32 desc_rresp;                     /* 0x050 - rresp of desc ops    */
    roreg32 desc_raxi_err_addr;             /* 0x054 - addr of axi read err */
    roreg32 desc_bresp;                     /* 0x058 - bresp of desc ops    */
    roreg32 desc_waxi_err_addr;             /* 0x05C - addr of axi write err*/
    rwreg32 desc_permut;                    /* 0x060 - byte swapping scheme */
    roreg32 _skip05[7];                     /* 0x064 - 0x17C                */
    rwreg32 max_trans;                      /* 0x080 - max unfinished trans */
    rwreg32 arlen;                          /* 0x084 - axi arlen            */
    rwreg32 arcache;                        /* 0x088 - axi arcache          */
    rwreg32 arprot;                         /* 0x08C - axi arprot           */
    rwreg32 arlock;                         /* 0x090 - axi arlock           */
    roreg32 rresp;                          /* 0x094 - axi operation rresp  */
    roreg32 waxi_err_addr;                  /* 0x098 - addr of axi write err*/
    roreg32 _skip06;                        /* 0x09C                        */
    roreg32 state;                          /* 0x0A0 - axi state            */
    roreg32 avaliable_space;                /* 0x0A4 - num of free bytes    */
    rwreg32 permutation;                    /* 0x0A8 - byte swapping scheme */
    roreg32 _skip07[5];                     /* 0x0AC - 0x1BC                */
/************************** Statistc counters *******************************/
    roreg32 a_frames_transmitted_ok;        /* 0x0C0                        */
    roreg64 a_octets_transmitted_ok;        /* 0x0C4                        */
    roreg32 if_out_ucast_pkts;              /* 0x0CC                        */
    roreg32 if_out_multicast_pkts;          /* 0x0D0                        */
    roreg32 if_out_broadcast_pkts;          /* 0x0D4                        */
    roreg32 _skip08[10];                    /* 0x0D8 - 0x0FC                */
} __attribute__ ((packed)) mgeth_tx_regs;

typedef struct _mgeth_regs {
/***************** Common registers for MGETH and MDMA **********************/
    roreg32 id;                             /* 0x000 - device id            */
    roreg32 version;                        /* 0x004 - device version       */
    rwreg32 sw_rst;                         /* 0x008 - program reset        */
    roreg32 global_status;                  /* 0x00C - status               */
/**************************** MGETH registers *******************************/
    roreg32 mg_status;                      /* 0x010 - mgeth status         */
    roreg32 _skip01;                        /* 0x014                        */
    rwreg32 mg_irq_mask;                    /* 0x018 - mgeth irq mask       */
    rwreg32 mg_control;                     /* 0x01C - mgeth control reg    */
    rwreg32 mg_len_mask_ch0;                /* 0x020 - mgeth mask len ch0   */
    rwreg32 mg_len_mask_ch1;                /* 0x024 - mgeth mask len ch1   */
    rwreg32 mg_len_mask_ch2;                /* 0x028 - mgeth mask len ch2   */
    rwreg32 mg_len_mask_ch3;                /* 0x02C - mgeth mask len ch3   */
    rwreg32 tx0_delay_timer;                /* 0x030 - delay timer for tx0  */
    rwreg32 tx1_delay_timer;                /* 0x034 - delay timer for tx1  */
    rwreg32 tx2_delay_timer;                /* 0x038 - delay timer for tx2  */
    rwreg32 tx3_delay_timer;                /* 0x03C - delay timer for tx3  */
    rwreg32 hd_sgmii_mode;                  /* 0x040 - SGMII mode           */
    roreg32 _skip02[47];                    /* 0x044 - 0x0FC                */
/************************** Statistc counters *******************************/
    roreg32 a_frames_received_ok;           /* 0x100                        */
    roreg64 a_octets_received_ok;           /* 0x104                        */
    roreg32 if_in_ucast_pkts;               /* 0x10C                        */
    roreg32 if_in_multicast_pkts;           /* 0x110                        */
    roreg32 if_in_broadcast_pkts;           /* 0x114                        */
    roreg32 a_frame_check_sequence_errors;  /* 0x118                        */
    roreg32 if_in_errors;                   /* 0x11C                        */
    roreg32 ether_stats_drop_events;        /* 0x120                        */
    roreg64 ether_stats_octets;             /* 0x124                        */
    roreg32 ether_stats_pkts;               /* 0x12C                        */
    roreg32 ether_stats_undersize_pkts;     /* 0x130                        */
    roreg32 ether_stats_oversize_pkts;      /* 0x134                        */
    roreg32 ether_stats_64_octets;          /* 0x138                        */
    roreg32 ether_stats_65_127_octets;      /* 0x13C                        */
    roreg32 ether_stats_128_255_octets;     /* 0x140                        */
    roreg32 ether_stats_256_511_octets;     /* 0x144                        */
    roreg32 ether_stats_512_1023_octets;    /* 0x148                        */
    roreg32 ether_stats_1024_1518_octets;   /* 0x14C                        */
    roreg32 ether_stats_1519_10240_octets;  /* 0x150                        */
    roreg32 ether_stats_jabbers;            /* 0x154                        */
    roreg32 ether_stats_fragments;          /* 0x158                        */
    roreg32 _skip03[9];                     /* 0x15C - 0x17C                */
    roreg32 a_frames_transmitted_ok;        /* 0x180                        */
    roreg64 a_octets_transmitted_ok;        /* 0x184                        */
    roreg32 if_out_ucast_pkts;              /* 0x18C                        */
    roreg32 if_out_multicast_pkts;          /* 0x190                        */
    roreg32 if_out_broadcast_pkts;          /* 0x194                        */
    roreg32 _skip04[26];                    /* 0x198 - 0x1FC                */
/****************************** RX channels *********************************/
    mgeth_rx_regs rx[4];                    /* 0x200 - 0x9FC                */
/****************************** TX channels *********************************/
    mgeth_tx_regs tx[4];                    /* 0xA00 - 0xDFC                */
/******************************* Reserved ***********************************/
    roreg32 _skip05[128];                   /* 0xE00 - 0xFFC                */
}   __attribute__ ((packed)) mgeth_regs;


struct rcm_mgeth_long_desc {
	uint32_t usrdata_l;
	uint32_t usrdata_h;
	uint32_t memptr;
	uint32_t flags_length;
} __attribute__ ((packed, aligned(16)));

/* descriptor flags */
#define MGETH_BD_OWN 0x80000000
#define MGETH_BD_LINK 0x40000000
#define MGETH_BD_INT  0x20000000
#define MGETH_BD_STOP 0x10000000

#endif /* SRC_DRIVERS_NET_RCM_MGETH_RCM_MGETH_H_ */
