/**
 * @file
 *
 * @date Sep 4, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_NET_DWC_GMAC_DWC_GMAC_H_
#define SRC_DRIVERS_NET_DWC_GMAC_DWC_GMAC_H_

#define DWC_GMAC_CONFIG          0x00000000  /* Configuration */
#define  DWC_GMAC_CONF_JD       (1 << 22)    /* jabber timer disable */
#define  DWC_GMAC_CONF_BE       (1 << 21)    /* Frame Burst Enable */
#define  DWC_GMAC_CONF_PS       (1 << 15)    /* GMII/MII */
#define  DWC_GMAC_CONF_FES      (1 << 14)    /* MII speed select */
#define  DWC_GMAC_CONF_DM       (1 << 11)    /* Full Duplex Enable */
#define  DWC_GMAC_CONF_ACS      (1 << 7)
#define  DWC_GMAC_CONF_TE       (1 << 3)
#define  DWC_GMAC_CONF_RE       (1 << 2)
#define DWC_GMAC_FRAME_FILTER       0x00000004 /* Frame Filter */
#define  DWC_GMAC_FRAME_FILTER_RA   (1 << 31) /* Receive All */
#define  DWC_GMAC_FRAME_FILTER_HPF  (1 << 10) /* Hash or Perfect Filter */
#define  DWC_GMAC_FRAME_FILTER_PM   (1 << 4)  /* Pass multicast */
#define  DWC_GMAC_FRAME_FILTER_HMC  (1 << 2)
#define  DWC_GMAC_FRAME_FILTER_HUC  (1 << 1)
#define  DWC_GMAC_FRAME_FILTER_PR   (1 << 0)  /* All Incoming Frames */

#define DWC_GMAC_MII_ADDR        0x00000010 /* MII Address */
#define  DWC_GMAC_GMII_ADDRESS_PA_MASK   0x1f		/* Phy device */
#define  DWC_GMAC_GMII_ADDRESS_PA_SHIFT  11
#define  DWC_GMAC_GMII_ADDRESS_GR_MASK   0x1f		/* Phy register */
#define  DWC_GMAC_GMII_ADDRESS_GR_SHIFT  6
#define  DWC_GMAC_GMII_ADDRESS_CR_MASK   0xf
#define  DWC_GMAC_GMII_ADDRESS_CR_SHIFT  2		/* Clock */
#define  DWC_GMAC_GMII_ADDRESS_GW        (1 << 1)	/* Write operation */
#define  DWC_GMAC_GMII_ADDRESS_GB        (1 << 0)	/* Busy */
#define DWC_GMAC_MII_DATA        0x00000014 /* MII Data */
#define DWC_GMAC_FLOW_CTRL       0x00000018 /* Flow Control */
#define DWC_GMAC_VLAN_TAG        0x0000001c /* VLAN Tag */
#define DWC_GMAC_VERSION         0x00000020 /* GMAC CORE Version */
#define DWC_GMAC_DEBUG           0x00000024 /* GMAC debug register */
#define DWC_GMAC_LPI_CONTROL_STATUS       0x30
#define DWC_GMAC_LPI_TIMERS_CONTROL       0x34
#define DWC_GMAC_INTERRUPT_STATUS         0x38
#define DWC_GMAC_INTERRUPT_MASK           0x3C
#define DWC_GMAC_ADDRESS_HIGH(n) ((n > 15 ? 0x800 : 0x40) + 0x8 * n)
#define DWC_GMAC_ADDRESS_LOW(n)  ((n > 15 ? 0x804 : 0x44) + 0x8 * n)

#define	DWC_GMAC_SGMII_RGMII_SMII_CTRL_STATUS 0xD8
#define	DWC_GMAC_MMC_CONTROL                  0x100
#define	 DWC_GMAC_MMC_CONTROL_CNTRST         (1 << 0)
#define	DWC_GMAC_MMC_RECEIVE_INTERRUPT        0x104
#define	DWC_GMAC_MMC_TRANSMIT_INTERRUPT       0x108
#define	DWC_GMAC_MMC_RECEIVE_INTERRUPT_MASK   0x10C
#define	DWC_GMAC_MMC_TRANSMIT_INTERRUPT_MASK  0x110
#define	DWC_GMAC_TXOCTETCOUNT_GB              0x114
#define	DWC_GMAC_TXFRAMECOUNT_GB              0x118
#define	DWC_GMAC_TXBROADCASTFRAMES_G          0x11C
#define	DWC_GMAC_TXMULTICASTFRAMES_G          0x120
#define	DWC_GMAC_TX64OCTETS_GB                0x124
#define	DWC_GMAC_TX65TO127OCTETS_GB           0x128
#define	DWC_GMAC_TX128TO255OCTETS_GB          0x12C
#define	DWC_GMAC_TX256TO511OCTETS_GB          0x130
#define	DWC_GMAC_TX512TO1023OCTETS_GB         0x134
#define	DWC_GMAC_TX1024TOMAXOCTETS_GB         0x138
#define	DWC_GMAC_TXUNICASTFRAMES_GB           0x13C
#define	DWC_GMAC_TXMULTICASTFRAMES_GB         0x140
#define	DWC_GMAC_TXBROADCASTFRAMES_GB         0x144
#define	DWC_GMAC_TXUNDERFLOWERROR             0x148
#define	DWC_GMAC_TXSINGLECOL_G                0x14C
#define	DWC_GMAC_TXMULTICOL_G                 0x150
#define	DWC_GMAC_TXDEFERRED                   0x154
#define	DWC_GMAC_TXLATECOL                    0x158
#define	DWC_GMAC_TXEXESSCOL                   0x15C
#define	DWC_GMAC_TXCARRIERERR                 0x160
#define	DWC_GMAC_TXOCTETCNT                   0x164
#define	DWC_GMAC_TXFRAMECOUNT_G               0x168
#define	DWC_GMAC_TXEXCESSDEF                  0x16C
#define	DWC_GMAC_TXPAUSEFRAMES                0x170
#define	DWC_GMAC_TXVLANFRAMES_G               0x174
#define	DWC_GMAC_TXOVERSIZE_G                 0x178
#define	DWC_GMAC_RXFRAMECOUNT_GB              0x180
#define	DWC_GMAC_RXOCTETCOUNT_GB              0x184
#define	DWC_GMAC_RXOCTETCOUNT_G               0x188
#define	DWC_GMAC_RXBROADCASTFRAMES_G          0x18C
#define	DWC_GMAC_RXMULTICASTFRAMES_G          0x190
#define	DWC_GMAC_RXCRCERROR                   0x194
#define	DWC_GMAC_RXALIGNMENTERROR             0x198
#define	DWC_GMAC_RXRUNTERROR                  0x19C
#define	DWC_GMAC_RXJABBERERROR                0x1A0
#define	DWC_GMAC_RXUNDERSIZE_G                0x1A4
#define	DWC_GMAC_RXOVERSIZE_G                 0x1A8
#define	DWC_GMAC_RX64OCTETS_GB                0x1AC
#define	DWC_GMAC_RX65TO127OCTETS_GB           0x1B0
#define	DWC_GMAC_RX128TO255OCTETS_GB          0x1B4
#define	DWC_GMAC_RX256TO511OCTETS_GB          0x1B8
#define	DWC_GMAC_RX512TO1023OCTETS_GB         0x1BC
#define	DWC_GMAC_RX1024TOMAXOCTETS_GB         0x1C0
#define	DWC_GMAC_RXUNICASTFRAMES_G            0x1C4
#define	DWC_GMAC_RXLENGTHERROR                0x1C8
#define	DWC_GMAC_RXOUTOFRANGETYPE             0x1CC
#define	DWC_GMAC_RXPAUSEFRAMES                0x1D0
#define	DWC_GMAC_RXFIFOOVERFLOW               0x1D4
#define	DWC_GMAC_RXVLANFRAMES_GB              0x1D8
#define	DWC_GMAC_RXWATCHDOGERROR              0x1DC
#define	DWC_GMAC_RXRCVERROR                   0x1E0
#define	DWC_GMAC_RXCTRLFRAMES_G               0x1E4
#define	DWC_GMAC_MMC_IPC_RECEIVE_INT_MASK     0x200
#define	DWC_GMAC_MMC_IPC_RECEIVE_INT          0x208
#define	DWC_GMAC_RXIPV4_GD_FRMS               0x210
#define	DWC_GMAC_RXIPV4_HDRERR_FRMS           0x214
#define	DWC_GMAC_RXIPV4_NOPAY_FRMS            0x218
#define	DWC_GMAC_RXIPV4_FRAG_FRMS             0x21C
#define	DWC_GMAC_RXIPV4_UDSBL_FRMS            0x220
#define	DWC_GMAC_RXIPV6_GD_FRMS               0x224
#define	DWC_GMAC_RXIPV6_HDRERR_FRMS           0x228
#define	DWC_GMAC_RXIPV6_NOPAY_FRMS            0x22C
#define	DWC_GMAC_RXUDP_GD_FRMS                0x230
#define	DWC_GMAC_RXUDP_ERR_FRMS               0x234
#define	DWC_GMAC_RXTCP_GD_FRMS                0x238
#define	DWC_GMAC_RXTCP_ERR_FRMS               0x23C
#define	DWC_GMAC_RXICMP_GD_FRMS               0x240
#define	DWC_GMAC_RXICMP_ERR_FRMS              0x244
#define	DWC_GMAC_RXIPV4_GD_OCTETS             0x250
#define	DWC_GMAC_RXIPV4_HDRERR_OCTETS         0x254
#define	DWC_GMAC_RXIPV4_NOPAY_OCTETS          0x258
#define	DWC_GMAC_RXIPV4_FRAG_OCTETS           0x25C
#define	DWC_GMAC_RXIPV4_UDSBL_OCTETS          0x260
#define	DWC_GMAC_RXIPV6_GD_OCTETS             0x264
#define	DWC_GMAC_RXIPV6_HDRERR_OCTETS         0x268
#define	DWC_GMAC_RXIPV6_NOPAY_OCTETS          0x26C
#define	DWC_GMAC_RXUDP_GD_OCTETS              0x270
#define	DWC_GMAC_RXUDP_ERR_OCTETS             0x274
#define	DWC_GMAC_RXTCP_GD_OCTETS              0x278
#define	DWC_GMAC_RXTCPERROCTETS               0x27C
#define	DWC_GMAC_RXICMP_GD_OCTETS             0x280
#define	DWC_GMAC_RXICMP_ERR_OCTETS            0x284
#define	DWC_GMAC_L3_L4_CONTROL0               0x400
#define	DWC_GMAC_LAYER4_ADDRESS0              0x404
#define	DWC_GMAC_LAYER3_ADDR0_REG0            0x410
#define	DWC_GMAC_LAYER3_ADDR1_REG0            0x414
#define	DWC_GMAC_LAYER3_ADDR2_REG0            0x418
#define	DWC_GMAC_LAYER3_ADDR3_REG0            0x41C
#define	DWC_GMAC_L3_L4_CONTROL1               0x430
#define	DWC_GMAC_LAYER4_ADDRESS1              0x434
#define	DWC_GMAC_LAYER3_ADDR0_REG1            0x440
#define	DWC_GMAC_LAYER3_ADDR1_REG1            0x444
#define	DWC_GMAC_LAYER3_ADDR2_REG1            0x448
#define	DWC_GMAC_LAYER3_ADDR3_REG1            0x44C
#define	DWC_GMAC_L3_L4_CONTROL2               0x460
#define	DWC_GMAC_LAYER4_ADDRESS2              0x464
#define	DWC_GMAC_LAYER3_ADDR0_REG2            0x470
#define	DWC_GMAC_LAYER3_ADDR1_REG2            0x474
#define	DWC_GMAC_LAYER3_ADDR2_REG2            0x478
#define	DWC_GMAC_LAYER3_ADDR3_REG2            0x47C
#define	DWC_GMAC_L3_L4_CONTROL3               0x490
#define	DWC_GMAC_LAYER4_ADDRESS3              0x494
#define	DWC_GMAC_LAYER3_ADDR0_REG3            0x4A0
#define	DWC_GMAC_LAYER3_ADDR1_REG3            0x4A4
#define	DWC_GMAC_LAYER3_ADDR2_REG3            0x4A8
#define	DWC_GMAC_LAYER3_ADDR3_REG3            0x4AC
#define	DWC_GMAC_HASH_TABLE_REG(n)            0x500 + (0x4 * n)
#define	DWC_GMAC_VLAN_INCL_REG                0x584
#define	DWC_GMAC_VLAN_HASH_TABLE_REG          0x588
#define	DWC_GMAC_TIMESTAMP_CONTROL            0x700
#define	DWC_GMAC_SUB_SECOND_INCREMENT         0x704
#define	DWC_GMAC_SYSTEM_TIME_SECONDS          0x708
#define	DWC_GMAC_SYSTEM_TIME_NANOSECONDS      0x70C
#define	DWC_GMAC_SYSTEM_TIME_SECONDS_UPDATE   0x710
#define	DWC_GMAC_SYSTEM_TIME_NANOSECONDS_UPDATE 0x714
#define	DWC_GMAC_TIMESTAMP_ADDEND             0x718
#define	DWC_GMAC_TARGET_TIME_SECONDS          0x71C
#define	DWC_GMAC_TARGET_TIME_NANOSECONDS      0x720
#define	DWC_GMAC_SYSTEM_TIME_HIGHER_WORD_SECONDS 0x724
#define	DWC_GMAC_TIMESTAMP_STATUS             0x728
#define	DWC_GMAC_PPS_CONTROL                  0x72C
#define	DWC_GMAC_AUXILIARY_TIMESTAMP_NANOSECONDS 0x730
#define	DWC_GMAC_AUXILIARY_TIMESTAMP_SECONDS  0x734
#define	DWC_GMAC_PPS0_INTERVAL                0x760
#define	DWC_GMAC_PPS0_WIDTH                   0x764

/* DMA */
#define	DWC_DMA_BUS_MODE               0x1000
#define	 DWC_DMA_BUS_MODE_EIGHTXPBL   (1 << 24) /* Multiplies PBL by 8 */
#define	 DWC_DMA_BUS_MODE_PBL_SHIFT    8 /* Single block transfer size */
#define	 DWC_DMA_BUS_MODE_PBL_BEATS_8  8
#define	 DWC_DMA_BUS_MODE_ENHDESC_USE (1 << 7)
#define	 DWC_DMA_BUS_MODE_SWR         (1 << 0) /* Reset */

#define	DWC_DMA_TRANSMIT_POLL_DEMAND  0x1004
#define	DWC_DMA_RECEIVE_POLL_DEMAND   0x1008
#define	DWC_DMA_RX_DESCR_LIST_ADDR    0x100C
#define	DWC_DMA_TX_DESCR_LIST_ADDR    0x1010
#define	DWC_DMA_STATUS                0x1014
#define	 DWC_DMA_STATUS_NIS          (1 << 16)
#define	 DWC_DMA_STATUS_AIS          (1 << 15)
#define	 DWC_DMA_STATUS_FBI          (1 << 13)
#define	 DWC_DMA_STATUS_RI           (1 << 6)
#define	 DWC_DMA_STATUS_TI           (1 << 0)
#define	 DWC_DMA_STATUS_FSM_MASK     ((7 << 20) | (7 << 17))
#define	 DWC_DMA_STATUS_INTR_MASK     0x1ffff
#define	DWC_DMA_OPERATION_MODE        0x1018
#define	 DWC_DMA_MODE_RSF            (1 << 25) /* RX Full Frame */
#define	 DWC_DMA_MODE_TSF            (1 << 21) /* TX Full Frame */
#define	 DWC_DMA_MODE_FTF            (1 << 20) /* Flush TX FIFO */
#define	 DWC_DMA_MODE_ST             (1 << 13) /* Start DMA TX */
#define	 DWC_DMA_MODE_FUF            (1 << 6)  /* TX frames < 64bytes */
#define	 DWC_DMA_MODE_RTC_LEV32       0x1
#define	 DWC_DMA_MODE_RTC_SHIFT       3
#define	 DWC_DMA_MODE_OSF            (1 << 2) /* Process Second frame */
#define	 DWC_DMA_MODE_SR             (1 << 1) /* Start DMA RX */
#define	DWC_DMA_INTERRUPT_ENABLE      0x101C
#define	 DWC_DMA_INT_EN_NIE          (1 << 16) /* Normal/Summary */
#define	 DWC_DMA_INT_EN_AIE          (1 << 15) /* Abnormal/Summary */
#define	 DWC_DMA_INT_EN_ERE          (1 << 14) /* Early receive */
#define	 DWC_DMA_INT_EN_FBE          (1 << 13) /* Fatal bus error */
#define	 DWC_DMA_INT_EN_ETE          (1 << 10) /* Early transmit */
#define	 DWC_DMA_INT_EN_RWE          (1 << 9)  /* Receive watchdog */
#define	 DWC_DMA_INT_EN_RSE          (1 << 8)  /* Receive stopped */
#define	 DWC_DMA_INT_EN_RUE          (1 << 7)  /* Recv buf unavailable */
#define	 DWC_DMA_INT_EN_RIE          (1 << 6)  /* Receive interrupt */
#define	 DWC_DMA_INT_EN_UNE          (1 << 5)  /* Tx underflow */
#define	 DWC_DMA_INT_EN_OVE          (1 << 4)  /* Receive overflow */
#define	 DWC_DMA_INT_EN_TJE          (1 << 3)  /* Transmit jabber */
#define	 DWC_DMA_INT_EN_TUE          (1 << 2)  /* Tx. buf unavailable */
#define	 DWC_DMA_INT_EN_TSE          (1 << 1)  /* Transmit stopped */
#define	 DWC_DMA_INT_EN_TIE          (1 << 0)  /* Transmit interrupt */
#define	 DWC_DMA_INT_EN_DEFAULT      (DWC_DMA_INT_EN_TIE|DWC_DMA_INT_EN_RIE|	\
	    DWC_DMA_INT_EN_NIE|DWC_DMA_INT_EN_AIE|			\
	    DWC_DMA_INT_EN_FBE|DWC_DMA_INT_EN_UNE)

#define DWC_DMA_MISSED_FRAMEBUF_OVERFLOW_CNTR  0x1020
#define DWC_DMA_RECEIVE_INT_WATCHDOG_TMR       0x1024
#define DWC_DMA_AXI_BUS_MODE                   0x1028
#define DWC_DMA_AHB_OR_AXI_STATUS              0x102C
#define DWC_DMA_CURRENT_HOST_TRANSMIT_DESCR    0x1048
#define DWC_DMA_CURRENT_HOST_RECEIVE_DESCR     0x104C
#define DWC_DMA_CURRENT_HOST_TRANSMIT_BUF_ADDR 0x1050
#define DWC_DMA_CURRENT_HOST_RECEIVE_BUF_ADDR  0x1054
#define DWC_DMA_HW_FEATURE                     0x1058

#endif /* SRC_DRIVERS_NET_DWC_GMAC_DWC_GMAC_H_ */
