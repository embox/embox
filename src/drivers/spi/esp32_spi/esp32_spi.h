/**
 * @file
 * @brief
 * @author Dmitrii Piliuk
 * @version
 * @date 28.10.2026
 */

#ifndef SRC_DRIVERS_SPI_ESP32_SPI_H_
#define SRC_DRIVERS_SPI_ESP32_SPI_H_

#include <stdint.h>

#include <drivers/spi_controller.h>

/* ESP32-C5 GP-SPI2. Offsets are relative to base_addr. */
#define ESP32_SPI_CMD          0x00
#define ESP32_SPI_CTRL         0x08
#define ESP32_SPI_CLOCK        0x0c
#define ESP32_SPI_USER         0x10
#define ESP32_SPI_USER1        0x14
#define ESP32_SPI_USER2        0x18
#define ESP32_SPI_MS_DLEN      0x1c
#define ESP32_SPI_MISC         0x20
#define ESP32_SPI_DMA_CONF     0x30
#define ESP32_SPI_DMA_INT_ENA  0x34
#define ESP32_SPI_DMA_INT_CLR  0x38
#define ESP32_SPI_DMA_INT_RAW  0x3c
#define ESP32_SPI_W(n)         (0x98 + 4 * (n))
#define ESP32_SPI_SLAVE        0xe0

#define ESP32_SPI_CMD_UPDATE   (1U << 23)
#define ESP32_SPI_CMD_USR      (1U << 24)
#define ESP32_SPI_USER_DOUTDIN (1U << 0)
#define ESP32_SPI_USER_CS_HOLD (1U << 6)
#define ESP32_SPI_USER_CS_SETUP (1U << 7)
#define ESP32_SPI_USER_MOSI    (1U << 27)
#define ESP32_SPI_USER_MISO    (1U << 28)

/* Idle levels for MOSI, MISO and the unused quad-SPI signals. */
#define ESP32_SPI_CTRL_Q_POL   (1U << 18)
#define ESP32_SPI_CTRL_D_POL   (1U << 19)
#define ESP32_SPI_CTRL_HOLD_POL (1U << 20)
#define ESP32_SPI_CTRL_WP_POL  (1U << 21)

#define ESP32_SPI_MISC_CS1_5_DIS (0x3eU)
#define ESP32_SPI_MISC_CS_KEEP_ACTIVE (1U << 30)

#define ESP32_SPI_DMA_RX_AFIFO_RST  (1U << 29)
#define ESP32_SPI_DMA_BUF_AFIFO_RST (1U << 30)
#define ESP32_SPI_INT_TRANS_DONE    (1U << 12)
#define ESP32_SPI_INT_RX_FULL       (1U << 17)
#define ESP32_SPI_INT_TX_EMPTY      (1U << 18)
#define ESP32_SPI_INT_ERRORS \
	(ESP32_SPI_INT_RX_FULL | ESP32_SPI_INT_TX_EMPTY)
#define ESP32_SPI_SLAVE_SOFT_RESET  (1U << 27)

/* Bounded polling per hardware operation (not a duration in time units). */
#define ESP32_SPI_POLL_LIMIT 1000000

#define ESP32_SPI_CLKCNT_H_SHIFT 6
#define ESP32_SPI_CLKCNT_N_SHIFT 12
#define ESP32_SPI_CLKDIV_PRE_SHIFT 18
#define ESP32_SPI_CLK_EDGE_SEL (1U << 30)

/* ESP32-C5 PCR register offsets and fields for SPI2. */
#define ESP32_SPI_PCR_CONF       0xc4
#define ESP32_SPI_PCR_CLKM_CONF  0xc8
#define ESP32_SPI_PCR_CLK_EN     (1U << 0)
#define ESP32_SPI_PCR_RST_EN     (1U << 1)
#define ESP32_SPI_PCR_READY      (1U << 2)
#define ESP32_SPI_PCR_DIV_SHIFT  12
#define ESP32_SPI_PCR_DIV_MASK   (0xffU << ESP32_SPI_PCR_DIV_SHIFT)
#define ESP32_SPI_PCR_SEL_SHIFT  20
#define ESP32_SPI_PCR_SEL_MASK   (3U << ESP32_SPI_PCR_SEL_SHIFT)
#define ESP32_SPI_PCR_CLKM_EN    (1U << 22)

/* Use the whole buffer from W0, with both HIGHPART bits cleared. */
#define ESP32_SPI_BUFFER_WORDS 16
#define ESP32_SPI_BUFFER_SIZE  (ESP32_SPI_BUFFER_WORDS * sizeof(uint32_t))

struct esp32_spi {
	uintptr_t base_addr;
};

extern int esp32_spi_hw_init(struct esp32_spi *dev, unsigned int clock_div);

extern struct spi_controller_ops esp32_spic_ops;

#endif /* SRC_DRIVERS_SPI_ESP32_SPI_H_ */
