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
#define PCI_VENDOR_ID_CIRRUS                0x1013
#define PCI_VENDOR_ID_REALTEK               0x10ec
#define PCI_VENDOR_ID_RICOH                 0x1180
#define PCI_VENDOR_ID_ATHEROS               0x168c
#define PCI_VENDOR_ID_RDC                   0x17f3
#define PCI_VENDOR_ID_INTEL                 0x8086
#define PCI_VENDOR_ID_BOCHS                 0x1234
#define PCI_VENDOR_ID_APPLE                 0x106b
#define PCI_VENDOR_ID_PLX                   0x10B5
#define PCI_VENDOR_ID_XILINX                0x10EE
#define PCI_VENDOR_ID_VIRTIO                0x1AF4
#define PCI_VENDOR_ID_SMI                   0x126f
#define PCI_VENDOR_ID_RED_HAT               0x1b36

#define PCI_VENDOR_ID_MCST_PCIE_BRIDGE      0xe3e3
#define PCI_VENDOR_ID_MCST_TMP              0x1FFF

/* Cirrus device id's */
#define PCI_DEV_ID_CIRRUS_5446              0x00b8
#define PCI_DEV_ID_CIRRUS_CS4281            0x6005

/* Realtek device id's */
#define PCI_DEV_ID_REALTEK_8029             0x8029
#define PCI_DEV_ID_REALTEK_8139             0x8139

/* RDC device id's */
#define PCI_DEV_ID_RDC_R6020                0x6020
#define PCI_DEV_ID_RDC_R6030                0x6030
#define PCI_DEV_ID_RDC_R6040                0x6040
#define PCI_DEV_ID_RDC_R6060                0x6060
#define PCI_DEV_ID_RDC_R6061                0x6061

/* Ricoh device id's */
#define PCI_DEV_ID_RICOH_RL5C476            0x0476
#define PCI_DEV_ID_RICOH_R5C592             0x0592
#define PCI_DEV_ID_RICOH_R5C822             0x0822

/* Atheros device id's */
#define PCI_DEV_ID_ATHEROS_AR9283           0x002a

/* Intel device id's */
#define PCI_DEV_ID_INTEL_82540EM            0x100e
#define PCI_DEV_ID_INTEL_82441              0x1237
#define PCI_DEV_ID_INTEL_82567V3            0x1501
#define PCI_DEV_ID_INTEL_82801AA_AC97       0x2415
#define PCI_DEV_ID_INTEL_82801BAM_PCI       0x2448
#define PCI_DEV_ID_INTEL_82801HBM_LPC       0x2815
#define PCI_DEV_ID_INTEL_82801HBM_SATA      0x2828
#define PCI_DEV_ID_INTEL_82801HB_USB1       0x2830
#define PCI_DEV_ID_INTEL_82801HB_USB2       0x2831
#define PCI_DEV_ID_INTEL_82801HB_USB3       0x2832
#define PCI_DEV_ID_INTEL_82801HB_USB4       0x2834
#define PCI_DEV_ID_INTEL_82801HB_USB5       0x2835
#define PCI_DEV_ID_INTEL_82801HB_PCIE1      0x283f
#define PCI_DEV_ID_INTEL_82801HB_PCIE2      0x2841
#define PCI_DEV_ID_INTEL_82801HB_PCIE3      0x2843
#define PCI_DEV_ID_INTEL_82801HB_PCIE4      0x2845
#define PCI_DEV_ID_INTEL_82801HB_HD_AUDIO   0x284b
#define PCI_DEV_ID_INTEL_82801HBM_IDE       0x2850
#define PCI_DEV_ID_INTEL_MCH_GM965          0x2a00
#define PCI_DEV_ID_INTEL_IGC_GM965          0x2a02
#define PCI_DEV_ID_INTEL_82371SB_ISA        0x7000
#define PCI_DEV_ID_INTEL_82371SB_IDE        0x7010
#define PCI_DEV_ID_INTEL_82371SB_USB        0x7020
#define PCI_DEV_ID_INTEL_PIIX4_ACPI         0x7113
#define PCI_DEV_ID_INTEL_82801DB_USB_EHCI   0x24cd
#define PCI_DEV_ID_INTEL_82801I_USB_EHCI    0x293A

/* Bochs device id's */
#define PCI_DEV_ID_BOCHS_VGA                0x1111

/* Apple device id's */
#define PCI_DEV_ID_APPLE_OHCI_HOST          0x003f

#define PCI_DEV_ID_PLX_PEX8619              0x8619

#define PCI_DEV_ID_XILINX_SPARTAN6          0xA125

/* VirtIO device id's */
#define PCI_DEV_ID_VIRTIO_NET               0x1000

#define PCI_DEV_ID_LYNX_EXP                 0x0750
#define PCI_DEV_ID_LYNX_SE                  0x0718
#define PCI_DEV_ID_LYNX_EM                  0x0712
#define PCI_DEV_ID_LYNX_3DM                 0x0720
#define PCI_DEV_ID_VOYAGER                  0x0501

/* MCST device id's */
#define PCI_DEVICE_ID_MCST_SATA             0x4748
#define PCI_DEVICE_ID_MCST_ELBRUS_E1000     0x4D45
#define PCI_DEVICE_ID_MCST_IDE              0x4D49
#define PCI_DEVICE_ID_MCST_USB_OHCI         0x554F
#define PCI_DEVICE_ID_MCST_USB_EHCI         0x5545
#define PCI_DEVICE_ID_MCST_PCI_BRIDGE       0x8000
#define PCI_DEVICE_ID_MCST_PCIE_BRIDGE      0xabcd
#define PCI_DEVICE_ID_MCST_VIRT_PCI_BRIDGE  0xe3e3

#define PCI_DEVICE_ID_MCST_USB_OHCI_CTRL    0x8008
#define PCI_DEVICE_ID_MCST_VGA_MGA2         0x8009
#define PCI_DEVICE_ID_MCST_HD_AUDIO         0x800A
#define PCI_DEVICE_ID_MCST_IDE_CTRL         0x800B
#define PCI_DEVICE_ID_MCST_SPMC             0x800E
#define PCI_DEVICE_ID_MCST_PCIE_X4          0x8010
#define PCI_DEVICE_ID_MCST_PCIE_X16         0x8011
#define PCI_DEVICE_ID_MCST_GPIO_MPV         0x8014
#define PCI_DEVICE_ID_MCST_GIG_ETHERNET     0x8016
#define PCI_DEVICE_ID_MCST_VIRT_PCI_BRIDGE2 0x8017
#define PCI_DEVICE_ID_MCST_I2C_SPI          0x8018
#define PCI_DEVICE_ID_MCST_PARALLEL_SERIAL  0x8019
#define PCI_DEVICE_ID_MCST_HOST_BRIDGE      0x801D
#define PCI_DEVICE_ID_MCST_SATA_CTRL        0x801A
#define PCI_DEVICE_ID_MCST_USB_EHCI_CTRL    0x801E

/* Red Hat, Inc. */
#define PCI_DEVICE_ID_QEMU_PCIE_HOST_BRINGE 0x0008

#endif /* PCI_ID_H_ */
