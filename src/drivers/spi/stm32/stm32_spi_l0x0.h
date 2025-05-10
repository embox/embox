/**
 * @file stm32_spi_l0x0.c
 * @brief
 * @author Vadim Deryabkin <Vadimatorikda@gmail.com>
 * @version
 * @date 20.02.2021
 */

#ifndef SRC_DRIVERS_SPI_STM32_SPI_L0X0_H_
#define SRC_DRIVERS_SPI_STM32_SPI_L0X0_H_

#include <stdint.h>
#include <stdbool.h>

#include <hal/reg.h>
#include <framework/mod/options.h>

typedef struct {
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t CRCPR;
	volatile uint32_t RXCRCR;
	volatile uint32_t TXCRCR;
} spi_struct; // Doc: RM0451, 705/774.

#define SPI1      ((spi_struct *)          0x40013000)

#define REG_RCC        OPTION_GET(NUMBER,reg_rcc_address)
#define REG_RCC_APB2ENR (REG_RCC + 0x34) // Doc: DocID031151 Rev 1, RM0451, 172/774.

struct stm32_spi {
	bool is_master;
};

static void set_spi1_pwr() {
	REG32_ORIN(REG_RCC_APB2ENR, (1 << 12)); // Doc: DocID031151 Rev 1, RM0451, 172/774.
}

#endif /* SRC_DRIVERS_SPI_STM32_SPI_L0X0_H_ */
