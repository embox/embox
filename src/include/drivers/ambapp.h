/**
 * @file
 * @brief Defines AMBA Plug & Play vendor and device IDs.
 *
 * @date 20.05.2010
 * @author Nikolay Korotky
 * @author Eldar Abusalimov
 */

#ifndef AMBAPP_H_
#define AMBAPP_H_

/* Vendor codes. */
#define AMBAPP_VENDOR_GAISLER       0x01   /**< Gaisler Research */
#define AMBAPP_VENDOR_PENDER        0x02   /**< PENDER */
#define AMBAPP_VENDOR_ESA           0x04   /**< European Space Agency */
#define AMBAPP_VENDOR_ASTRIUM       0x06   /**< ASTRIUM */
#define AMBAPP_VENDOR_OPENCHIP      0x07   /**< OpenChip */
#define AMBAPP_VENDOR_OPENCORES     0x08   /**< OpenCores */
#define AMBAPP_VENDOR_EONIC         0x0b   /**< Eonic BV */
#define AMBAPP_VENDOR_RADIONOR      0x0f   /**< Radionor Communications */
#define AMBAPP_VENDOR_GLEICHMANN    0x10   /**< Gleichmann Electronics */
#define AMBAPP_VENDOR_MENTA         0x11   /**< Menta */
#define AMBAPP_VENDOR_SUN           0x13   /**< Sun Microsystems */
#define AMBAPP_VENDOR_MOVIDIA       0x14   /**< MOVIDIA */
#define AMBAPP_VENDOR_ORBITA        0x17   /**< Orbita */
#define AMBAPP_VENDOR_SYNOPSYS      0x21   /**< Synopsys Inc. */
#define AMBAPP_VENDOR_NASA          0x22   /**< NASA */
#define AMBAPP_VENDOR_CAL           0xca   /**< CAL */
#define AMBAPP_VENDOR_EMBEDDIT      0xea   /**< Embedd.it */
#define AMBAPP_VENDOR_CETON         0xcb   /**< Ceton Corporation */
#define AMBAPP_VENDOR_ACTEL         0xac   /**< Actel Corporation */
#define AMBAPP_VENDOR_APPLECORE     0xae   /**< AppleCore */

#define AMBAPP_DEVICE_GAISLER_LEON2DSU     0x002 /**< Leon2 Debug Support Unit */
#define AMBAPP_DEVICE_GAISLER_LEON3        0x003 /**< Leon3 SPARC V8 Processor */
#define AMBAPP_DEVICE_GAISLER_LEON3DSU     0x004 /**< Leon3 Debug Support Unit */
#define AMBAPP_DEVICE_GAISLER_ETHAHB       0x005 /**< OC ethernet AHB interface */
#define AMBAPP_DEVICE_GAISLER_APBMST       0x006 /**< AHB/APB Bridge */
#define AMBAPP_DEVICE_GAISLER_AHBUART      0x007 /**< AHB Debug UART */
#define AMBAPP_DEVICE_GAISLER_SRCTRL       0x008 /**< Simple SRAM Ctrl. */
#define AMBAPP_DEVICE_GAISLER_SDCTRL       0x009 /**< PC133 SDRAM Ctrl. */
#define AMBAPP_DEVICE_GAISLER_SSRCTRL      0x00a /**< Synchronous SRAM Ctrl. */
#define AMBAPP_DEVICE_GAISLER_APBUART      0x00c /**< Generic UART */
#define AMBAPP_DEVICE_GAISLER_IRQMP        0x00d /**< Multi-processor Interrupt Ctrl. */
#define AMBAPP_DEVICE_GAISLER_AHBRAM       0x00e /**< Single-port AHB SRAM module */
#define AMBAPP_DEVICE_GAISLER_AHBDPRAM     0x00f /**< Dual-port AHB SRAM module */
#define AMBAPP_DEVICE_GAISLER_GPTIMER      0x011 /**< Modular Timer Unit */
#define AMBAPP_DEVICE_GAISLER_PCITRG       0x012 /**< Simple 32-bit PCI Target */
#define AMBAPP_DEVICE_GAISLER_PCISBRG      0x013 /**< Simple 32-bit PCI Bridge */
#define AMBAPP_DEVICE_GAISLER_PCIFBRG      0x014 /**< Fast 32-bit PCI Bridge */
#define AMBAPP_DEVICE_GAISLER_PCITRACE     0x015 /**< 32-bit PCI Trace Buffer */
#define AMBAPP_DEVICE_GAISLER_DMACTRL      0x016 /**< AMBA DMA Ctrl. */
#define AMBAPP_DEVICE_GAISLER_AHBTRACE     0x017 /**< AMBA Trace Buffer */
#define AMBAPP_DEVICE_GAISLER_DSUCTRL      0x018 /**< DSU/ETH Ctrl. */
#define AMBAPP_DEVICE_GAISLER_CANAHB       0x019 /**< OC CAN AHB interface */
#define AMBAPP_DEVICE_GAISLER_GPIO         0x01a /**< General Purpose I/O port */
#define AMBAPP_DEVICE_GAISLER_AHBROM       0x01b /**< Generic AHB ROM */
#define AMBAPP_DEVICE_GAISLER_AHBJTAG      0x01c /**< JTAG Debug Link */
#define AMBAPP_DEVICE_GAISLER_ETHMAC       0x01d /**< GR Ethernet MAC */
#define AMBAPP_DEVICE_GAISLER_SWNODE       0x01e /**< SpaceWire Node Interface */
#define AMBAPP_DEVICE_GAISLER_SPW          0x01f /**< SpaceWire Serial Link */
#define AMBAPP_DEVICE_GAISLER_AHB2AHB      0x020 /**< AHB-to-AHB Bridge */
#define AMBAPP_DEVICE_GAISLER_USBDC        0x021 /**< GR USB 2.0 Device Ctrl. */
#define AMBAPP_DEVICE_GAISLER_USB_DCL      0x022 /**< USB Debug Communication Link */
#define AMBAPP_DEVICE_GAISLER_DDRMP        0x023 /**< Multi-port DDR Ctrl. */
#define AMBAPP_DEVICE_GAISLER_ATACTRL      0x024 /**< ATA Ctrl. */
#define AMBAPP_DEVICE_GAISLER_DDRSP        0x025 /**< Single-port DDR266 Ctrl. */
#define AMBAPP_DEVICE_GAISLER_EHCI         0x026 /**< USB Enhanced Host Ctrl. */
#define AMBAPP_DEVICE_GAISLER_UHCI         0x027 /**< USB Universal Host Ctrl. */
#define AMBAPP_DEVICE_GAISLER_I2CMST       0x028 /**< AMBA Wrapper for OC I2C-master */
#define AMBAPP_DEVICE_GAISLER_SPW2         0x029 /**< GRSPW2 SpaceWire Serial Link */
#define AMBAPP_DEVICE_GAISLER_AHBDMA       0x02a /**< AHBDMA */
#define AMBAPP_DEVICE_GAISLER_NUHOSP3      0x02b /**< Nuhorizons Spartan3 IO I/F */
#define AMBAPP_DEVICE_GAISLER_CLKGATE      0x02c /**< Clock gating unit */
#define AMBAPP_DEVICE_GAISLER_SPICTRL      0x02d /**< SPI Ctrl. */
#define AMBAPP_DEVICE_GAISLER_DDR2SP       0x02e /**< Single-port DDR2 Ctrl. */
#define AMBAPP_DEVICE_GAISLER_SLINK        0x02f /**< SLINK Master */
#define AMBAPP_DEVICE_GAISLER_GRTM         0x030 /**< CCSDS Telemetry Encoder */
#define AMBAPP_DEVICE_GAISLER_GRTC         0x031 /**< CCSDS Telecommand Decoder */
#define AMBAPP_DEVICE_GAISLER_GRPW         0x032 /**< PacketWire to AMBA AHB I/F */
#define AMBAPP_DEVICE_GAISLER_GRCTM        0x033 /**< CCSDS Time Manager */
#define AMBAPP_DEVICE_GAISLER_GRHCAN       0x034 /**< ESA HurriCANe CAN with DMA */
#define AMBAPP_DEVICE_GAISLER_GRFIFO       0x035 /**< FIFO Ctrl. */
#define AMBAPP_DEVICE_GAISLER_GRADCDAC     0x036 /**< ADC / DAC Interface */
#define AMBAPP_DEVICE_GAISLER_GRPULSE      0x037 /**< General Purpose I/O with Pulses */
#define AMBAPP_DEVICE_GAISLER_GRTIMER      0x038 /**< Timer Unit with Latches */
#define AMBAPP_DEVICE_GAISLER_AHB2PP       0x039 /**< AMBA AHB to Packet Parallel I/F */
#define AMBAPP_DEVICE_GAISLER_GRVERSION    0x03a /**< Version and Revision Register */
#define AMBAPP_DEVICE_GAISLER_APB2PW       0x03b /**< PacketWire Transmit Interface */
#define AMBAPP_DEVICE_GAISLER_PW2APB       0x03c /**< PacketWire Receive Interface */
#define AMBAPP_DEVICE_GAISLER_GRCAN        0x03d /**< CAN Ctrl. with DMA */
#define AMBAPP_DEVICE_GAISLER_I2CSLV       0x03e /**< I2C Slave */
#define AMBAPP_DEVICE_GAISLER_U16550       0x03f /**< Simple 16550 UART */
#define AMBAPP_DEVICE_GAISLER_AHBMST_EM    0x040 /**< AMBA Master Emulator */
#define AMBAPP_DEVICE_GAISLER_AHBSLV_EM    0x041 /**< AMBA Slave Emulator */
#define AMBAPP_DEVICE_GAISLER_GRTESTMOD    0x042 /**< Test report module */
#define AMBAPP_DEVICE_GAISLER_ASCS         0x043 /**< ASCS Master */
#define AMBAPP_DEVICE_GAISLER_IPMVBCTRL    0x044 /**< IPM-bus/MVBC memory Ctrl. */
#define AMBAPP_DEVICE_GAISLER_SPIMCTRL     0x045 /**< SPI Memory Ctrl. */
#define AMBAPP_DEVICE_GAISLER_LEON4        0x048 /**< Leon4 SPARC V8 Processor */
#define AMBAPP_DEVICE_GAISLER_LEON4DSU     0x049 /**< Leon4 Debug Support Unit */
#define AMBAPP_DEVICE_GAISLER_PWM          0x04a /**< PWM generator */
#define AMBAPP_DEVICE_GAISLER_L2CACHE      0x04b /**< L2-Cache Ctrl. */
#define AMBAPP_DEVICE_GAISLER_SDCTRL64     0x04c /**< SDCTRL64 */
#define AMBAPP_DEVICE_GAISLER_FTAHBRAM     0x050 /**< Generic FT AHB SRAM module */
#define AMBAPP_DEVICE_GAISLER_FTSRCTRL     0x051 /**< Simple FT SRAM Ctrl. */
#define AMBAPP_DEVICE_GAISLER_AHBSTAT      0x052 /**< AHB Status Register */
#define AMBAPP_DEVICE_GAISLER_LEON3FT      0x053 /**< Leon3-FT SPARC V8 Processor */
#define AMBAPP_DEVICE_GAISLER_FTMCTRL      0x054 /**< Memory Ctrl. with EDAC */
#define AMBAPP_DEVICE_GAISLER_FTSDCTRL     0x055 /**< FT PC133 SDRAM Ctrl. */
#define AMBAPP_DEVICE_GAISLER_FTSRCTRL8    0x056 /**< FT 8-bit SRAM/16-bit IO Ctrl */
#define AMBAPP_DEVICE_GAISLER_APBPS2       0x060 /**< PS2 interface */
#define AMBAPP_DEVICE_GAISLER_VGACTRL      0x061 /**< VGA Ctrl. */
#define AMBAPP_DEVICE_GAISLER_LOGAN        0x062 /**< On chip Logic Analyzer */
#define AMBAPP_DEVICE_GAISLER_SVGACTRL     0x063 /**< SVGA frame buffer */
#define AMBAPP_DEVICE_GAISLER_T1AHB        0x064 /**< Niagara T1 PCX/AHB bridge */
#define AMBAPP_DEVICE_GAISLER_MP7WRAP      0x065 /**< CoreMP7 wrapper */
#define AMBAPP_DEVICE_GAISLER_GRSYSMON     0x066 /**< AMBA wrapper for System Monitor */
#define AMBAPP_DEVICE_GAISLER_GRACECTRL    0x067 /**< System ACE I/F Ctrl. */
#define AMBAPP_DEVICE_GAISLER_ATAHBSLV     0x068 /**< AMBA Test Framework AHB Slave */
#define AMBAPP_DEVICE_GAISLER_ATAHBMST     0x069 /**< AMBA Test Framework AHB Master */
#define AMBAPP_DEVICE_GAISLER_ATAPBSLV     0x06a /**< AMBA Test Framework APB Slave */
#define AMBAPP_DEVICE_GAISLER_B1553BC      0x070 /**< AMBA Wrapper for Core1553BBC */
#define AMBAPP_DEVICE_GAISLER_B1553RT      0x071 /**< AMBA Wrapper for Core1553BRT */
#define AMBAPP_DEVICE_GAISLER_B1553BRM     0x072 /**< AMBA Wrapper for Core1553BRM */
#define AMBAPP_DEVICE_GAISLER_AES          0x073 /**< Advanced Encryption Standard */
#define AMBAPP_DEVICE_GAISLER_ECC          0x074 /**< Elliptic Curve Cryptography */
#define AMBAPP_DEVICE_GAISLER_PCIF         0x075 /**< AMBA Wrapper for CorePCIF */
#define AMBAPP_DEVICE_GAISLER_CLKMOD       0x076 /**< CPU Clock Switching Ctrl module */
#define AMBAPP_DEVICE_GAISLER_HAPSTRAK     0x077 /**< HAPS HapsTrak I/O Port */
#define AMBAPP_DEVICE_GAISLER_TEST_1X2     0x078 /**< HAPS TEST_1x2 interface */
#define AMBAPP_DEVICE_GAISLER_WILD2AHB     0x079 /**< WildCard CardBus interface */
#define AMBAPP_DEVICE_GAISLER_BIO1         0x07a /**< Basic I/O board BIO1 */
#define AMBAPP_DEVICE_GAISLER_SATCAN       0x080 /**< SatCAN Ctrl. */
#define AMBAPP_DEVICE_GAISLER_CANMUX       0x081 /**< CAN Bus multiplexer */
#define AMBAPP_DEVICE_GAISLER_GRTMRX       0x082 /**< CCSDS Telemetry Receiver */
#define AMBAPP_DEVICE_GAISLER_GRTCTX       0x083 /**< CCSDS Telecommand Transmitter */
#define AMBAPP_DEVICE_GAISLER_GRTMDESC     0x084 /**< CCSDS Telemetry Descriptor */
#define AMBAPP_DEVICE_GAISLER_GRTMVC       0x085 /**< CCSDS Telemetry VC Generator */
#define AMBAPP_DEVICE_GAISLER_GEFFE        0x086 /**< Geffe Generator */
#define AMBAPP_DEVICE_GAISLER_GPREG        0x087 /**< GPREG */
#define AMBAPP_DEVICE_GAISLER_GRTMPAHB     0x088 /**< CCSDS Telemetry VC AHB Input */

/* European Space Agency device ID defines */
#define AMBAPP_DEVICE_ESA_LEON2            0x002 /**< Leon2 SPARC V8 Processor */
#define AMBAPP_DEVICE_ESA_LEON2APB         0x003 /**< Leon2 Peripheral Bus */
#define AMBAPP_DEVICE_ESA_IRQ              0x005 /**< Leon2 Interrupt Ctrl. */
#define AMBAPP_DEVICE_ESA_TIMER            0x006 /**< Leon2 Timer */
#define AMBAPP_DEVICE_ESA_UART             0x007 /**< Leon2 UART */
#define AMBAPP_DEVICE_ESA_CFG              0x008 /**< Leon2 Configuration Register */
#define AMBAPP_DEVICE_ESA_IO               0x009 /**< Leon2 Input/Output */
#define AMBAPP_DEVICE_ESA_MCTRL            0x00f /**< Leon2 Memory Ctrl. */
#define AMBAPP_DEVICE_ESA_PCIARB           0x010 /**< PCI Arbiter */
#define AMBAPP_DEVICE_ESA_HURRICANE        0x011 /**< HurriCANe/HurryAMBA CAN Ctrl */
#define AMBAPP_DEVICE_ESA_SPW_RMAP         0x012 /**< UoD/Saab SpaceWire/RMAP link */
#define AMBAPP_DEVICE_ESA_AHBUART          0x013 /**< Leon2 AHB Debug UART */
#define AMBAPP_DEVICE_ESA_SPWA             0x014 /**< ESA/ASTRIUM SpaceWire link */
#define AMBAPP_DEVICE_ESA_BOSCHCAN         0x015 /**< SSC/BOSCH CAN Ctrl */
#define AMBAPP_DEVICE_ESA_IRQ2             0x016 /**< Leon2 Secondary Irq Ctrl. */
#define AMBAPP_DEVICE_ESA_AHBSTAT          0x017 /**< Leon2 AHB Status Register */
#define AMBAPP_DEVICE_ESA_WPROT            0x018 /**< Leon2 Write Protection */
#define AMBAPP_DEVICE_ESA_WPROT2           0x019 /**< Leon2 Extended Write Protection */
#define AMBAPP_DEVICE_ESA_PDEC3AMBA        0x020 /**< ESA CCSDS PDEC3AMBA TC Decoder */
#define AMBAPP_DEVICE_ESA_PTME3AMBA        0x021 /**< ESA CCSDS PTME3AMBA TM Encoder */

/* OpenChip device ID defines */
#define AMBAPP_DEVICE_OPENCHIP_APBGPIO     0x001 /**< APB General Purpose IO */
#define AMBAPP_DEVICE_OPENCHIP_APBI2C      0x002 /**< APB I2C Interface */
#define AMBAPP_DEVICE_OPENCHIP_APBSPI      0x003 /**< APB SPI Interface */
#define AMBAPP_DEVICE_OPENCHIP_APBCHARLCD  0x004 /**< APB Character LCD */
#define AMBAPP_DEVICE_OPENCHIP_APBPWM      0x005 /**< APB PWM */
#define AMBAPP_DEVICE_OPENCHIP_APBPS2      0x006 /**< APB PS/2 Interface */
#define AMBAPP_DEVICE_OPENCHIP_APBMMCSD    0x007 /**< APB MMC/SD Card Interface */
#define AMBAPP_DEVICE_OPENCHIP_APBNAND     0x008 /**< APB NAND(SmartMedia) Interface */
#define AMBAPP_DEVICE_OPENCHIP_APBLPC      0x009 /**< APB LPC Interface */
#define AMBAPP_DEVICE_OPENCHIP_APBCF       0x00a /**< APB CompactFlash (IDE) */
#define AMBAPP_DEVICE_OPENCHIP_APBSYSACE   0x00b /**< APB SystemACE Interface */
#define AMBAPP_DEVICE_OPENCHIP_APB1WIRE    0x00c /**< APB 1-Wire Interface */
#define AMBAPP_DEVICE_OPENCHIP_APBJTAG     0x00d /**< APB JTAG TAP Master */
#define AMBAPP_DEVICE_OPENCHIP_APBSUI      0x00e /**< APB Simple User Interface */

/* Gleichmann Electronics device ID defines */
#define AMBAPP_DEVICE_GLEICHMANN_CUSTOM    0x001 /**< Custom device */
#define AMBAPP_DEVICE_GLEICHMANN_GEOLCD01  0x002 /**< GEOLCD01 graphics system */
#define AMBAPP_DEVICE_GLEICHMANN_DAC       0x003 /**< Sigma delta DAC */
#define AMBAPP_DEVICE_GLEICHMANN_HPI       0x004 /**< AHB-to-HPI bridge */
#define AMBAPP_DEVICE_GLEICHMANN_SPI       0x005 /**< SPI master */
#define AMBAPP_DEVICE_GLEICHMANN_HIFC      0x006 /**< Human interface Ctrl. */
#define AMBAPP_DEVICE_GLEICHMANN_ADCDAC    0x007 /**< Sigma delta ADC/DAC */
#define AMBAPP_DEVICE_GLEICHMANN_SPIOC     0x008 /**< SPIOC */
#define AMBAPP_DEVICE_GLEICHMANN_AC97      0x009 /**< AC97 */

/* Sun Microsystems device ID defines */
#define AMBAPP_DEVICE_SUN_T1               0x001 /**< Niagara T1 SPARC V9 Processor */
#define AMBAPP_DEVICE_SUN_S1               0x011 /**< Niagara S1 SPARC V9 Processor */

/* Orbita device ID defines */
#define AMBAPP_DEVICE_ORBITA_1553B         0x001 /**< MIL-STD-1553B Ctrl. */
#define AMBAPP_DEVICE_ORBITA_429           0x002 /**< 429 Interface */
#define AMBAPP_DEVICE_ORBITA_SPI           0x003 /**< SPI Interface */
#define AMBAPP_DEVICE_ORBITA_I2C           0x004 /**< I2C Interface */
#define AMBAPP_DEVICE_ORBITA_SMARTCARD     0x064 /**< Smart Card Reader */
#define AMBAPP_DEVICE_ORBITA_SDCARD        0x065 /**< SD Card Reader */
#define AMBAPP_DEVICE_ORBITA_UART16550     0x066 /**< 16550 UART */
#define AMBAPP_DEVICE_ORBITA_CRYPTO        0x067 /**< Crypto Engine */
#define AMBAPP_DEVICE_ORBITA_SYSIF         0x068 /**< System Interface */
#define AMBAPP_DEVICE_ORBITA_PIO           0x069 /**< Programmable IO module */
#define AMBAPP_DEVICE_ORBITA_RTC           0x0c8 /**< Real-Time Clock */
#define AMBAPP_DEVICE_ORBITA_COLORLCD      0x12c /**< Color LCD Ctrl. */
#define AMBAPP_DEVICE_ORBITA_PCI           0x190 /**< PCI Module */
#define AMBAPP_DEVICE_ORBITA_DSP           0x1f4 /**< DPS Co-Processor */
#define AMBAPP_DEVICE_ORBITA_USBHOST       0x258 /**< USB Host */
#define AMBAPP_DEVICE_ORBITA_USBDEV        0x2bc /**< USB Device */

/* NASA device ID defines */
#define AMBAPP_DEVICE_NASA_EP32            0x001 /**< EP32 Forth processor */

/* CAL device ID defines */
#define AMBAPP_DEVICE_CAL_DDRCTRL          0x188 /**< DDRCTRL */

/* Actel Corporation device ID defines */
#define AMBAPP_DEVICE_ACTEL_COREMP7        0x001 /**< CoreMP7 Processor */

/* AppleCore device ID defines */
#define AMBAPP_DEVICE_APPLECORE_UTLEON3    0x001 /**< AppleCore uT-LEON3 Processor */
#define AMBAPP_DEVICE_APPLECORE_UTLEON3DSU 0x002 /**< AppleCore uT-LEON3 DSU */

/* Opencores device id's */
#define AMBAPP_DEVICE_OPENCORES_PCIBR      0x004 /**< PCI Bridge */
#define AMBAPP_DEVICE_OPENCORES_ETHMAC     0x005 /**< Ethernet MAC */

#endif /* AMBAPP_H_ */

