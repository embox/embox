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
	struct pin_description  pin_desc;
	uintptr_t               base_addr;
	char                   *clk_name;
	int                     channel;
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

#endif /* DRIVERS_PWM_NIIET_NIIET_PWM_H_ */
