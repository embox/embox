/**
 * @file
 *
 * @date Feb 2, 2022
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_PCI_PCI_REGS_H_
#define SRC_DRIVERS_PCI_PCI_REGS_H_

/**
 * PCI configuration space
 * (Each device on the bus has a 256 bytes configuration space,
 * the first 64 bytes are standardized)
 * (according to PCI Local Bus Specification 2.2
 * DeviceID, VendorID, Status, Command, Class Code,
 * Revision ID, Header Type are required)
 */
#define PCI_VENDOR_ID               0x00  /* 16 bits */
#define PCI_DEVICE_ID               0x02  /* 16 bits */
#define PCI_COMMAND                 0x04  /* 16 bits */
#define PCI_COMMAND_IO              0x001 /* Enable response in I/O space */
#define PCI_COMMAND_MEMORY          0x002 /* Enable response in Memory space */
#define PCI_COMMAND_MASTER          0x004 /* Enable bus mastering */
#define PCI_COMMAND_SPECIAL         0x008 /* Enable response to special cycles */
#define PCI_COMMAND_INVALIDATE      0x010 /* Use memory write and invalidate */
#define PCI_COMMAND_VGA_PALETTE     0x020 /* Enable palette snooping */
#define PCI_COMMAND_PARITY          0x040 /* Enable parity checking */
#define PCI_COMMAND_WAIT            0x080 /* Enable address/data stepping */
#define PCI_COMMAND_SERR            0x100 /* Enable SERR */
#define PCI_COMMAND_FAST_BACK       0x200 /* Enable back-to-back writes */
#define PCI_COMMAND_INTX_DISABLE    0x400 /* INTx Emulation Disable */

#define PCI_STATUS                  0x06 /* 16 bits */
#define PCI_STATUS_INTERRUPT        0x08 /* Interrupt status */
#define PCI_STATUS_CAP_LIST         0x10 /* Support Capability List */
#define PCI_STATUS_66MHZ            0x20 /* Support 66 MHz PCI 2.1 bus */
#define PCI_STATUS_UDF              0x40 /* Support User Definable Features [obsolete] */
#define PCI_STATUS_FAST_BACK        0x80  /* Accept fast-back to back */
#define PCI_STATUS_PARITY           0x100 /* Detected parity error */
#define PCI_STATUS_DEVSEL_MASK      0x600 /* DEVSEL timing */
#define PCI_STATUS_DEVSEL_FAST      0x000
#define PCI_STATUS_DEVSEL_MEDIUM    0x200
#define PCI_STATUS_DEVSEL_SLOW      0x400
#define PCI_STATUS_SIG_TARGET_ABORT 0x800  /* Set on target abort */
#define PCI_STATUS_REC_TARGET_ABORT 0x1000 /* Master ack of " */
#define PCI_STATUS_REC_MASTER_ABORT 0x2000 /* Set on master abort */
#define PCI_STATUS_SIG_SYSTEM_ERROR 0x4000 /* Set when we drive SERR */
#define PCI_STATUS_DETECTED_PARITY  0x8000 /* Set on parity error */

#define PCI_REVISION_ID             0x08 /* 8 bits  */
#define PCI_PROG_IFACE              0x09 /* 8 bits  */
#define PCI_SUBCLASS_CODE           0x0a /* 8 bits  */
#define PCI_BASECLASS_CODE          0x0b /* 8 bits  */
#define PCI_CACHE_LINE_SIZE         0x0C /* 8 bits  */
#define PCI_LATENCY_TIMER           0x0D /* 8 bits  */
#define PCI_HEADER_TYPE             0x0e /* 8 bits  */
#define PCI_BIST                    0x0f /* 8 bits  */

#define PCI_VENDOR_WRONG            0xFFFFFFFF /* device is not found in the slot */
#define PCI_VENDOR_NONE             0x00000000 /* device is not found in the slot */

/**
 * +------------+------------+---------+-----------+
 * |31         4|           3|2       1|          0|
 * +------------+------------+---------+-----------+
 * |Base Address|Prefetchable|Locatable|region type|
 * +------------+------------+---------+-----------+
 * \___________________________________/
 *           For Memory BARs
 * +--------------------------+--------+-----------+
 * |31                       2|       1|          0|
 * +--------------------------+--------+-----------+
 * |Base Address              |Reserved|region type|
 * +--------------------------+--------+-----------+
 * \___________________________________/
 *           For I/O BARs (Deprecated)
 * region type:  0 = Memory, 1 = I/O (deprecated)
 * Locatable:    0 = any 32-bit, 1 = < 1MiB, 2 = any 64-bit
 * Prefetchable: 0 = no, 1 = yes
 * Base Address: 16-byte aligned
 */
#define PCI_BASE_ADDR_REG_0         0x10 /* 32 bits */
#define PCI_BASE_ADDR_REG_1         0x14 /* 32 bits */
#define PCI_BASE_ADDR_REG_2         0x18 /* 32 bits */
#define PCI_BASE_ADDR_REG_3         0x1C /* 32 bits */
#define PCI_BASE_ADDR_REG_4         0x20 /* 32 bits */
#define PCI_BASE_ADDR_REG_5         0x24 /* 32 bits */
#define PCI_BASE_ADDR_IO_MASK       (~0x03)

/* Header type 0 (normal devices) */
#define PCI_CARDBUS_CIS             0x28 /* 32 bits */
#define PCI_SUBSYSTEM_VENDOR_ID     0x2C /* 16 bits */
#define PCI_SUBSYSTEM_ID            0x2E /* 16 bits */
/* Bits 31..11 are address, 10..1 reserved */
#define PCI_ROM_ADDRESS             0x30
#define PCI_ROM_ADDRESS_ENABLE      0x01
#define PCI_ROM_ADDRESS_MASK        (~0x7ffUL)

/* Offset of first capability list entry */
#define PCI_CAPABILITY_LIST         0x34

/* 0x35-0x3b are reserved */
#define PCI_INTERRUPT_LINE          0x3C /* 8 bits  */
#define PCI_INTERRUPT_PIN           0x3D /* 8 bits  */
#define PCI_MIN_GNT                 0x3E /* 8 bits  */
#define PCI_MAX_LAT                 0x3F /* 8 bits  */

/* Header type 1 (PCI-to-PCI bridges) */
#define PCI_PRIMARY_BUS             0x18
#define PCI_SECONDARY_BUS           0x19
#define PCI_SUBORDINATE_BUS         0x1A

/* Capability lists */

#define PCI_CAP_LIST_ID             0 /* Capability ID */
#define PCI_CAP_ID_NULL             0x00
#define PCI_CAP_ID_PM               0x01 /* Power Management */
#define PCI_CAP_ID_AGP              0x02 /* Accelerated Graphics Port */
#define PCI_CAP_ID_VPD              0x03 /* Vital Product Data */
#define PCI_CAP_ID_SLOTID           0x04 /* Slot Identification */
#define PCI_CAP_ID_MSI              0x05 /* Message Signalled Interrupts */
#define PCI_CAP_ID_CHSWP            0x06 /* CompactPCI HotSwap */
#define PCI_CAP_ID_PCIX             0x07 /* PCI-X */
#define PCI_CAP_ID_HT               0x08 /* HyperTransport */
#define PCI_CAP_ID_VNDR             0x09 /* Vendor-Specific */
#define PCI_CAP_ID_DBG              0x0A /* Debug port */
#define PCI_CAP_ID_CCRC             0x0B /* CompactPCI Central Resource Control */
#define PCI_CAP_ID_SHPC             0x0C /* PCI Standard Hot-Plug Controller */
#define PCI_CAP_ID_HOTPLUG          PCI_CAP_ID_SHPC
#define PCI_CAP_ID_SSVID            0x0D /* Bridge subsystem vendor/device ID */
#define PCI_CAP_ID_AGP3             0x0E /* AGP Target PCI-PCI bridge */
#define PCI_CAP_ID_SECDEV           0x0F /* Secure Device */
#define PCI_CAP_ID_SECURE           PCI_CAP_ID_SECDEV
#define PCI_CAP_ID_EXP              0x10 /* PCI Express */
#define PCI_CAP_ID_MSIX             0x11 /* MSI-X */
#define PCI_CAP_ID_SATA             0x12 /* SATA Data/Index Conf. */
#define PCI_CAP_ID_AF               0x13 /* PCI Advanced Features */
#define PCI_CAP_ID_EA               0x14 /* PCI Enhanced Allocation */
#define PCI_CAP_ID_MAX              PCI_CAP_ID_EA
#define PCI_CAP_LIST_NEXT           1 /* Next capability in the list */
#define PCI_CAP_FLAGS               2 /* Capability defined flags (16 bits) */
#define PCI_CAP_SIZEOF              4

/* Power Management Registers */

#define PCI_PM_PMC                  2      /* PM Capabilities Register */
#define PCI_PM_CAP_VER_MASK         0x0007 /* Version */
#define PCI_PM_CAP_PME_CLOCK        0x0008 /* PME clock required */
#define PCI_PM_CAP_RESERVED         0x0010 /* Reserved field */
#define PCI_PM_CAP_DSI              0x0020 /* Device specific initialization */
#define PCI_PM_CAP_AUX_POWER        0x01C0 /* Auxiliary power support mask */
#define PCI_PM_CAP_D1               0x0200 /* D1 power state support */
#define PCI_PM_CAP_D2               0x0400 /* D2 power state support */
#define PCI_PM_CAP_PME              0x0800 /* PME pin supported */
#define PCI_PM_CAP_PME_MASK         0xF800 /* PME Mask of all supported states */
#define PCI_PM_CAP_PME_D0           0x0800 /* PME# from D0 */
#define PCI_PM_CAP_PME_D1           0x1000 /* PME# from D1 */
#define PCI_PM_CAP_PME_D2           0x2000 /* PME# from D2 */
#define PCI_PM_CAP_PME_D3           0x4000 /* PME# from D3 (hot) */
#define PCI_PM_CAP_PME_D3cold       0x8000 /* PME# from D3 (cold) */
#define PCI_PM_CAP_PME_SHIFT        11     /* Start of the PME Mask in PMC */
#define PCI_PM_CTRL                 4      /* PM control and status register */
#define PCI_PM_CTRL_STATE_MASK      0x0003 /* Current power state (D0 to D3) */
#define PCI_PM_CTRL_NO_SOFT_RESET   0x0008 /* No reset for D3hot->D0 */
#define PCI_PM_CTRL_PME_ENABLE      0x0100 /* PME pin enable */
#define PCI_PM_CTRL_DATA_SEL_MASK   0x1e00 /* Data select (??) */
#define PCI_PM_CTRL_DATA_SCALE_MASK 0x6000 /* Data scale (??) */
#define PCI_PM_CTRL_PME_STATUS      0x8000 /* PME pin status */
#define PCI_PM_PPB_EXTENSIONS       6      /* PPB support extensions (??) */
#define PCI_PM_PPB_B2_B3            0x40   /* Stop clock when in D3hot (??) */
#define PCI_PM_BPCC_ENABLE          0x80 /* Bus power/clock control enable (??) */
#define PCI_PM_DATA_REGISTER        7
#define PCI_PM_SIZEOF               8

/* AGP registers */

#define PCI_AGP_VERSION             2          /* BCD version number */
#define PCI_AGP_RFU                 3          /* Rest of capability flags */
#define PCI_AGP_STATUS              4          /* Status register */
#define PCI_AGP_STATUS_RQ_MASK      0xff000000 /* Maximum number of requests - 1 */
#define PCI_AGP_STATUS_SBA          0x0200 /* Sideband addressing supported */
#define PCI_AGP_STATUS_64BIT        0x0020 /* 64-bit addressing supported */
#define PCI_AGP_STATUS_FW           0x0010 /* FW transfers supported */
#define PCI_AGP_STATUS_RATE4        0x0004 /* 4x transfer rate supported */
#define PCI_AGP_STATUS_RATE2        0x0002 /* 2x transfer rate supported */
#define PCI_AGP_STATUS_RATE1        0x0001 /* 1x transfer rate supported */
#define PCI_AGP_COMMAND             8      /* Control register */
#define PCI_AGP_COMMAND_RQ_MASK \
	0xff000000 /* Master: Maximum number of requests */
#define PCI_AGP_COMMAND_SBA         0x0200 /* Sideband addressing enabled */
#define PCI_AGP_COMMAND_AGP         0x0100 /* Allow processing of AGP transactions */
#define PCI_AGP_COMMAND_64BIT       0x0020 /* Allow processing of 64-bit addresses */
#define PCI_AGP_COMMAND_FW          0x0010 /* Force FW transfers */
#define PCI_AGP_COMMAND_RATE4       0x0004 /* Use 4x rate */
#define PCI_AGP_COMMAND_RATE2       0x0002 /* Use 2x rate */
#define PCI_AGP_COMMAND_RATE1       0x0001 /* Use 1x rate */
#define PCI_AGP_SIZEOF              12

/* Vital Product Data */

#define PCI_VPD_ADDR                2      /* Address to access (15 bits!) */
#define PCI_VPD_ADDR_MASK           0x7fff /* Address mask */
#define PCI_VPD_ADDR_F              0x8000 /* Write 0, 1 indicates completion */
#define PCI_VPD_DATA                4      /* 32-bits of data returned here */
#define PCI_CAP_VPD_SIZEOF          8

/* Slot Identification */

#define PCI_SID_ESR                 2    /* Expansion Slot Register */
#define PCI_SID_ESR_NSLOTS          0x1f /* Number of expansion slots available */
#define PCI_SID_ESR_FIC             0x20 /* First In Chassis Flag */
#define PCI_SID_CHASSIS_NR          3    /* Chassis Number */

/* Message Signalled Interrupts registers */

#define PCI_MSI_FLAGS               2      /* Message Control */
#define PCI_MSI_FLAGS_ENABLE        0x0001 /* MSI feature enabled */
#define PCI_MSI_FLAGS_QMASK         0x000e /* Maximum queue size available */
#define PCI_MSI_FLAGS_QSIZE         0x0070 /* Message queue size configured */
#define PCI_MSI_FLAGS_64BIT         0x0080 /* 64-bit addresses allowed */
#define PCI_MSI_FLAGS_MASKBIT       0x0100 /* Per-vector masking capable */
#define PCI_MSI_FLAGS_MASK_BIT      PCI_MSI_FLAGS_MASKBIT
#define PCI_MSI_RFU                 3 /* Rest of capability flags */
#define PCI_MSI_ADDRESS_LO          4 /* Lower 32 bits */
#define PCI_MSI_ADDRESS_HI          8 /* Upper 32 bits (if PCI_MSI_FLAGS_64BIT set) */
#define PCI_MSI_DATA_32             8 /* 16 bits of data for 32-bit devices */
#define PCI_MSI_MASK_32             12 /* Mask bits register for 32-bit devices */
#define PCI_MSI_PENDING_32          16 /* Pending intrs for 32-bit devices */
#define PCI_MSI_MASK_BIT_32         PCI_MSI_PENDING_32
#define PCI_MSI_DATA_64             12 /* 16 bits of data for 64-bit devices */
#define PCI_MSI_MASK_64             16 /* Mask bits register for 64-bit devices */
#define PCI_MSI_MASK_BIT_64         PCI_MSI_MASK_64
#define PCI_MSI_PENDING_64          20 /* Pending intrs for 64-bit devices */

/* MSI-X registers */
#define PCI_MSIX_FLAGS              2      /* Message Control */
#define PCI_MSIX_FLAGS_QSIZE        0x07FF /* Table size */
#define PCI_MSIX_FLAGS_MASKALL      0x4000 /* Mask all vectors for this function */
#define PCI_MSIX_FLAGS_ENABLE       0x8000     /* MSI-X enable */
#define PCI_MSIX_TABLE              4          /* Table offset */
#define PCI_MSIX_TABLE_BIR          0x00000007 /* BAR index */
#define PCI_MSIX_TABLE_OFFSET       0xfffffff8 /* Offset into specified BAR */
#define PCI_MSIX_PBA                8          /* Pending Bit Array offset */
#define PCI_MSIX_PBA_BIR            0x00000007 /* BAR index */
#define PCI_MSIX_PBA_OFFSET         0xfffffff8 /* Offset into specified BAR */
#define PCI_MSIX_FLAGS_BIRMASK      PCI_MSIX_PBA_BIR /* deprecated */
#define PCI_CAP_MSIX_SIZEOF         12 /* size of MSIX registers */

/* MSI-X Table entry format */
#define PCI_MSIX_ENTRY_SIZE         16
#define PCI_MSIX_ENTRY_LOWER_ADDR   0
#define PCI_MSIX_ENTRY_UPPER_ADDR   4
#define PCI_MSIX_ENTRY_DATA         8
#define PCI_MSIX_ENTRY_VECTOR_CTRL  12
#define PCI_MSIX_ENTRY_CTRL_MASKBIT 1

#endif /* SRC_DRIVERS_PCI_PCI_REGS_H_ */
