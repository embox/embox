/**
 * @file
 *
 * @brief AMBA Plag & Play device and vendor ID
 *
 * @date 20.05.2010
 * @author Nikolay Korotky
 */

#ifndef AMBAPP_H_
#define AMBAPP_H_

/* Vendor codes */
#define CONFIG_VENDOR_ID_GAISLER              0x001
#define CONFIG_VENDOR_ID_ESA                  0x004

/* Gaisler Research device id's */
#define CONFIG_DEV_ID_GAISLER_LEON3_CPU       0x003
#define CONFIG_DEV_ID_GAISLER_JTAG            0x01C
#define CONFIG_DEV_ID_GAISLER_DDR2_CTRLR      0x02E
#define CONFIG_DEV_ID_GAISLER_AHB_ABP_BRIDGE  0x006
#define CONFIG_DEV_ID_GAISLER_LEON3_DSU       0x004
#define CONFIG_DEV_ID_GAISLER_TIMER           0x011
#define CONFIG_DEV_ID_GAISLER_INTERRUPT_UNIT  0x00D
#define CONFIG_DEV_ID_GAISLER_UART            0x00C

/* European Space Agency device id's */
#define CONFIG_DEV_ID_ESA_LEON2_MEM_CTRLR     0x00F

#endif /* AMBAPP_H_ */

