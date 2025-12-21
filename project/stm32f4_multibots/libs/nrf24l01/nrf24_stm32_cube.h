/**
 * @file
 * @brief  Adoptaion of NRF24 library to STM32F4 Cube
 *
 * @date   27.06.18
 * @author Alexander Kalmuk
 */

#ifndef NRF24_STM32_CUBE
#define NRF24_STM32_CUBE

#include <stdint.h>

extern int nrf24_hw_init(void);
extern void nrf24_ce_digitalWrite(uint8_t state);
extern void nrf24_csn_digitalWrite(uint8_t state);

/* send and receive multiple bytes over SPI */
extern void nrf24_spi_transfer(uint8_t *tx, uint8_t *rx, uint8_t len);

/* send multiple bytes over SPI */
extern void nrf24_spi_transmit(uint8_t *tx, uint8_t len);

/* receive multiple bytes over SPI */
extern void nrf24_spi_receive(uint8_t *rx, uint8_t len);

#endif /* NRF24_STM32_CUBE */
