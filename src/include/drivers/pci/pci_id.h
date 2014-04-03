/**
 * @file
 * @brief PCI Vendor and Device IDs
 *
 * @date 20.01.11
 * @author NIkolay Korotky
 */

#ifndef PCI_ID_H_
#define PCI_ID_H_

/* Vendor codes */
#define PCI_VENDOR_ID_CIRRUS              0x1013
#define PCI_VENDOR_ID_REALTEK             0x10ec
#define PCI_VENDOR_ID_RICOH               0x1180
#define PCI_VENDOR_ID_ATHEROS             0x168c
#define PCI_VENDOR_ID_RDC                 0x17f3
#define PCI_VENDOR_ID_INTEL               0x8086
#define PCI_VENDOR_ID_BOCHS               0x1234
#define PCI_VENDOR_ID_APPLE               0x106b
#define PCI_VENDOR_ID_PLX                 0x10B5
#define PCI_VENDOR_ID_XILINX              0x10EE
#define PCI_VENDOR_ID_VIRTIO              0x1AF4

/* Cirrus device id's */
#define PCI_DEV_ID_CIRRUS_5446            0x00b8

/* Realtek device id's */
#define PCI_DEV_ID_REALTEK_8029           0x8029
#define PCI_DEV_ID_REALTEK_8139           0x8139

/* RDC device id's */
#define PCI_DEV_ID_RDC_R6020              0x6020
#define PCI_DEV_ID_RDC_R6030              0x6030
#define PCI_DEV_ID_RDC_R6040              0x6040
#define PCI_DEV_ID_RDC_R6060              0x6060
#define PCI_DEV_ID_RDC_R6061              0x6061

/* Ricoh device id's */
#define PCI_DEV_ID_RICOH_RL5C476          0x0476
#define PCI_DEV_ID_RICOH_R5C592           0x0592
#define PCI_DEV_ID_RICOH_R5C822           0x0822

/* Atheros device id's */
#define PCI_DEV_ID_ATHEROS_AR9283         0x002a

/* Intel device id's */
#define PCI_DEV_ID_INTEL_82540EM          0x100e
#define PCI_DEV_ID_INTEL_82441            0x1237
#define PCI_DEV_ID_INTEL_82567V3          0x1501
#define PCI_DEV_ID_INTEL_82801BAM_PCI     0x2448
#define PCI_DEV_ID_INTEL_82801HBM_LPC     0x2815
#define PCI_DEV_ID_INTEL_82801HBM_SATA    0x2828
#define PCI_DEV_ID_INTEL_82801HB_USB1     0x2830
#define PCI_DEV_ID_INTEL_82801HB_USB2     0x2831
#define PCI_DEV_ID_INTEL_82801HB_USB3     0x2832
#define PCI_DEV_ID_INTEL_82801HB_USB4     0x2834
#define PCI_DEV_ID_INTEL_82801HB_USB5     0x2835
#define PCI_DEV_ID_INTEL_82801HB_PCIE1    0x283f
#define PCI_DEV_ID_INTEL_82801HB_PCIE2    0x2841
#define PCI_DEV_ID_INTEL_82801HB_PCIE3    0x2843
#define PCI_DEV_ID_INTEL_82801HB_PCIE4    0x2845
#define PCI_DEV_ID_INTEL_82801HB_HD_AUDIO 0x284b
#define PCI_DEV_ID_INTEL_82801HBM_IDE     0x2850
#define PCI_DEV_ID_INTEL_MCH_GM965        0x2a00
#define PCI_DEV_ID_INTEL_IGC_GM965        0x2a02
#define PCI_DEV_ID_INTEL_82371SB_ISA      0x7000
#define PCI_DEV_ID_INTEL_82371SB_IDE      0x7010

/* Bochs device id's */
#define PCI_DEV_ID_BOCHS_VGA              0x1111

/* Apple device id's */
#define PCI_DEV_ID_APPLE_OHCI_HOST	  0x003f

#define PCI_DEV_ID_PLX_PEX8619            0x8619

#define PCI_DEV_ID_XILINX_SPARTAN6        0xA125

/* VirtIO device id's */
#define PCI_DEV_ID_VIRTIO_NET             0x1000

#endif /* PCI_ID_H_ */
