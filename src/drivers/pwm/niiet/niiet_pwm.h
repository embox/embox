/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#ifndef DRIVERS_PWM_NIIET_NIIET_PWM_H_
#define DRIVERS_PWM_NIIET_NIIET_PWM_H_

#include <stdint.h>

#include <drivers/pwm.h> /* only for struct pin_description  */

struct niiet_pwm_priv {
	const struct pin_description *pin_desc;
	const uintptr_t               base_addr;
	const char                   *clk_name;
	const int                     channel;
};

struct niiet_capcom_reg {
	volatile uint32_t CAPCOM_CTRL;  /*!< Capture / Compare Control register */
	volatile uint32_t CAPCOM_VAL;   /*!< VAL : type used for word access */
};

struct niiet_tmr16_regs {
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




#endif /* DRIVERS_PWM_NIIET_NIIET_PWM_H_ */
