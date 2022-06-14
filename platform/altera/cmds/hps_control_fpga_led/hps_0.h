#ifndef _ALTERA_HPS_0_H_
#define _ALTERA_HPS_0_H_

#define ALT_STM_OFST        0xFC000000
#define ALT_LWFPGASLVS_OFST 0xFF200000
#define LED_PIO_BASE        0x10040
#define HW_REGS_BASE        ALT_STM_OFST
#define HW_REGS_SPAN        0x1000
#define HW_REGS_MASK        (HW_REGS_SPAN - 1)

#endif /* _ALTERA_HPS_0_H_ */
