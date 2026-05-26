/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#ifndef DRIVERS_PWM_NIIET_NIIET_PWM_REGS_H_
#define DRIVERS_PWM_NIIET_NIIET_PWM_REGS_H_

#include <stdint.h>

#include "niiet_pwm_priv.h"

#define VG1T_VERSION   OPTION_GET(NUMBER, vg1t_version)

#if VG1T_VERSION == 0

struct niiet_capcom_reg {
	volatile uint32_t CAPCOM_CTRL;  /*!< Capture / Compare Control register */
	volatile uint32_t CAPCOM_VAL;   /*!< VAL : type used for word access */
};

struct niiet_tmr_regs {
	volatile uint32_t CTRL;           /*!< Timer control register */
	volatile uint32_t COUNT;          /*!< Current value timer register */
	volatile uint32_t IM;             /*!< Interrupt mask register */
	volatile uint32_t RIS;            /*!< Raw Interrupt Status register */
	volatile uint32_t MIS;            /*!< Masked Interrupt Status register */
	volatile uint32_t IC;             /*!< Clear Interrupt Status register */
	struct niiet_capcom_reg CAPCOM[4];
	volatile uint32_t DMA_IM;         /*!< DMA request mask register */
	volatile uint32_t ADC_IM;         /*!< ADC request mask register */
};


#else /* VG1T_VERSION != 0 */

struct niiet_capcom_reg {
	volatile uint32_t CAPCOM_CTRL;  /*!< Capture / Compare Control register */
	volatile uint32_t CAPCOM_VAL;   /*!< VAL : type used for word access */
	volatile uint32_t CAPCOM_VAL1;  /*!< VAL : type used for word access */
	uint32_t Reserved0;
} ;

struct niiet_tmr_regs {
	volatile uint32_t CTRL;           /*!< 0x00 Timer control register */
	volatile uint32_t COUNT;          /*!< 0x04 Current value timer register */
	volatile uint32_t CLKDIV;         /*!< 0x08 Divider */
	volatile uint32_t PERIOD;         /*!< 0x0C Period */
	volatile uint32_t IM;             /*!< 0x10 Interrupt mask register */
	volatile uint32_t RIS;            /*!< 0x14 Raw Interrupt Status register */
	volatile uint32_t MIS;            /*!< 0x18 Masked Interrupt Status register */
	volatile uint32_t IC;             /*!< 0x1C Clear Interrupt Status register */
	volatile uint32_t DMA_TXIM;       /*!< 0x20 DAM TX mask */
	volatile uint32_t DMA_RXIM;       /*!< 0x24 DAM RX mask */
	volatile uint32_t EXTEVT_IM;      /*!< 0x28 Mask of ext irq */
	uint32_t Reserved0[53];
	struct niiet_capcom_reg CAPCOM[4]; /* 0x100 + 0x10 * n*/
};

#endif

#define TMR_CTRL_DIV_MASK  0x3
#define TMR_CTRL_DIV_SHIFT 6
#define TMR_CTRL_DIV(div)  \
					((div & TMR_CTRL_DIV_MASK) << TMR_CTRL_DIV_SHIFT)
#define TMR_CTRL_DIV_1     0
#define TMR_CTRL_DIV_2     1
#define TMR_CTRL_DIV_4     2
#define TMR_CTRL_DIV_8     3

#define TMR_CTRL_MODE_SHIFT 4
#define TMR_CTRL_MODE_MASK  0x3
#define TMR_CTRL_MODE(mode)  \
					((mode & TMR_CTRL_MODE_MASK) << TMR_CTRL_MODE_SHIFT)
#define TMR_CTRL_MODE_STOP  0
#define TMR_CTRL_MODE_UP    1

#define TMR_CTRL_OUTEN_SHIFT 9
#define TMR_CTRL_OUTEN_MASK  0xF
#define TMR_CTRL_OUTEN(chan)  \
					(((1 << chan) & TMR_CTRL_OUTEN_MASK) << TMR_CTRL_OUTEN_SHIFT)

#define TMR_CAPCOM_CTRL_SCCI (1 << 10) /* Synchronized Capture/Compare Input */
#define TMR_CAPCOM_CTRL_CAP  (1 << 8)  /* Capture Mode Enable */
#define TMR_CAPCOM_CTRL_CCI  (1 << 3)  /* Capture/Compare Input */
#define TMR_CAPCOM_CTRL_OUT  (1 << 2)  /* Output */
#define TMR_CAPCOM_CTRL_OVF  (1 << 1)  /* Capture Overflow */

#define TMR_CAPCOM_CTRL_CAPMODE_MASK  0x3
#define TMR_CAPCOM_CTRL_CAPMODE_SHIFT 14
#define TMR_CAPCOM_CTRL_CAPMODE(mode)  \
				((mode & TMR_CAPCOM_CTRL_CAPMODE_MASK) \
										<< TMR_CAPCOM_CTRL_CAPMODE_SHIFT)

#define TMR_CAPCOM_CTRL_CCISEL       /* Capture/Compare Input Select */
#define TMR_CAPCOM_CTRL_CCISEL_MASK  0x3
#define TMR_CAPCOM_CTRL_CCISEL_SHIFT 12

#define TMR_CAPCOM_CTRL_OUTMODE_MASK  0x7
#define TMR_CAPCOM_CTRL_OUTMODE_SHIFT 5
#define TMR_CAPCOM_CTRL_OUTMODE(mode) \
				((mode & TMR_CAPCOM_CTRL_OUTMODE_MASK) \
										<< TMR_CAPCOM_CTRL_OUTMODE_SHIFT)
#define TMR_CAPCOM_CTRL_OUTMODE_SET_RESET   3
#define TMR_CAPCOM_CTRL_OUTMODE_RESET_SET   7




#endif /* DRIVERS_PWM_NIIET_NIIET_PWM_REGS_H_ */
